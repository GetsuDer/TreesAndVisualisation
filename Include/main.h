#ifndef MAIN_H
#define MAIN_H
constexpr int ARG_NUM = 2;
constexpr int FILE_IN = 1;
constexpr mode_t out_mode = S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH | S_IWOTH;
const char DOT_COMMAND[] = "dot -Tpng -o ";
constexpr int dot_com_size = strlen(DOT_COMMAND);
const char EOG_COMMAND[] = "eog ";
constexpr int eog_com_size = strlen(EOG_COMMAND);
constexpr int DOT_PREF_SIZE = 5;
#endif
