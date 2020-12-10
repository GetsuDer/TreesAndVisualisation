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
#include "visualize.h"

int
main(int argc, char **argv)
{
    if (argc < ARG_NUM) {
        fprintf(stderr, "Not enough input arguments: need in file and two flags: open png and open pdf\n");
        return 1;
    }

    errno = 0;
    int show_png = strtol(argv[SHOW_PNG], NULL, 10);
    if (errno) {
        fprintf(stderr, "Wrong input argument %s: expected int\n", argv[SHOW_PNG]);
        return 1;
    }
    
    int show_pdf = strtol(argv[SHOW_PDF], NULL, 10);
    if (errno) {
        fprintf(stderr, "Wrong input argument %s: expected int\n", argv[SHOW_PDF]);
    }

    int file_name_size = strlen(argv[FILE_IN]);
    // root->simplify()
    char *simp_name = (char *)calloc(1, file_name_size + 5);
    strncpy(simp_name, argv[FILE_IN], file_name_size);
    strncpy(simp_name + file_name_size, "_simp", 6);
    
    // root->derivate()
    char *der_name = (char *)calloc(1, file_name_size + 4);
    strncpy(der_name, argv[FILE_IN], file_name_size);
    strncpy(der_name + file_name_size, "_der", 5);

    Node *root = parse_file_create_tree(argv[FILE_IN]);

    create_png(argv[FILE_IN], root, show_png);
    create_pdf(argv[FILE_IN], root, show_pdf);
    
    root->simplify();
    create_png(simp_name, root, show_png);
    create_pdf(simp_name, root, show_pdf);

    char var[] = "x";
    Node *der = root->derivate(var);
    der->simplify();
    create_png(der_name, der, show_png);
    create_pdf(der_name, der, show_pdf);
    
    rec_del(root);
    rec_del(der);
    return 0;
}
