#include <cstdio>
#include <ctype.h>
#include <cmath>

#include "tree.h"

#define REQUIRE(a, env) \
    { if ((env)->current_ind >= (env)->str_size) (env)->error = NO_SYMBOL;\
      if ((env)->str[(env)->current_ind] != a) (env)->error = WRONG_SYMBOL;\
    }  

#define NEXT(a, env) ((env)->current_ind < (env)->str_size && (env)->str[(env)->current_ind] == a)

Node *GetSum(struct Env *env);


static void
skip_spaces(struct Env *env) {
    while (env->current_ind < env->str_size && 
            isspace(env->str[env->current_ind])) {
        env->current_ind++;
    }
    return;
}

//! \brief Read unsigned integer
int 
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

Node *
GetPart(struct Env *env) {
    if (env->error != OK) {
        return NULL;
    }
    if (env->current_ind >= env->str_size) {
        env->error = NO_SYMBOL;
        return NULL;
    }
    Node *root = NULL;
    skip_spaces(env);
    switch(env->str[env->current_ind]) {
        case '(':
            env->current_ind++;
            root = GetSum(env);
            skip_spaces(env);
            REQUIRE(')', env);
            env->current_ind++;
            skip_spaces(env);
            return root;
        case 's':
            env->current_ind++;
            REQUIRE('i', env);
            env->current_ind++;
            REQUIRE('n', env);
            env->current_ind++;
            root = new Node(SIN);
            root->add_child(GetSum(env));
            skip_spaces(env);
            return root;

        case 'c':
            env->current_ind++;
            REQUIRE('o', env);
            env->current_ind++;
            REQUIRE('s', env);
            env->current_ind++;
            root = new Node(COS);
            root->add_child(GetPart(env));
            skip_spaces(env);
            return root;
        case 'l':
            env->current_ind++;
            REQUIRE('n', env);
            env->current_ind++;
            root = new Node(LN);
            root->add_child(GetPart(env));
            skip_spaces(env);
            return root;
        default:
            root = GetDouble(env);
            skip_spaces(env);
            return root;
    }
}


Node *
GetMul(struct Env *env) {
    if (env->error != OK) {
        return NULL;
    }
    if (env->current_ind >= env->str_size) {
        env->error = NO_SYMBOL;
        return NULL;
    }
    Node *root = GetPart(env);
    Node *tmp1 = NULL, *tmp2 = NULL;
    skip_spaces(env);
    while (true) {
        switch(env->str[env->current_ind]) {
            case '*':
                env->current_ind++;
                tmp2 = GetPart(env);
                tmp1 = root;
                root = new Node(MUL);
                root->add_child(tmp1);
                root->add_child(tmp2);
                skip_spaces(env);
                break;
            case '/':
                env->current_ind++;
                tmp2 = GetPart(env);
                tmp1 = root;
                root = new Node(DIV);
                root->add_child(tmp1);
                root->add_child(tmp2);
                skip_spaces(env);
                break;

            default:
                return root;
        }
    }    
}

Node *
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
    skip_spaces(env);
    while (true) {
        switch(env->str[env->current_ind]) {
            case '+':
                env->current_ind++;
                tmp2 = GetMul(env);   
                tmp = root;
                root = new Node(ADD);
                root->add_child(tmp);
                root->add_child(tmp2);
                skip_spaces(env);
                break;
            case '-':
                env->current_ind++;
                tmp2 = GetMul(env);
                tmp = root;
                root = new Node(SUB);
                root->add_child(tmp);
                root->add_child(tmp2);
                skip_spaces(env);
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
    skip_spaces(&env);
    REQUIRE('$', &env);
    if (env.error) {
        fprintf(stderr, "Error during recursive descent\n");
        root = NULL;
    }
    return root;
}
