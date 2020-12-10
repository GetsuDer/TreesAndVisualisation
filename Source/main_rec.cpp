#include <cstdio>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <malloc.h>

#include "rec_desc.h"


int
main(int argc, char **argv) {
    if (argc <= 1) {
        fprintf(stderr, "No input file\n");
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
    double val = Parse_All(expr_str, str_size);
    fprintf(stdout, "Res = %lf\n", val);

    return 0;
}
