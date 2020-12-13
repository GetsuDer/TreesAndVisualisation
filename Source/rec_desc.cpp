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
     if (((env)->error == OK) && (env)->current_ind >= (env)->str_size) (env)->error = NO_SYMBOL;\
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
    if (env->error != OK) {
        return 0;
    }
    if (env->current_ind >= env->str_size) {
        env->error = NO_SYMBOL;
        return 0;
    }
    skip_spaces(env);
    int val = 0;
    if (!isdigit(env->str[env->current_ind])) {
        env->error = WRONG_SYMBOL;
        env->expected_symbol = 'd';
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
    
    skip_spaces(env);
    int sign = 1;
    if (env->str[env->current_ind] == '-') {
        sign = -1;
        env->current_ind++;
    }
    double res = GetNumber(env);    
    
    CHECK_ENV(env);

    if (NEXT('.', env)) {
        env->current_ind++;
        int old_ind = env->current_ind;
        res += GetNumber(env) / pow(10, env->current_ind - old_ind);
    }

    CHECK_ENV(env);
    return new Node(res * sign);
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
        env->expected_symbol = 'c';
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
    
    Node *id = GetId(env);
    
    if (env->error) {
        rec_del(id);
        return NULL;
    }

    skip_spaces(env);
    REQUIRE('=', env);
    if (env->error != OK) {
        rec_del(id);
        return NULL;
    }

    env->current_ind++;
    
    Node *value = GetExpression(env);
    
    if (env->error != OK) {
        rec_del(value);
        rec_del(id);
        return NULL;
    }

    Node *root = new Node(ASSIGNMENT);
    root->add_child(id);
    root->add_child(value);
    
    return root;
}


//! \brief Get func_name(params)
//! \param [in] env String and linked varse
//! \return Return root of the resulting tree
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
        env->expected_symbol = '(';
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
    if (env->error != OK) {
        rec_del(root);
        env->expected_symbol = ')';
        return NULL;
    }
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
        if (env->error != OK) {
            env->expected_symbol = ')';
            rec_del(root);
            return NULL;
        }
        env->current_ind++;
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
    CHECK_ENV(env);
    
    Node *root = GetPart(env);
    Node *tmp1 = NULL;
    while (true) {
        skip_spaces(env);
        switch(env->str[env->current_ind]) {
            case '^':
                tmp1 = root;
                env->current_ind++;
                root = new Node(POWER);
                root->add_child(tmp1);
                tmp1 = GetPart(env);
                if (env->error != OK) {
                    rec_del(root);
                    return NULL;
                }
                root->add_child(tmp1);
                break;
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
    CHECK_ENV(env);
    
    Node *root = GetPower(env);
    Node *tmp1 = NULL, *tmp2 = NULL;
    while (true) {
        skip_spaces(env);
        tmp1 = root;
        switch(env->str[env->current_ind]) {
            case '*':
                root = new Node(MUL);
                break;
            case '/':
                root = new Node(DIV);
                break;
            default:
                return root;
        }
        env->current_ind++;
        tmp2 = GetPower(env);
        if (env->error != OK) {
            rec_del(tmp2);
            rec_del(root);
            return NULL;
        }
        root->add_child(tmp1);
        root->add_child(tmp2);
    }    
}


//! \brief Read 'expr ['+', '-'] expr*'
//! \param [in] env String and linked vars
//! \return Return resulting tree
Node *
GetSum(struct Env *env) {
    CHECK_ENV(env);
    
    Node *root = GetMul(env);
    Node *tmp = NULL, *tmp2 = NULL;
    skip_spaces(env);
    while (true) {
        skip_spaces(env);
        tmp = root;
        switch(env->str[env->current_ind]) {
            case '+':
                root = new Node(ADD);
                break;
            case '-':
                root = new Node(SUB);
                break;
            default:
                return root;
        }
        env->current_ind++;
        tmp2 = GetMul(env);
        if (env->error != OK) {
            rec_del(tmp2);
            rec_del(root);
            return NULL;
        }
        root->add_child(tmp);
        root->add_child(tmp2);
    }
}


//! \brief Get expression | expr > expr | expr < expr | expr ~ expr etc
//! \param [in] env String and linked vars
//! \return Return root of the resulting tree
Node *
GetExpression(struct Env *env) {
    CHECK_ENV(env);
    
    Node *root = GetSum(env);
   
    if (env->error != OK) {
       rec_del(root);
       return NULL; 
    }

    Node *tmp2 = NULL;

    while (true) {
        tmp2 = root;
        skip_spaces(env);
        switch(env->str[env->current_ind]) {
            case '>':
                root = new Node(MORE);
                break;
            case '<':
                root = new Node(LESS);
                break;
            case '~':
                root = new Node(EQ);
                break;
            default:
                return root;
        }
        env->current_ind++;
        root->add_child(tmp2);
        tmp2 = GetSum(env);
        if (env->error != OK) {
            rec_del(root);
            return NULL;
        }
        root->add_child(tmp2);
    }
}

//! \brief Get if (condition) { ... } (else { ... })
//! \param [in] env String and linked vars
//! \return Return root of the resulting tree
Node *
GetIf(struct Env *env) {
    CHECK_ENV(env);
    
    skip_spaces(env);
    NEED_WORD("if", env);
    if (env->error != OK) {
        env->expected_symbol = 'i';
        return NULL;
    }

    env->current_ind += strlen("if");

    Node *root = new Node(IF);
   
    skip_spaces(env);
    REQUIRE('(', env);
    if (env->error != OK) {
        env->expected_symbol = '(';
        rec_del(root);
        return NULL;
    }
    env->current_ind++;

    Node *if_statement = GetExpression(env);
    
    if (env->error != OK) {
        rec_del(root);
        rec_del(if_statement);
        return NULL;
    }

    skip_spaces(env);
    REQUIRE(')', env);
    if (env->error != OK) {
        env->expected_symbol = ')';
        rec_del(root);
        rec_del(if_statement);
        return NULL;
    }
    env->current_ind++;

    root->add_child(if_statement);
    skip_spaces(env);
    REQUIRE('{', env);
    if (env->error != OK) {
        rec_del(root);
        env->expected_symbol = '{';
        return NULL;
    }

    env->current_ind++;
    Node *then_do = GetSequence(env);
    if (env->error != OK) {
        rec_del(then_do);
        rec_del(root);
        return NULL;
    }

    skip_spaces(env);
    REQUIRE('}', env);
    if (env->error != OK) {
        rec_del(root);
        rec_del(then_do);
        env->expected_symbol = '}';
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
            env->expected_symbol = '{';
            return NULL;
        }
        env->current_ind++;

        Node *else_do = GetSequence(env);
        if (env->error != OK) {
            rec_del(else_do);
            rec_del(root);
            return NULL;
        }
        
        root->add_child(else_do);
        skip_spaces(env);
        REQUIRE('}', env);
        if (env->error) {
            rec_del(root);
            env->expected_symbol = '}';
            return NULL;
        }
        env->current_ind++;
    } else {
        env->error = OK;
    }
    return root;
}

