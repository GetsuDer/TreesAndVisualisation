#include <cstdio>
#include <ctype.h>
#include <cmath>
#include <cstring>

#include "tree.h"

#define REQUIRE(a, env) \
    { if ((env)->current_ind >= (env)->str_size) (env)->error = NO_SYMBOL;\
      if ((env)->str[(env)->current_ind] != a) (env)->error = WRONG_SYMBOL;\
    }  

#define NEXT(a, env) ((env)->current_ind < (env)->str_size && (env)->str[(env)->current_ind] == a)

#define CHECK_ENV(env) \
     if ((env)->current_ind >= (env)->str_size) (env)->error = NO_SYMBOL;\
     if ((env)->error != OK) return NULL;


#define NEED_WORD(a, env) if ((env)->str_size - (env)->current_ind < (int)strlen(a) || strncmp((env)->str + (env)->current_ind, a, strlen(a))) (env)->error = WRONG_SYMBOL;

Node *GetSum(struct Env *env);
Node *GetExpression(struct Env *env);
Node *GetStatement(struct Env *env);
Node *GetSequence(struct Env *env);

//! \brief Skip space symbols for expression string
//! \param [in] env String and linked vars
static void
skip_spaces(struct Env *env) {
    while (env->current_ind < env->str_size && 
            isspace(env->str[env->current_ind])) {
        env->current_ind++;
    }
    return;
}

//! \brief Read unsigned integer
//! \param [in] env String and linked vars
//! \return Return read integer 
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
    if (!isdigit(env->str[env->current_ind])) {
        env->error = WRONG_SYMBOL;
        return 0;
    }
    while (isdigit(env->str[env->current_ind])) {
        val = val * 10 + (env->str[env->current_ind] - '0');
        env->current_ind++;
    }
    return val;
}

//! \brief Read double
//! \param [in] env String and linked vars
//! \return Returns tree node with read double
Node *
GetDouble(struct Env *env) {
    CHECK_ENV(env);

    double res = GetNumber(env);    
    if (NEXT('.', env)) {
        env->current_ind++;
        int old_ind = env->current_ind;
        res += GetNumber(env) / pow(10, env->current_ind - old_ind);
    }
    skip_spaces(env);
    return new Node(res);
}

//! \brief Get identificator [a-zA-Z][a-zA-Z0-9]*
//! \param [in] env String and linked vars
//! \return Return resulting tree node
Node *
GetId(struct Env *env) {
    CHECK_ENV(env);
    
    skip_spaces(env);
    if (!isalpha(env->str[env->current_ind])) {
        env->error = WRONG_SYMBOL;
        return NULL;
    }

    int id_length = 1;
    char *id = (char *)calloc(ID_NAME_SIZE + 1, sizeof(char));
    id[0] = env->str[env->current_ind];
    env->current_ind++;
    while(isalnum(env->str[env->current_ind])) {
        id[id_length] = env->str[env->current_ind];
        env->current_ind++;
        id_length++;
        if (id_length > ID_NAME_SIZE) {
            env->error = TOO_LONG_ID;
            return NULL;
        }
    }
    skip_spaces(env);
    Node *root = new Node(VAR, id);
    free(id);
    return root;
}


//! \brief Get assigment identificator = calculated_expression
//! \param [in] env String and linked vars
//! \param [in] env String and linked vars
Node *
Assignment(struct Env *env) {
    CHECK_ENV(env);
    
    skip_spaces(env);
    Node *id = GetId(env);

    skip_spaces(env);
    REQUIRE('=', env);
    if (env->error != OK) {
        free(id);
        return NULL;
    }

    env->current_ind++;
    
    Node *value = GetExpression(env);
    
    if (env->error != OK) {
        free(value);
        free(id);
        return NULL;
    }

    Node *root = new Node(ASSIGNMENT);
    root->add_child(id);
    root->add_child(value);
    
    skip_spaces(env);
    return root;
}

