#include <cstdio>
#include <ctype.h>

#include "rec_desc.h"

#define REQUIRE(a, env) \
    { if ((env)->current_ind >= (env)->str_size) (env)->error = NO_SYMBOL;\
      if ((env)->str[(env)->current_ind] != a) (env)->error = WRONG_SYMBOL;\
    }  

#define NEXT(a, env) ((env)->current_ind < (env)->str_size && (env)->str[(env)->current_ind] == a)

static void
skip_spaces(struct Env *env) {
    while (env->current_ind < env->str_size && 
            isspace(env->str[env->current_ind])) {
        env->current_ind++;
    }
    return;
}

static double
make_double(int first_part, int second_part) {
    double res = second_part;
    while (res > 1) {
        res /= 10;
    }
    return res + first_part;
}

//! \brief Read unsigned integer
static int 
GetN(struct Env *env) {
    if (env->current_ind >= env->str_size) {
        env->error = NO_SYMBOL;
    }
    int val = 0;
    while (env->str[env->current_ind] >= '0' && env->str[env->current_ind] <= '9') {
        val = val * 10 + (env->str[env->current_ind] - '0');
        env->current_ind++;
    }
    return val;
}

double
Parse_All(char *str, int str_length) {
    struct Env env;
    env.str = str;
    env.current_ind = 0;
    env.str_size = str_length;
    env.error = OK;
    
    skip_spaces(&env);
    int first_part = GetN(&env);
    int second_part = 0;
    if (NEXT('.', &env)) {
        env.current_ind++;
        second_part = GetN(&env);
    }
    double res = make_double(first_part, second_part); 
    skip_spaces(&env);

    REQUIRE('$', &env);
    if (env.error) {
        fprintf(stderr, "Error during recursive descent\n");
        res = 0;
    }
    return res;
}