//! \brief Get while (cond) { ... }
//! \param [in] env String and linked vars
//! \return Return root of the resulting tree
Node *
GetWhile(struct Env *env) {
    CHECK_ENV(env);

    skip_spaces(env);
    NEED_WORD("while", env);
    if (env->error == OK) {
        env->current_ind += strlen("while");
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
            env->expected_symbol = '{';
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
            env->expected_symbol = '}';
            return NULL;
        }
        env->current_ind++;
        return root;
    } else {
        env->error = WRONG_SYMBOL;
        env->expected_symbol = 'w';
        return NULL;
    }
}


//! \brief Get return ( ... )
//! \param [in] env String and linked vars
//! \return Return root of the resulting tree
Node *
GetReturn(struct Env *env) {
    CHECK_ENV(env);

    skip_spaces(env);
    NEED_WORD("return", env);
    if (env->error) {
        env->expected_symbol = 'r';
        return NULL;
    }
    env->current_ind += strlen("return");
    Node *root = new Node(RETURN);

    skip_spaces(env);
    REQUIRE('(', env);
    
    if (env->error) {
        env->expected_symbol = '(';
        rec_del(root);
        return NULL;
    }
    
    env->current_ind++;
    Node *tmp = GetExpression(env);
    if (env->error) {
        rec_del(tmp);
        rec_del(root);
        return NULL;
    }
    root->add_child(tmp);
    skip_spaces(env);
    REQUIRE(')', env);
    if (env->error != OK) {
        rec_del(root);
        env->expected_symbol = ')';
        return NULL;
    }
    env->current_ind++;
    return root;
}

