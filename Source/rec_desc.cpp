#include <cstdio>
#include <ctype.h>
#include <cmath>

#include "tree.h"

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
    if (env->error != OK) {
        return 0;
    }
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


Node *
GetDouble(struct Env *env) {
    if (env->error != OK) {
        return NULL;
    }
    skip_spaces(env);
    if (env->current_ind >= env->str_size) {
        env->error = NO_SYMBOL;
        return NULL;
    }

    double res = GetNumber(env);    
    if (NEXT('.', env)) {
        env->current_ind++;
        int old_ind = env->current_ind;
        res += GetNumber(env) / pow(10, env->current_ind - old_ind);
    }
    skip_spaces(env);
    return new Node(res);
}

static Node *
GetMul(struct Env *env) {
    if (env->error != OK) {
        return NULL;
    }
    if (env->current_ind >= env->str_size) {
        env->error = NO_SYMBOL;
        return NULL;
    }
    Node *root = GetDouble(env);
    Node *tmp1 = NULL, *tmp2 = NULL;

    while (true) {
        switch(env->str[env->current_ind]) {
            case '*':
                env->current_ind++;
                tmp2 = GetDouble(env);
                tmp1 = root;
                root = new Node(MUL);
                root->add_child(tmp1);
                root->add_child(tmp2);
                break;
            case '/':
                env->current_ind++;
                tmp2 = GetDouble(env);
                tmp1 = root;
                root = new Node(DIV);
                root->add_child(tmp1);
                root->add_child(tmp2);
                break;

            default:
                return root;
        }
    }    
}

static Node *
GetSum(struct Env *env) {
    if (env->error != OK) {
        return NULL;
    }
    if (env->current_ind >= env->str_size) {
        env->error = NO_SYMBOL;
        return NULL;
    }
    Node *root = GetMul(env);
    Node *tmp = NULL, *tmp2 = NULL;

    while (true) {
        switch(env->str[env->current_ind]) {
            case '+':
                env->current_ind++;
                tmp2 = GetMul(env);   
                tmp = root;
                root = new Node(ADD);
                root->add_child(tmp);
                root->add_child(tmp2);
                break;
            case '-':
                env->current_ind++;
                tmp2 = GetMul(env);
                tmp = root;
                root = new Node(SUB);
                root->add_child(tmp);
                root->add_child(tmp2);
                break;
            default:
                return root;
        }
    }
}

Node *
Parse_All(char *str, int str_length) {
    struct Env env;
    env.str = str;
    env.current_ind = 0;
    env.str_size = str_length;
    env.error = OK;
    
    Node *root = GetSum(&env);
    REQUIRE('$', &env);
    if (env.error) {
        fprintf(stderr, "Error during recursive descent\n");
        root = NULL;
    }
    return root;
}