Node *
GetFuncCall(struct Env *env) {
    CHECK_ENV(env);

    Node *root = GetId(env);
    if (env->error != OK) {
        rec_del(root);
        return NULL;
    }

    root->change_operation(FUNC_CALL);
    skip_spaces(env);
    REQUIRE('(', env);
    if (env->error != OK) {
        rec_del(root);
        return NULL;
    }
    env->current_ind++;

    Node *tmp = NULL;
    while (true) {
        int old_ind = env->current_ind;
        tmp = GetExpression(env);
        if (env->error != OK) {
            env->current_ind = old_ind;
            env->error = OK;
            break;
        }
        root->add_child(tmp);
        skip_spaces(env);
        if (env->str[env->current_ind] != ',') {
            break;
        }
        env->current_ind++;
    }

    skip_spaces(env);
    REQUIRE(')', env);
    env->current_ind++;
    return root;
}

//! \brief Get expression (expr) | double | func(expr)
//! \param [in] env String and linked vars
//! \return Returns root of the resulting tree
Node *
GetPart(struct Env *env) {
    CHECK_ENV(env);
    
    Node *root = NULL;
    skip_spaces(env);
    if (env->str[env->current_ind] == '(') {
        env->current_ind++;
        root = GetExpression(env);
        skip_spaces(env);
        REQUIRE(')', env);
        env->current_ind++;
        skip_spaces(env);
        return root;
    }
    int old_ind = env->current_ind;
    root = Assignment(env);
    if (env->error == OK) {
        return root;
    }
    env->error = OK;
    env->current_ind = old_ind;
    rec_del(root);
    root = GetDouble(env);
    if (env->error == OK) {
        return root;
    }
    env->error = OK;
    env->current_ind = old_ind;
    rec_del(root);

    root = GetFuncCall(env);
    if (env->error == OK) {
        return root;
    }
    env->error = OK;
    env->current_ind = old_ind;
    root = GetId(env);
    return root;
}


//! \brief Parse expr (^ expr)*
//! \param [in] env String and linked vars
//! \return Return root of the resulting tree 
Node *
GetPower(struct Env *env) {
    if (env->error != OK) {
        return NULL;
    }
    if (env->current_ind >= env->str_size) {
        env->error = NO_SYMBOL;
        return NULL;
    }
    skip_spaces(env);
    Node *root = GetPart(env);
    Node *tmp1 = NULL;
    skip_spaces(env);
    while (true) {
        switch(env->str[env->current_ind]) {
            case '^':
                tmp1 = root;
                env->current_ind++;
                skip_spaces(env);
                root = new Node(POWER);
                root->add_child(tmp1);
                root->add_child(GetPart(env));
                return root;
            default:
                return root;        
        }

    }

}