//! \brief Get for ( before; while; after ) { ... } 
//! \param [in] env String and linked vars
//! \return Return root of the resulting tree
Node *
GetFor(struct Env *env) {
    CHECK_ENV(env);

    skip_spaces(env);
    NEED_WORD("for", env);
    if (env->error) {
        env->expected_symbol = 'f';
        return NULL;
    }
    env->current_ind += strlen("for");
    Node *root = new Node(FOR);
    Node *tmp = NULL;

    skip_spaces(env);
    REQUIRE('(', env);
    if (env->error != OK) {
        rec_del(root);
        env->expected_symbol = '(';
        return NULL;
    }

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
            if (env->error != OK) {
                rec_del(root);
                env->expected_symbol = ';';
                return NULL;
            }
            env->current_ind++;
        }
    }

    skip_spaces(env);
    REQUIRE(')', env);
    if (env->error != OK) {
        rec_del(root);
        env->expected_symbol = ')';
        return NULL;
    }
    env->current_ind++;

    skip_spaces(env);
    REQUIRE('{', env);
    if (env->error != OK) {
        rec_del(root);
        env->expected_symbol = '{';
        return NULL;
    }
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
    if (env->error != OK) {
        env->expected_symbol = '}';
        rec_del(root);
        return NULL;
    }
    env->current_ind++;
    return root;
}

//! \brief Get if, while, for, return or expression
//! \param [in] env String and linked vars
//! \return Return root of the resulting tree
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
    if (env->error != OK) {
        env->expected_symbol = 's';
        rec_del(root);
        return NULL;
    }
    env->current_ind++;
    return root;      
}

//! \brief Get sequence of statements (operators etc)
//! \param [in] env String and linked vars
//! \return Return root of the resulting tree
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

//! \brief Get function definition: function func_name(param1, ... ) { ... }
//! \param [in] env String and linked vars
//! \return Return root of the resulting tree
Node *
GetFuncDef(struct Env *env) {
    CHECK_ENV(env);

    skip_spaces(env);
    NEED_WORD("function", env);
    if (env->error != OK) {
        env->expected_symbol = 'F';
        return NULL;
    }

    env->current_ind += strlen("function");

    Node *root = GetId(env);
    if (env->error) {
        rec_del(root);
        return NULL;
    }

    root->change_operation(FUNC_DEF);
    skip_spaces(env);
    REQUIRE('(', env);
    if (env->error != OK) {
        rec_del(root);
        env->expected_symbol = '(';
        return NULL;
    }
    env->current_ind++;

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
    if (env->error != OK) {
        rec_del(root);
        env->expected_symbol = ')';
        return NULL;
    }
    env->current_ind++;

    skip_spaces(env);
    REQUIRE('{', env);
    if (env->error != OK) {
        rec_del(root);
        env->expected_symbol = '{';
        return NULL;
    }

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
    if (env->error != OK) {
        rec_del(root);
        env->expected_symbol = '}';
        return NULL;
    }
    env->current_ind++;
    return root;
}

static void
long_error(char c) {
    switch (c) {
        case 'w':
            fprintf(stderr, "while");
            return;
        case 'f':
            fprintf(stderr, "for");
            return;
        case 'i':
            fprintf(stderr, "if");
            return;
        case 'F':
            fprintf(stderr, "function");
            return;
        default:
            fprintf(stderr, "%c", c);
            return;
    }
}


//! \brief Print error and error context
//! \param [in] String and linked vars
static void
show_error(struct Env *env) {
    switch (env->error) {
        case OK:
            return;
        case NO_SYMBOL:
            fprintf(stderr, "Expected: ");
            long_error(env->expected_symbol);
            fprintf(stderr, ", but got EOF\n"); 
            return;
        case WRONG_SYMBOL:
            fprintf(stderr, "Expected: ");
            long_error(env->expected_symbol);
            fprintf(stderr, ", got %c: %s\n", env->str[env->current_ind], env->str + env->current_ind);
            return;
        case TOO_LONG_ID:
            fprintf(stderr, "Too long identificator of var or function: %s\n", env->str + env->current_ind);
    }
    return;
}   



//! \brief Parse whole program
//! \param [in] str String with program
//! \param [in] str_length Program length
//! \return Return root of the resulting tree
Node *
Parse_All(char *str, int str_length) {
    struct Env env;
    env.str = str;
    env.current_ind = 0;
    env.str_size = str_length;
    env.error = OK;
    env.expected_symbol = 0;

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
        fprintf(stderr, "Error during recursive descent:\n");
        show_error(&env);
        root = NULL;
    }
    return root;
}
