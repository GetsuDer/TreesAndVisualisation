#ifndef REC_DESC_H
#define REC_DESC_H

struct Env {
    const char *str;
    int current_ind;
    int str_size;
    int error;
};


enum Env_Errors {
    OK = 0,
    NO_SYMBOL,
    WRONG_SYMBOL
};

double Parse_All(char *str, int str_length);
#endif
