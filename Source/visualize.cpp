#include <cstdio>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <cstdlib>
#include <cstring>

#include "tree.h"
#include "in_and_out.h"
#include "visualize.h"

int
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

int
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

