#include <cstdio>
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <malloc.h>
#include <cstdlib>

#include "tree.h"
#include "visualize.h"

int
main(int argc, char **argv) {
    if (argc <= 2) {
        fprintf(stderr, "No input file or no show parameter\n");
        return -1;
    }
    
    int fd = open(argv[1], O_RDONLY);
    if (fd < 0) {
        fprintf(stderr, "Can not open file %s\n", argv[1]);
        return -1;
    }
    
    struct stat file_stat;
    if (stat(argv[1], &file_stat)) {
        fprintf(stderr, "Can not get file size for file %s\n", argv[1]);
        return -1;
    }
    int str_size = file_stat.st_size + 1; // for '$' in the end
    char *expr_str = (char *)calloc(str_size, sizeof(char));
    if (!expr_str) {
        fprintf(stderr, "Can not allocate memory\n");
        return -1;
    }

    if (read(fd, expr_str, str_size - 1) != str_size - 1) {
        fprintf(stderr, "Can not read all expresion\n");
        return -1;
    }
    close(fd);

    expr_str[str_size - 1] = '$';
    Node *val = Parse_All(expr_str, str_size);

    errno = 0;
    int show = strtol(argv[2], NULL, 10);
    if (errno) {
        fprintf(stderr, "Please, specify second param as 0 or 1 to show or not show result\n");
        show = 0;
    }
    create_png(argv[1], val, show);
    create_pdf(argv[1], val, 0);
    return 0;
}