//! \brief Read 'expr ['*', '/'] expr*'
//! \param [in] env String and linked vars
//! \return Return root of the resulting tree
Node *
GetMul(struct Env *env) {
    if (env->error != OK) {
        return NULL;
    }
    if (env->current_ind >= env->str_size) {
        env->error = NO_SYMBOL;
        return NULL;
    }
    skip_spaces(env);
    Node *root = GetPower(env);
    Node *tmp1 = NULL, *tmp2 = NULL;
    skip_spaces(env);
    while (true) {
        switch(env->str[env->current_ind]) {
            case '*':
                env->current_ind++;
                tmp2 = GetPower(env);
                tmp1 = root;
                root = new Node(MUL);
                root->add_child(tmp1);
                root->add_child(tmp2);
                skip_spaces(env);
                break;
            case '/':
                env->current_ind++;
                tmp2 = GetPower(env);
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


//! \brief Read 'expr ['+', '-'] expr*'
//! \param [in] env String and linked vars
//! \return Return resulting tree
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
GetExpression(struct Env *env) {
    CHECK_ENV(env);
    
    Node *root = GetSum(env);
    
    CHECK_ENV(env);

    Node *tmp2 = NULL;

    while (true) {
        tmp2 = root;
        skip_spaces(env);
        switch(env->str[env->current_ind]) {
            case '>':
                env->current_ind++;
                root = new Node(MORE);
                break;
            case '<':
                env->current_ind++;
                root = new Node(LESS);
                break;
            case '~':
                env->current_ind++;
                root = new Node(EQ);
                break;
            default:
                return root;
        }
        root->add_child(tmp2);
        root->add_child(GetSum(env));
    }

}


Node *
GetIf(struct Env *env) {
    CHECK_ENV(env);
    
    skip_spaces(env);

    NEED_WORD("if", env);
    env->current_ind += strlen("if");

    CHECK_ENV(env);

    Node *root = new Node(IF);
    skip_spaces(env);
    Node *if_statement = GetExpression(env);
    if (env->error != OK) {
        rec_del(root);
        rec_del(if_statement);
        return NULL;
    }
    root->add_child(if_statement);
    skip_spaces(env);
    REQUIRE('{', env);
    if (env->error != OK) {
        rec_del(root);
        return NULL;
    }
    env->current_ind++;
    Node *then_do = GetSequence(env);
    skip_spaces(env);
    REQUIRE('}', env);
    if (env->error != OK) {
        rec_del(root);
        rec_del(then_do);
        return NULL;
    }
    env->current_ind++;
    root->add_child(then_do);
    skip_spaces(env);

    NEED_WORD("else", env);
    if (env->error == OK) {
        env->current_ind += strlen("else");
        skip_spaces(env);
        REQUIRE('{', env);
        if (env->error != OK) {
            rec_del(root);
            return NULL;
        }
        env->current_ind++;
        Node *else_do = GetSequence(env);
        root->add_child(else_do);
        skip_spaces(env);
        REQUIRE('}', env);
        if (env->error) {
            rec_del(root);
            return NULL;
        }
        env->current_ind++;
    } else {
        env->error = OK;
    }
    return root;
}


Node *
GetWhile(struct Env *env) {
    CHECK_ENV(env);

    skip_spaces(env);
    NEED_WORD("while", env);
    if (env->error == OK) {
        env->current_ind += strlen("while");
        skip_spaces(env);
        Node *root = new Node(WHILE);
        Node *while_cond = GetExpression(env);
        if (env->error != OK) {
            rec_del(while_cond);
            rec_del(root);
            return NULL;
        }
        root->add_child(while_cond);
        skip_spaces(env);
        REQUIRE('{', env);
        if (env->error) {
            rec_del(root);
            return NULL;
        }
        env->current_ind++;
        Node *while_do = GetSequence(env);
        if (env->error != OK) {
            rec_del(root);
            rec_del(while_do);
            return NULL;
        }
        root->add_child(while_do);
        skip_spaces(env);
        REQUIRE('}', env);
        if (env->error) {
            rec_del(root);
            return NULL;
        }
        env->current_ind++;
        return root;
    } else {
        env->error = WRONG_SYMBOL;
        return NULL;
    }
}

Node *
GetReturn(struct Env *env) {
    CHECK_ENV(env);

    skip_spaces(env);
    NEED_WORD("return", env);
    if (env->error) {
        return NULL;
    }
    env->current_ind += strlen("return");
    Node *root = new Node(RETURN);
    skip_spaces(env);
    REQUIRE('(', env);
    env->current_ind++;
    if (env->error) {
        rec_del(root);
        return NULL;
    }
    
    Node *tmp = GetExpression(env);
    if (env->error) {
        rec_del(tmp);
        rec_del(root);
        return NULL;
    }
    root->add_child(tmp);
    skip_spaces(env);
    REQUIRE(')', env);
    env->current_ind++;
    return root;
}

Node *
GetFor(struct Env *env) {
    CHECK_ENV(env);
    skip_spaces(env);
    NEED_WORD("for", env);
    if (env->error) {
        return NULL;
    }
    env->current_ind += strlen("for");
    Node *root = new Node(FOR);
    Node *tmp = NULL;

    skip_spaces(env);
    REQUIRE('(', env);
    env->current_ind++;
    for (int i = 0; i < 3; i++) {
        tmp = GetExpression(env);
        if (env->error) {
            rec_del(tmp);
            rec_del(root);
            return NULL;
        }
        root->add_child(tmp);
        skip_spaces(env);
        if (i != 2) {
            REQUIRE(';', env);
            env->current_ind++;
        }
    }

    skip_spaces(env);
    REQUIRE(')', env);
    env->current_ind++;

    skip_spaces(env);
    REQUIRE('{', env);
    env->current_ind++;
    
    tmp = GetSequence(env);
    if (env->error) {
        rec_del(tmp);
        rec_del(root);
        return NULL;
    }
    root->add_child(tmp);
    skip_spaces(env);
    REQUIRE('}', env);
    env->current_ind++;
    return root;
}
Node *
GetStatement(struct Env *env) {
    CHECK_ENV(env);

    int old_ind = env->current_ind;
    Node *root = GetIf(env);
    if (env->error == OK) {
        return root;
    }
    rec_del(root);
    env->current_ind = old_ind;
    env->error = OK;

    root = GetWhile(env);
    if (env->error == OK) {
        return root;
    }
    rec_del(root);
    env->current_ind = old_ind;
    env->error = OK;
    
    root = GetReturn(env);
    skip_spaces(env);
    REQUIRE(';', env);
    env->current_ind++;
    if (env->error == OK) {
        return root;
    }
    rec_del(root);
    env->current_ind = old_ind;
    env->error = OK;

    root = GetFor(env);
    if (env->error == OK) {
        return root;
    }
    rec_del(root);
    env->current_ind = old_ind;
    env->error = OK;

    root = GetExpression(env);
    skip_spaces(env);
    REQUIRE(';', env);
    env->current_ind++;
    return root;      
}


Node *
GetSequence(struct Env *env) {
    CHECK_ENV(env);

    Node *root = new Node(DO_IN_ORDER);

    while (true) {
        int old_ind = env->current_ind;
        Node *tmp = GetStatement(env);
        if (env->error != OK) {
            env->current_ind = old_ind;
            env->error = OK;
            rec_del(tmp);
            return root;
        }
        root->add_child(tmp);
    }
    return root;
}

Node *
GetFuncDef(struct Env *env) {
    CHECK_ENV(env);

    skip_spaces(env);
    NEED_WORD("function", env);
    CHECK_ENV(env);

    env->current_ind += strlen("function");

    Node *root = GetId(env);
    if (env->error) {
        rec_del(root);
        return NULL;
    }

    root->change_operation(FUNC_DEF);
    skip_spaces(env);
    REQUIRE('(', env);
    env->current_ind++;

    if (env->error) {
        rec_del(root);
        return NULL;
    }

    Node *tmp = NULL;
    
    while (true) {
        int old_ind = env->current_ind;
        tmp = GetId(env);
        if (env->error) {
            env->current_ind = old_ind;
            env->error = OK;
            break;
        }
        root->add_child(tmp);
        skip_spaces(env);
        if (env->str[env->current_ind] != ',') {
            break;
        }
        env->current_ind++;
    }
    
    skip_spaces(env);
    REQUIRE(')', env);
    env->current_ind++;

    if (env->error) {
        rec_del(root);
        return NULL;
    }

    skip_spaces(env);
    REQUIRE('{', env);
    env->current_ind++;

    tmp = GetSequence(env);
    if (env->error) {
        rec_del(tmp);
        rec_del(root);
        return NULL;
    }

    root->add_child(tmp);

    skip_spaces(env);
    REQUIRE('}', env);
    env->current_ind++;
    return root;
}



//! \brief Parse whole expression
//! \param [in] str String with expression
//! \param [in] str_length Expression string length
//! \return Return root of the resulting tree
Node *
Parse_All(char *str, int str_length) {
    struct Env env;
    env.str = str;
    env.current_ind = 0;
    env.str_size = str_length;
    env.error = OK;

    Node *root = new Node(DO_IN_ORDER);
    while (true) {
        int old_ind = env.current_ind;
        Node *tmp = GetFuncDef(&env);
        if (env.error != OK) {
            env.current_ind = old_ind;
            env.error = OK;
            break;
        }
        root->add_child(tmp);
    }
    skip_spaces(&env);
    REQUIRE('$', &env);
    if (env.error) {
        fprintf(stderr, "Error during recursive descent\n");
        root = NULL;
    }
    return root;
}
