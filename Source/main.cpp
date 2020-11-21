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
    int out = open(argv[FILE_OUT], O_WRONLY | O_CREAT, out_mode);
    if (out < 0) {
        fprintf(stderr, "Can not open out file %s\n", argv[FILE_OUT]);
        return 1;
    }
    root->export_dot(out);
    close(out);
    rec_del(root);

    int commands_len = strlen(argv[FILE_OUT]) + strlen(argv[FILE_PNG]) + dot_com_size;
    
    char *commands_buffer = (char *)calloc(commands_len + 2, sizeof(*commands_buffer));
    snprintf(commands_buffer, commands_len + 2, "%s%s %s", DOT_COMMAND, argv[FILE_PNG], argv[FILE_OUT]);
    system(commands_buffer);
    
    commands_len = strlen(argv[FILE_PNG]) + eog_com_size;
    snprintf(commands_buffer, commands_len + 2, "%s %s", EOG_COMMAND, argv[FILE_PNG]);
    system(commands_buffer);

    free(commands_buffer);
    return 0;
}
