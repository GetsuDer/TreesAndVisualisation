#include <cstdio>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <cstring>
#include <stdlib.h>

#include "tree.h"
#include "main.h"

int
main(int argc, char **argv)
{
    if (argc < ARG_NUM) {
        fprintf(stderr, "Specify in and out files, please\n");
        return 1;
    }
    Node *root = parse_file_create_tree(argv[FILE_IN]);

    int base_file_name = strlen(argv[FILE_IN]);

    char *file_out = (char *)calloc(1, base_file_name + DOT_PREF_SIZE);
    if (!file_out) {
        fprintf(stderr, "Memory error\n");
        rec_del(root);
        return 1;
    }

    snprintf(file_out, base_file_name + DOT_PREF_SIZE, "%s.dot", argv[FILE_IN]);
    fprintf(stderr, "%s\n", file_out);
    int out = open(file_out, O_WRONLY | O_CREAT | O_TRUNC, out_mode);
    if (out < 0) {
        fprintf(stderr, "Can not open out file %s\n", file_out);
        return 1;
    }
    root->export_dot(out);
    close(out);
    rec_del(root);

    int commands_len = base_file_name * 2 + DOT_PREF_SIZE * 2 + dot_com_size;
    char *commands_buffer = (char *)calloc(commands_len + 2, sizeof(*commands_buffer));

    snprintf(commands_buffer, commands_len + 2, "%s%s.png %s.dot", DOT_COMMAND, argv[FILE_IN], argv[FILE_IN]);
    fprintf(stderr, "%s\n", commands_buffer);
    system(commands_buffer);
    
    commands_len = base_file_name + DOT_PREF_SIZE + eog_com_size;
    snprintf(commands_buffer, commands_len + 2, "%s %s.png", EOG_COMMAND, argv[FILE_IN]);
    fprintf(stderr, "%s\n", commands_buffer);

    system(commands_buffer);
    
    free(commands_buffer);
    return 0;
}
