#ifndef MAIN_H
#define MAIN_H
constexpr int ARG_NUM = 4;
constexpr int FILE_IN = 1;
constexpr int SHOW_PNG = 2;
constexpr int SHOW_PDF = 3;

constexpr mode_t out_mode = S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH | S_IWOTH;
constexpr int BUFFER_SIZE = 200;
#endif
