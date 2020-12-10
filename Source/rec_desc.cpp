#include <cstdio>
#include <ctype.h>
#include <cmath>

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

//! \brief Read unsigned integer
static int 
GetNumber(struct Env *env) {
    skip_spaces(env);

    if (env->current_ind >= env->str_size) {
        env->error = NO_SYMBOL;
        return 0;
    }
    int val = 0;
    while (env->str[env->current_ind] >= '0' && env->str[env->current_ind] <= '9') {
        val = val * 10 + (env->str[env->current_ind] - '0');
        env->current_ind++;
    }
    return val;
}


static double
GetDouble(struct Env *env) {
    skip_spaces(env);
    if (env->current_ind >= env->str_size) {
        env->error = NO_SYMBOL;
        return NAN;
    }
    double res = GetNumber(env);
    
    if (NEXT('.', env)) {
        env->current_ind++;
        int old_ind = env->current_ind;
        res += GetNumber(env) / pow(10, env->current_ind - old_ind);
    }
    return res;
}

static double 
GetSum(struct Env *env) {
    if (env->current_ind >= env->str_size) {
        env->error = NO_SYMBOL;
        return NAN;
    }
    double res = GetDouble(env);
    while (true) {
        skip_spaces(env);
        switch(env->str[env->current_ind]) {
            case '+':
                env->current_ind++;
                res += GetDouble(env);   
                break;
            case '-':
                env->current_ind++;
                res -= GetDouble(env);
                break;
            default:
                return res;
        }
    }
}

double
Parse_All(char *str, int str_length) {
    struct Env env;
    env.str = str;
    env.current_ind = 0;
    env.str_size = str_length;
    env.error = OK;
    
    double res = GetSum(&env);
    REQUIRE('$', &env);
    if (env.error) {
        fprintf(stderr, "Error during recursive descent\n");
        res = 0;
    }
    return res;
}
