#include <cstdio>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <cstring>
#include <stdlib.h>
#include <cerrno>

#include "tree.h"
#include "main.h"
#include "in_and_out.h"

static int
create_png(char *filename, Node *root, bool show) {
    if (!root) {
        return 1;
    }

    int base_file_name = strlen(filename);

    char *file_out = (char *)calloc(1, base_file_name + sizeof(".dot") + 1); //do not forget \0 in the end
    if (!file_out) {
        fprintf(stderr, "Can not allocate memory\n");
        rec_del(root);
        return 1;
    }

    snprintf(file_out, base_file_name + sizeof(".dot"), "%s.dot", filename);
    int out = open(file_out, O_WRONLY | O_CREAT | O_TRUNC, out_mode);
    if (out < 0) {
        fprintf(stderr, "Can not open out file %s\n", file_out);
        return 1;
    }

    root->export_dot(out);
    close(out);

    char *commands_buffer = (char *)calloc(BUFFER_SIZE, sizeof(char));
    if (!commands_buffer) {
        fprintf(stderr, "Memory allocation error\n");
        return 1;
    }

//create png
    snprintf(commands_buffer, BUFFER_SIZE, "dot -Tpng -o%s.png %s.dot", filename, filename);
    system(commands_buffer);
//open png
    if (show) {
        snprintf(commands_buffer, BUFFER_SIZE, "eog %s.png", filename);
        system(commands_buffer);
    }
    free(commands_buffer);
    return 0;
}

static int
create_pdf(char *filename, Node *root, int show) {
    if (!root) {
        return 1;
    }

    int base_file_name = strlen(filename);

    char *file_out = (char *)calloc(base_file_name + sizeof(".tex"), sizeof(char));
    if (!file_out) {
        fprintf(stderr, "Memory allocation error\n");
        return 1;
    }
   
    snprintf(file_out, base_file_name + sizeof(".tex"), "%s.tex", filename);

    int fd = open(file_out, O_WRONLY | O_CREAT | O_TRUNC, out_mode);
    if (fd < 0) {
        fprintf(stderr, "Can not open file %s\n", file_out);
        return 1;
    }
    
    root->export_tex(fd);
    close(fd);

    char *commands_buffer = (char *)calloc(BUFFER_SIZE, sizeof(char));
    if (!commands_buffer) {
        fprintf(stderr, "Memory allocation error\n");
        return 1;
    }

    snprintf(commands_buffer, BUFFER_SIZE, "pdftex %s.tex > pdftex_out; rm pdftex_out", filename);
    system(commands_buffer);

    if (show) {
        snprintf(commands_buffer, BUFFER_SIZE, "gio open %s.pdf", filename);
        system(commands_buffer);
    }
    return 0;

}


int
main(int argc, char **argv)
{
    if (argc < ARG_NUM) {
        fprintf(stderr, "Not enough input arguments: need in file and two flags: open png and open pdf\n");
        return 1;
    }

    errno = 0;
    int show = strtol(argv[SHOW_PNG], NULL, 10);
    if (errno) {
        fprintf(stderr, "Wrong input argument %s: expected int\n", argv[SHOW_PNG]);
        return 1;
    }

    Node *root = parse_file_create_tree(argv[FILE_IN]);

    create_png(argv[FILE_IN], root, show);

    Node *der = root->derivate();
   
    int file_name_size = strlen(argv[FILE_IN]);
    char *der_name = (char *)calloc(1, file_name_size + 4);
    strncpy(der_name, argv[FILE_IN], file_name_size);
    strncpy(der_name + file_name_size, "_der", 5);

    create_png(der_name, der, show);

    char *simp_name = (char *)calloc(1, file_name_size + 5);
    strncpy(simp_name, argv[FILE_IN], file_name_size);
    strncpy(simp_name + file_name_size, "_simp", 6);

    Node *tmp = root->copy();
    root->simplify();
    Node *root_simp = root;
    root = tmp;

    create_png(simp_name, root_simp, show);

    char *simp_der_name = (char *)calloc(1, file_name_size + 5 + 4);
    strncpy(simp_der_name, simp_name, file_name_size + 5);
    strncpy(simp_der_name + file_name_size + 5, "_der", 5);

    Node *der_simp = root_simp->derivate();

    create_png(simp_der_name, der_simp, show);

    errno = 0;
    show = strtol(argv[SHOW_PDF], NULL, 10);
    if (errno) {
        fprintf(stderr, "Wrong inpus argument %s: expected int\n", argv[SHOW_PDF]);
        return 1;
    }

    create_pdf(argv[FILE_IN], root, show);
    create_pdf(der_name, der, show);
    create_pdf(simp_name, root_simp, show);
    create_pdf(simp_der_name, der_simp, show);

    rec_del(root);
    rec_del(der);
    rec_del(root_simp);
    rec_del(der_simp);
    return 0;
}
