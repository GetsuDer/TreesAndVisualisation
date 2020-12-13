#include <string>
#include <cstdio>
#include <cassert>
#include <sys/mman.h>
#include <cmath>
#include <string.h>
#include <algorithm>

#include "tree.h"
#include "in_and_out.h"

int Node::id = 0;
constexpr double EPS = 1e-7;

//! \brief Check for equality of two values
//! \param [in] val1,val2 Values to check
//! \return Return result
bool
is_eq(double val1, double val2) {
    return fabs(val1 - val2) < EPS;
}
//! \brief Node constructor for operations
//! \param [in] _operation Operation identificator
Node::Node(int _operation) {
    children_number = 0;
    node_id = id;
    id++;
    childs = NULL;
    parent = NULL;
    operation = _operation;
    name_len = 1;
    name = (char *) calloc(name_len, sizeof(char));
    switch (operation) {
        case MUL:
            strncpy(name, "*", name_len);
            break;
        case DIV:
            strncpy(name, "/", name_len);
            break;
        case SUB:
            strncpy(name, "-", name_len);
            break;
        case ADD:
            strncpy(name, "+", name_len);
            break;
        case POWER:
            strncpy(name, "^", name_len);
            break;
        case LN:
            name_len = 2;
            name = (char *)realloc(name, name_len * sizeof(char));
            strncpy(name, "ln", name_len);
            break;
        case COS:
            name_len = 3;
            name = (char *)realloc(name, name_len * sizeof(char));
            strncpy(name, "cos", name_len);
            break;
        case SIN:
            name_len = 3;
            name = (char *)realloc(name, name_len * sizeof(char));
            strncpy(name, "sin", name_len);
            break;
        case VAR:
            strncpy(name, "x", name_len);
            break;
        case ASSIGNMENT:
            strncpy(name, "=", name_len);
            break;
        case MORE:
            strncpy(name, ">", name_len);
            break;
        case LESS:
            strncpy(name, "<", name_len);
            break;
        case EQ:
            strncpy(name, "~", name_len);
            break;
        case IF:
            name_len = 2;
            name = (char *)realloc(name, name_len * sizeof(char));
            strncpy(name, "if", name_len);
            break;
        case WHILE:
            name_len = 5;
            name = (char *)realloc(name, name_len * sizeof(char));
            strncpy(name, "while", name_len);
            break;
        case FOR:
            name_len = 3;
            name = (char *)realloc(name, name_len * sizeof(char));
            strncpy(name, "while", name_len);
            break;
        case DO_IN_ORDER:
            name_len = 2;
            name = (char *)realloc(name, name_len * sizeof(char));
            strncpy(name, "do", name_len);
            break;
        case FUNC_CALL:
            strncpy(name, "?", name_len);
            break;
        case FUNC_DEF:
            strncpy(name, "?", name_len);
            break;
        case RETURN:
            name_len = 6;
            name = (char *)realloc(name, name_len * sizeof(char));
            strncpy(name, "return", name_len);
            break;
        default:
            name_len = 0;
            free(name);
            name = NULL;
            break;
    }
}


//! \brief Change node type
//! \param [in] _operation new type
//! \return Return true, if this change is permitted
bool
Node::change_operation(int _operation) {
    if (operation == VAR && (_operation == FUNC_CALL || _operation == FUNC_DEF)) {
        operation = _operation;
        return true;
    }
   return false; 
}
//! \brief Node constructor for vars (to make possible different vars)
//! \param [in] _operation Operation identificator
//! \param [in] _name Oprator name
Node::Node(int _operation, char *_name) {
    children_number = 0;
    node_id = id;
    id++;
    childs = NULL;
    parent = NULL;
    operation = _operation;
    name_len = strlen(_name);
    name = (char *)calloc(name_len + 1, sizeof(char *));
    strncpy(name, _name, name_len + 1);
}

//! \brief Node constructor for constants
//! \param [in] _value Value for constant
Node::Node(double _value) {
    children_number = 0;
    node_id = id;
    id++;
    childs = NULL;
    parent = NULL;
    operation = CONSTANT;
    value = _value;
    name_len = 0;
    name = NULL;
}

//! \brief Node destructor
Node::~Node() {
    free(childs);
}

//! \brief Value getter
double
Node::get_value() {
    return value;
}
//! \brief children_number getter
//! \return Returns number of node children
int
Node::get_children_number() {
    return children_number;
}

//! \brief Operation getter
//! \return Returns operation code
int 
Node::get_operation() {
    return operation;
}

//! \brief Write description of node in graphivz-readable format
//! \param [in] fd File descriptor
//! \return Returns 0 in success -1 else
int
Node::visualize(int fd) {
    assert(fd > 0);
    if (operation == CONSTANT) {
        dprintf(fd, "%lf", value);
    } else {
        dprintf(fd, "%s", name);
    }
    return 0;
}
//! \brief Calculate value using specified operation
//! \param [in] operation Operation
//! \param [in] res Result
//! \param [in] op1,op2 operands 
static void 
calculate(int operation, double *res, double operand) {
    switch (operation) {
        case ADD:
            *res += operand;
            break;
        case SUB:
            *res -= operand;
            break;
        case MUL:
            *res *= operand;
            break;
        case DIV:
            *res /= operand;
            break;
        case POWER:
            *res = pow(*res, operand);
            break;
        case LN:
            *res = log(operand);
            break;
        case SIN:
            *res = sin(operand);
            break;
        case COS:
            *res = cos(operand);
            break;
        case VAR: //do nothing. may be changed in the future
            break;
        default:
            break;
    }
    return;
}
//! \brief Recursive function for tree visualization generation
//! \param [in] fd File descriptor
//! \return Returns counted value
double
Node::visualize_tree_rec(int fd) {
    dprintf(fd, "%d [style = filled, label=\"", node_id);
    visualize(fd);
    double res = 0;
    if (operation) {
        dprintf(fd, "\", shape = box, fillcolor=");
        switch(operation) {
            case VAR:
                dprintf(fd, "\"green\"");
                break;
            case FUNC_CALL:
                dprintf(fd, "\"blue\"");
                break;
            case FUNC_DEF:
                dprintf(fd, "\"pink\"");
                break;
            case RETURN:
                dprintf(fd, "\"red\"");
                break;
            default:
                dprintf(fd, "\"grey\"");
                break;
        }
        dprintf(fd, "];\n");
    } else {
        res = value;
        dprintf(fd, "\", fillcolor=\"yellow\"];\n");
        return res;
    }
    for (int i = 0; i < get_children_number(); i++) {
        dprintf(fd, "%d->%d;\n", node_id, childs[i]->node_id);
        if (i) {
            calculate(operation, &res, childs[i]->visualize_tree_rec(fd));
        } else {
            if (operation == SIN || operation == COS || operation == LN) {
                calculate(operation, &res, childs[i]->visualize_tree_rec(fd));
            } else {
                res = childs[i]->visualize_tree_rec(fd);
            }
        }
    }
    return res;
}

//! \brief childs getter
//! \return Returns pointer to pointers to childs
Node **
Node::get_childs() {
    return childs;
}
//! \brief Writes tree description in dot-readable format
//! \param [in] fd File descriptor
//! \return Returns 0 in success, -1 else
int
Node::export_dot(int fd, char *graph_name) {
    assert(fd >= 0);

    dprintf(fd, "digraph ");
    if (graph_name) {
        dprintf(fd, "%s {\n", graph_name);
    } else {
        dprintf(fd, "G {\n");
    }
    double res = visualize_tree_rec(fd);
    if (is_constant()) {
        dprintf(fd, "\"result=%lf\" [shape=box];", res);
    }
    dprintf(fd, "\n}\n");
    return 0;
}

double
Node::visualize_tree_rec_tex(int fd) {
    double res = 0;
    if (operation && operation != VAR) {
        dprintf(fd, "(");
        if (operation == DIV || operation == POWER) {
            for (int i = 0; i < children_number; i++) {
                dprintf(fd, "{");
            }
        }
        if (operation == COS || operation == SIN) {
            visualize(fd);
            dprintf(fd, "(");
        }
        if (operation == LN) {
            dprintf(fd, "\\ln{");
        }
        for (int i = 0; i < children_number; i++) {
            if (i) {
                if (operation == DIV) {
                    dprintf(fd, "\\over {");
                } else {
                    visualize(fd);
                }
                if (operation == POWER) {
                    dprintf(fd, "{");
                }
                calculate(operation, &res, childs[i]->visualize_tree_rec_tex(fd));
            } else {
                if (operation == SIN || operation == COS || operation == LN) {
                    calculate(operation, &res, childs[i]->visualize_tree_rec_tex(fd));
                } else {
                    res = childs[i]->visualize_tree_rec_tex(fd);
                }
            }
            if (operation == DIV || operation == POWER || operation == LN) {
                dprintf(fd, "}");
            }
            if (i && ((operation == DIV) || (operation == POWER))) {
                dprintf(fd, "}");
            }
        }
        if (operation == SIN || operation == COS) {
            dprintf(fd, ")");
        }
        dprintf(fd, ")");
    } else {
        visualize(fd);
        res = value;
    }
    return res;
}


//! \brief Writes tree desription in tex format
//! \param [in] fd File decriptor
//! \return Return 0 in success, -1 else
int Node::export_tex(int fd) {
    assert(fd >= 0);
    dprintf(fd, "$$ ");
    double res = visualize_tree_rec_tex(fd);
    if (is_constant()) {
        dprintf(fd, " = %lf ", res);
    }
    dprintf(fd, "$$ \n \\end");
    return 0;
}

//! \brief Made link between two nodes by adding one node as child to another
//! \param [in] child Pointer to new child node
//! \return Returns 0 in success -1 else
int
Node::add_child(Node *child) {
    children_number++;
    Node **tmp = (Node **)realloc(childs, children_number * sizeof(*tmp));
    if (!tmp) {
        fprintf(stderr, "Memory Allocation Error during add_child\n");
        return -1;
    }
    childs = tmp;
    childs[children_number - 1] = child;
    child->parent = this;
    return 0;
}

//! \brief Skip space symbols
//! \param [in,out] begin Begining of the symbols. Is shifted to the first non-space value
//! \param [in] end First incorrect symbol
static void
skip(char **begin, char *end) {
    while (*begin < end && isspace(**begin)) (*begin)++;
    return;
}

//! \brief Recursively clear tree
//! \param [in] root Tree root
void
rec_del(Node *root) {
    if (!root) {
        return;
    }
    for (int i = 0; i < root->get_children_number(); i++) {
        rec_del(root->get_childs()[i]);
    }
    delete root;
    return;
}

//! \brief Recognize operation
//! \param [in,out] operation Operation to recognize
//! \return Return operation identificator
static int
find_operation(char **operation) {
    switch (**operation) { //in future operation len may be more than one!
        case '*':
            (*operation)++;
            return MUL;
        case '+':
            (*operation)++;
            return ADD;
        case '-':
            (*operation)++;
            return SUB;
        case '/':
            (*operation)++;
            return DIV;
        case '^':
            (*operation)++;
            return POWER;
        case 's':
            if (!strncmp(*operation, "sin", 3)) return -1;
            (*operation) += 3;
            return SIN;
        case 'c':
            if (!strncmp(*operation, "cos", 3)) return -1;
            (*operation) += 3;
            return COS;
        case 'l':
            if (!strncmp(*operation, "ln", 2)) return -1;
            (*operation) += 2;
            return LN;
        default:
            return -1;
    }
    return -1;
}

//! \brief Recursively parse input tree
//! \param [in,out] begin Pointer to first symbol
//! \param [in] end Pointer after last correct symbol
//! \return Returns root of the parsed tree
static Node*
parse_rec(char **begin, char *end) {
    skip(begin, end);
    if (*begin >= end) {
        return NULL;
    }
    if (**begin != '(') {
        fprintf(stderr, "Wrong input file format: %s\n Expected '('!\n", *begin);
        return NULL;
    }
    (*begin)++; //skip '(' at the beginning of the node: ->(<- ... ) 
    skip(begin, end);
    if (*begin >= end) {
        fprintf(stderr, "Wrong input file format: %s\n Expected symbol\n", *begin);
        return NULL;
    }
    if (**begin == '(') { //not constant
        Node *parent = NULL;
        Node *child = parse_rec(begin, end); // ( ->child1<- op child2 ... )
        //now should parse childs: ( (child1) op (child2) op ... )
        if (!child) {
            fprintf(stderr, "Parsing error in : %s\n", *begin);
            return NULL;
        }
        while (child) {
            skip(begin, end); // ( .... ->op<- .... )
            if (*begin >= end) {
                fprintf(stderr, "No operation in input file: %s\n", *begin);
                rec_del(child);
                return NULL;
            }
        //find operation
            if (**begin == ')') { // ( ... ->)<-
                parent->add_child(child);
                break; //no more childs
            }
            int operation = find_operation(begin);
            if (operation < 0) {
                fprintf(stderr, "Can not work with this operation or can not recognize: %c\n", **begin);
                rec_del(child);
                return NULL;
            }
            if (!parent) {
                parent = new Node(operation);
            } else {
                if (operation != parent->get_operation()) {
                    fprintf(stderr, "Can not parse more than one operation types in node: %s\n", *begin);
                }
            }
            parent->add_child(child);
            child = parse_rec(begin, end);
        }
        if (!parent) {
            rec_del(child);
        }
        skip(begin, end);
        (*begin)++; // last ')' in node parent
        return parent;

    } else { //constant or sin or cos or ln or var
        if (!strncmp(*begin, "sin", 3)) { // ( sin ( ... ) )
            Node *parent = new Node(SIN);
            (*begin) += 3;
            parent->add_child(parse_rec(begin, end));
            skip(begin, end);
            (*begin)++; // ')'
            return parent;
        }
        if (!strncmp(*begin, "cos", 3)) { // ( cos ( ... ) )
            Node *parent = new Node(COS);
            (*begin) += 3;
            parent->add_child(parse_rec(begin, end));
            skip(begin, end);
            (*begin)++; // ')'
            return parent;
        }
        if (!strncmp(*begin, "ln", 2)) {
            Node *parent = new Node(LN);
            (*begin) += 2;
            parent->add_child(parse_rec(begin, end));
            skip(begin, end);
            (*begin)++; // ')';
            return parent;
        }
        char *endptr = NULL;
        errno = 0;
        double value = strtod(*begin, &endptr);
        if (!errno &&  endptr != *begin) {
            *begin = endptr;
            skip(begin, end);
            if (*begin >= end) {
                fprintf(stderr, "Wrong input file format: %s\n Expected ')'\n", *begin);
                return NULL;
            }
            (*begin)++;
            return new Node(value);
        }
        // now we suppose var type with unknown yet name.
        int name_len = 0;
        char *tmp = *begin;
        while (tmp < end && isalnum(*tmp)) {
            name_len++;
            tmp++;
        }
        if (!name_len) {
            fprintf(stderr, "Wrong input file format: can not recoginse var name %s\n", *begin);
            return NULL;
        }
        char *name = (char *)calloc(name_len + 1, sizeof(char));
        strncpy(name, *begin, name_len);
        Node *parent = new Node(VAR, name);
        (*begin) += name_len; // name
        skip(begin, end);
        (*begin)++;
        return parent;
    }


}

//! \brief Read expression from file and create tree
//! \param [in] filename Input file
//! \return Returns root of created tree or NULL if unsuccess
Node *
parse_file_create_tree(char *filename) {
    assert(filename);
    if (!filename) {
        fprintf(stderr, "No input file\n");
        return NULL;
    }
    int file_size = 0;
    char *exp = mmap_file(filename, &file_size);
    if (!exp) {
        fprintf(stderr, "Can not mmap input file %s\n", filename);
        return NULL;
    }
    char *old_exp = exp;
    Node *root = parse_rec(&exp, exp + file_size);
    munmap(old_exp, file_size);
    return root;
}

//! \brief Copy tree
//! \return Returns root of the copied tree
Node *
Node::copy() {
    Node *root = NULL;
    if (operation) {
        if (name) {
            root = new Node(operation, name);
        } else {
            root = new Node(operation);
        }
    } else {
        root = new Node(value);
    }
    for (int i = 0; i < children_number; i++) {
        root->add_child(childs[i]->copy());
    }
    return root;
}


//! \brief Find, if the expression is constant (can be calculated). It means no VAR nodes
//! \return Returns true, if no VAR nodes, false else
bool
Node::is_constant() {
    if (operation == VAR) {
        return false;
    }
    for (int i = 0; i < children_number; i++) {
        if (!childs[i]->is_constant()) {
            return false;
        }
    }
    return true;
}

//! \brief Create new tree with derivate of old tree
//! \return Returns root of the new tree
Node *
Node::derivate(char *var_name) {
    Node *root = NULL;
    Node *tmp = NULL;
    int var_name_len = 0;
    switch (operation) {
        case CONSTANT:
            root = new Node(0.0);
            break;
        case VAR:
            var_name_len = strlen(var_name);
            if ((var_name_len != name_len) || strncmp(name, var_name, var_name_len)) {
                root = new Node(0.0);
                break;
            }
            root = new Node(1.0);
            break;
        case ADD:
            root = new Node(ADD); // (a + b + c)` = a` + b` + c`
            for (int i = 0; i < children_number; i++) {
                root->add_child(childs[i]->derivate(var_name));
            }
            break;
        case SUB:
            root = new Node(SUB); // (a - b - c)` = a` - b` - c`
            for (int i = 0; i < children_number; i++) {
                root->add_child(childs[i]->derivate(var_name));
            }
            break;
        case MUL:
            root = new Node(ADD); // (a * b * c)` = a` * b * c + a * b` * c + a * b * c`
            for (int i = 0; i < children_number; i++) {
                root->add_child(new Node(MUL));
                for (int j = 0; j < children_number; j++) {
                    root->childs[i]->add_child((i == j) ? (childs[j]->derivate(var_name)) : (childs[j]->copy()));
                }
            }
            break;
        case DIV: // (a / b)` = (a` * b - a * b`) / (b * b)
           // (a / b / c)` = ((a / b) / c)` = (((a / b)` * c - (a / b) * c`)) / (c * c) 
            root = new Node(DIV);
            if (children_number > 2) {
                tmp = new Node(DIV);
                for (int i = 0; i < children_number - 1; i++) {
                    tmp->add_child(childs[i]->copy());
                }
            } else {
                tmp = childs[0]->copy();
            }
            //now take derivate as for (a / b)
            root->add_child(new Node(SUB)); // see above: (a` * b) - (a * b`)
            root->childs[0]->add_child(new Node(MUL)); // a` * b
            root->childs[0]->add_child(new Node(MUL)); // a * b`
            root->childs[0]->childs[0]->add_child(tmp->derivate(var_name)); // a`
            root->childs[0]->childs[0]->add_child(childs[children_number - 1]->copy()); // b
            root->childs[0]->childs[1]->add_child(tmp->copy()); // a
            root->childs[0]->childs[1]->add_child(childs[children_number - 1]->derivate(var_name)); // b`
            delete tmp;
            tmp = NULL; // may be better make b ^ 2? 
            root->add_child(new Node(MUL)); // (b * b)
            root->childs[1]->add_child(childs[children_number - 1]->copy()); // b
            root->childs[1]->add_child(childs[children_number - 1]->copy()); // b
            break;
        case POWER: // three ways: f(x) ^ C, C ^ f(x), f(x) ^ g(x)
            // (a ^ b ^ c)` = ((a ^ b) ^ c)` (as for DIVision)
            if (children_number > 2) {
                tmp = new Node(POWER);
                for (int i = 0; i < children_number - 1; i++) {
                    tmp->add_child(childs[i]);
                }
            } else {
                tmp = childs[0]->copy();
            }
            if (tmp->is_constant()) { // (C ^ x)` = (ln C) * (C ^ x) * x`
                root = new Node(MUL);
                root->add_child(new Node(LN)); // ln
                root->childs[0]->add_child(tmp->copy()); // ln C

                root->add_child(copy()); // C ^ x
                
                root->add_child(childs[children_number - 1]->derivate(var_name));
                break;
            }
            if (childs[children_number - 1]->is_constant()) { // (x ^ C)` = C * (x ^ (C - 1)) * x`
                root = new Node(MUL);
                root->add_child(childs[children_number - 1]->copy()); // C
                
                root->add_child(new Node(POWER));
                root->childs[1]->add_child(tmp->copy()); // x ^
                root->childs[1]->add_child(new Node(SUB)); // (C - 1)
                root->childs[1]->childs[1]->add_child(childs[children_number - 1]->copy()); // C
                root->childs[1]->childs[1]->add_child(new Node(1.0)); // 1

                root->add_child(tmp->derivate(var_name)); // x`
                break;
            }
            // (f(x) ^ g(x))` = (f ^ g) * (g` * ln(f) + g / f * f`) = 
            // = (f ^ g) * g` * ln(f) + (f ^ (g - 1)) * g * f`
            root = new Node(ADD); // +

            root->add_child(new Node(MUL)); // *
            root->childs[0]->add_child(copy()); // f ^ g
            root->childs[0]->add_child(childs[children_number - 1]->derivate(var_name)); // g`
            root->childs[0]->add_child(new Node(LN)); // ln
            root->childs[0]->childs[2]->add_child(tmp->copy()); // ln f

            root->add_child(new Node(MUL)); // *
            root->childs[1]->add_child(new Node(POWER)); // f ^ (g - 1)

            root->childs[1]->childs[0]->add_child(tmp->copy()); // f
            root->childs[1]->childs[0]->add_child(new Node(SUB)); // g - 1
            root->childs[1]->childs[0]->childs[1]->add_child(childs[children_number - 1]->copy()); // g
            root->childs[1]->childs[0]->childs[1]->add_child(new Node(1.0)); // 1
            
            root->childs[1]->add_child(tmp->copy()); // g
            root->childs[1]->add_child(childs[children_number - 1]->derivate(var_name)); // f`
            
            break;
        case LN: // (ln x)` = (1 / x) * x`
            root = new Node(MUL);
            root->add_child(new Node(DIV)); // 1 / x
            root->childs[0]->add_child(new Node(1.0)); // 1
            root->childs[0]->add_child(childs[0]->copy()); // x
            
            root->add_child(childs[0]->derivate(var_name)); // x`
            break;
        case SIN: // (sin x)` = (cos x) * x`
            root = new Node(MUL);
            root->add_child(new Node(COS)); // cos
            root->childs[0]->add_child(childs[0]->copy()); // x
            root->add_child(childs[0]->derivate(var_name)); // x`
            break;
        case COS: // (cos x)` = - sin(x) * x` = (-1) * sin (x) * x`
            root = new Node(MUL);
            root->add_child(new Node(-1.0));
            root->add_child(new Node(SIN)); // sin
            root->childs[1]->add_child(childs[0]->copy()); // x
            root->add_child(childs[0]->derivate(var_name)); // x`
            break;
        default:
            fprintf(stderr, "Derivate error: unknown operation %d\n", operation);
            break;
    }
    return root;
}

//! \brief Get expression value
//! \return Returns value (NAN, if not-Constant, but caller is responsible for it)
double
Node::get_val() {
    if (!operation) {
        return value;
    }
    double res = 0;
    if (operation == VAR) {
        fprintf(stderr, "Try to get val from non-constant expression\n");
        return NAN;
    }
    if (operation == LN) {
        return log(childs[0]->get_val());
    }
    if (operation == SIN) {
        return sin(childs[0]->get_val());
    }
    if (operation == COS) {
        return cos(childs[0]->get_val());
    }
    // from this place all operations have two and more operands
    res = childs[0]->get_val();   
    for (int i = 1; i < children_number; i++) {
        calculate(operation, &res, childs[i]->get_val());
    }
    return res;
}

//! \brief Cut child 
//! \param [in] Child index
//! \return Returns cutted child
Node *
Node::cut_child(int child_ind) {
    if (child_ind < 0 || child_ind > children_number) {
        return NULL;
    }
    Node *cutted = childs[child_ind];
    cutted->parent = NULL;
    for (int i = child_ind; i < children_number - 1; i++) {
        childs[i] = childs[i + 1];
    }
    children_number--; // now have more memory, than necessary for this number of childs
    Node **tmp = (Node **)realloc(childs, sizeof(*tmp) * children_number);
    if (children_number && !tmp) {
        fprintf(stderr, "Memory reallocation error in cut_child for child_ind %d\n", child_ind);
        return cutted; // Do not break *this
    } 
    childs = tmp;
    return cutted;
}

//! \brief Remove neitral elements
void
Node::remove_neitrals() {
    if (operation != SUB && operation != DIV && operation != POWER && operation != ADD && operation != MUL) {
        return;
    }
    int ind = (operation == SUB || operation == DIV || operation == POWER) ? 1 : 0;
    double neitral = get_neitral(operation);
    Node *tmp = NULL;
    while (ind < children_number) {
        if (childs[ind]->operation == CONSTANT && is_eq(neitral, childs[ind]->value)) {
            free(cut_child(ind));
        } else {
            ind++; 
        } 
    }
    if (!children_number) { // all childs were neitral elements
        operation = CONSTANT;
        value = neitral;
        free(name);
        name = NULL;
        name_len = 0;
        return;
    }
    if (children_number == 1) { // only one childs is alive
        tmp = cut_child(0);
        operation = tmp->operation;
        free(name);
        name = tmp->name;
        name_len = tmp->name_len;
        int children_number_old = tmp->children_number;
        for (int i = 0; i < children_number_old; i++) {
            add_child(tmp->cut_child(0));
        }
        value = tmp->value;
        free(tmp);
    }
    return;
}

//! \brief Some algebraic rules
void
Node::specific_simpling() {
    if (operation == DIV) {
        if (childs[0]->operation == CONSTANT && is_eq(0.0, childs[0]->value)) {
// 0 / ... = 0 (if ... = 0, expression is illegal)
            int children_number_old = children_number;
            for (int i = 0; i < children_number_old; i++) {
                rec_del(cut_child(0));
            }
            operation = CONSTANT;
            free(name);
            name = NULL;
            name_len = 0;
            value = 0.0;
        }
        return;
    }    
    if (operation == MUL) {
        for (int i = 0; i < children_number; i++) {
            if (childs[i]->operation == CONSTANT && is_eq(0.0, childs[i]->value)) {
// 0 * x = 0
                int children_number_old = children_number;
                for (int j = 0; j < children_number_old; j++) {
                    rec_del(cut_child(0));
                }
                operation = CONSTANT;
                free(name);
                name = NULL;
                name_len = 0;
                value = 0.0;
                return;
            }

        }
    }

    if (operation == POWER) {
        if (childs[0]->operation == CONSTANT && is_eq(0.0, childs[0]->value)) {
// 0 ^ x = 0
            int children_number_old = children_number;
            for (int i = 0; i < children_number_old; i++) {
                rec_del(cut_child(0));
            }
            operation = CONSTANT;
            name_len = 0;
            free(name);
            name = NULL;
            value = 0.0;
            return;
        }
        for (int i = 1; i < children_number; i++) {
            if (childs[i]->operation == CONSTANT && is_eq(0.0, childs[i]->value)) {
                // x ^ 0 = 1
                int children_number_old = children_number;
                for (int j = 0; j < children_number_old; j++) {
                    rec_del(cut_child(0));
                }
                operation = CONSTANT;
                free(name);
                name = NULL;
                name_len = 0;
                value = 1.0;
                return;
            }
        }
    }
    return;
}



//! \brief If there is no VARs in the tree, value can be calculated directly
void
Node::calculate_values() {
    if (is_constant()) {
        double res = get_val();
        int children_number_old = children_number;
        for (int i = 0; i < children_number_old; i++) {
            rec_del(cut_child(0));
        }
        operation = CONSTANT;
        free(name);
        name = NULL;
        name_len = 0;
        value = res;
    }
    return;
}

//! \brief Check if two trees are the same
//! \param [in] other Other tree
//! \return Returns true if the same, false else
bool
Node::tree_eq(Node *other) {
    if (!other) {
        return false;
    }
    if (operation != other->operation) {
        return false;
    }
    if (name_len != other->name_len) {
        return false;
    }
    if (strncmp(name, other->name, name_len)) {
        return false;
    }
    if (children_number != other->children_number) {
        return false;
    }
    for (int i = 0; i < children_number; i++) {
        if (!childs[i]->tree_eq(other->childs[i])) {
            return false;
        }
    }
    return true;
}

//! \brief For MUL, ADD, SUB, POW union layers
void
Node::union_layers() {
    int old_num = 0;
    Node *tmp = NULL;
    switch(operation) {
        case MUL:
        case ADD:
            old_num = children_number;
            for (int i = 0; i < old_num; i++) {
                if (childs[i]->operation == operation) {
                    tmp = childs[i];
                    childs[i] = tmp->childs[0];
                    childs[i]->parent = this;
                    for (int j = 1; j < tmp->children_number; j++) {
                        add_child(tmp->childs[j]);
                    }
                    free(tmp->name);
                    free(tmp);
                    tmp = NULL;
                }
            }
            return;
        case SUB:
            old_num = children_number;
            for (int i = 1; i < old_num; i++) {
                if (childs[i]->operation == ADD) {
                    tmp = childs[i];
                    childs[i] = tmp->childs[0];
                    childs[i]->parent = this;
                    for (int j = 1; j < tmp->children_number; j++) {
                        add_child(tmp->childs[j]);
                    }
                    free(tmp->name);
                    free(tmp);
                    tmp = NULL;
                    continue;
                }
            }
            return;
        case POWER: // (x ^ y) ^ z = x ^ y ^ x, but x ^ (y ^ z) != x ^ y ^ z
            if (childs[0]->operation == POWER) {
                tmp = childs[0];
                childs[0] = tmp->childs[0];
                tmp->childs[0]->parent = this;
                for (int i = 1; i < tmp->children_number; i++) {
                    add_child(tmp->childs[i]);
                }
                free(tmp->name);
                free(tmp);
            }
        default:
            return;
    }
    return;
}

//! \brief Get neitral element
//! \param [in] operation Operation
//! \return Returns neitral element as int
int
get_neitral(int operation) {
    switch (operation) {
        case ADD:
        case SUB:
            return 0;
        case MUL:
        case DIV:
        case POWER:
            return 1;
        default:
            return -1;
    }
    return -1;
}


//! \brief Get operation for (op1 op op2 op op3) = ((op1) op (op2 op_op op3))
int 
get_opposite(int operation) {
    switch (operation) {
        case ADD:
            return ADD;
        case SUB:
            return ADD;
        case DIV:
            return MUL;
        case MUL:
            return MUL;
        case POWER:
            return MUL;
        default:
            return -1;
    }
}

//! \brief 1 + 1 --> 2
void
Node::transform_constants() {
    if (operation != ADD && operation != SUB && operation != MUL && operation != DIV && operation != POWER) {
        return;
    }
    if (children_number <= 2) {
        return; // will be calculated in calculate_values, if possible
    }
    int num = 0;
    for (int i = 0; i < children_number; i++) {
        if (childs[i]->operation == CONSTANT) {
            num++;
        }
    }
    if (num <= 1) {
        return;
    }
    double res = get_neitral(operation);
    int ind = 1;
    int op = get_opposite(operation);
    // constants
    while (ind < children_number) {
        if (childs[ind]->operation == CONSTANT) {
            calculate(op, &res, childs[ind]->value);
            cut_child(ind);
        } else {
            ind++;
        }
    }
    if (childs[0]->operation == CONSTANT) {
        calculate(operation, &(childs[0]->value), res); 
    } else {
        add_child(new Node(res));
    }
    return;
}

char **
Node::get_node_vars(int *var_num) {
    *var_num = 0;
    char **var_names = NULL;
    for (int i = 0; i < children_number; i++) {
        if (childs[i]->operation == VAR) {
            bool flag = false;
            for (int j = 0; j < *var_num; j++) {
                if (!strcmp(childs[i]->name, var_names[j])) {
                    flag = true;
                    break;
                }
            }
            if (!flag) {
                (*var_num)++;
                var_names = (char **)realloc(var_names, (*var_num) * sizeof(char *)); 
                var_names[*var_num - 1] = childs[i]->name;
            }
        }
    }
    return var_names;
}


//! \brief x + x --> x * 2; x - x --> x * 0; x * x --> x ^ 2;
void
Node::transform_vars(char *var_name) {
   if (operation != ADD && operation != SUB && operation != MUL) {
       return;
   }
   int var_num = 0;
   int ind = (operation == SUB) ? 1 : 0;
   int var_name_len = strlen(var_name);
   while (ind < children_number) {
       if (childs[ind]->operation == VAR && childs[ind]->name_len == var_name_len && 
               !strncmp(childs[ind]->name, var_name, var_name_len)) {
            var_num++;
            cut_child(ind);
       } else {
           ind++;
       }
   }  
   if (var_num == 1 && operation != SUB) { // x - x will be later
       add_child(new Node(VAR, var_name)); 
       return;
   }
   if (var_num == 0) {
       return;
   }
   Node *tmp = NULL;
   switch (operation) {
       case ADD:
           tmp = new Node(MUL);
           tmp->add_child(new Node(VAR, var_name));
           tmp->add_child(new Node((double)var_num));
           break;
       case MUL:
           tmp = new Node(POWER);
           tmp->add_child(new Node(VAR, var_name));
           tmp->add_child(new Node((double)var_num));
           break;
       case SUB: 
           tmp = new Node(MUL); // 1 - nx
           tmp->add_child(new Node(VAR, var_name));
           if (childs[0]->operation == VAR && childs[0]->name_len == var_name_len &&
                   !strncmp(childs[0]->name, var_name, var_name_len)) {
               // y - y - y
            // x - (n - 1)x = x * (2 - n)            
               tmp->add_child(new Node((double)(1 - var_num)));
               free(childs[0]->name);
               free(childs[0]);
               childs[0] = tmp;
               childs[0]->parent = this;
               if (children_number == 1) {
                   operation = MUL;
                   tmp = cut_child(0);
                   add_child(tmp->childs[0]);
                   add_child(tmp->childs[1]);
                   free(tmp);
               }
           } else {
               tmp->add_child(new Node((double)var_num));
               add_child(tmp); 
           }
           return;
   } 
   if (!children_number) {
       add_child(tmp->childs[0]);
       add_child(tmp->childs[1]);
       operation = tmp->operation;
       name = tmp->name;
       name_len = tmp->name_len;
       free(tmp);
   } else {
       add_child(tmp);
   }
   return;
    
}

//! \brief Getter for var name
char *
Node::get_name() {
    return name;
}

//! \brief Getter for name len
int 
Node::get_name_len() {
    return name_len;
}

//! \brief x * a
//! \param [in] node x * a
//! \return node == x * a
static bool
var_mul_coef(Node *node, char *var_name) {
    int var_name_len = strlen(var_name);
    if (node->get_operation() == VAR && !strncmp(node->get_name(), var_name, var_name_len)) return true;
    if (node->get_operation() != MUL) return false;
    if (node->get_children_number() != 2) return false;
    Node *first = node->get_childs()[0];
    Node *second = node->get_childs()[1]; 
    if ((first->get_operation() == VAR && first->get_name_len() == var_name_len && !strncmp(first->get_name(), var_name, var_name_len) && second->get_operation() == CONSTANT) || (first->get_operation() == CONSTANT && second->get_operation() == VAR && second->get_name_len() == var_name_len && !strncmp(second->get_name(), var_name, var_name_len))) {
        return true;
    }
    return false;
}


//! \brief x * a --> a
//! \param [in] node x * a
//! \return a
static double
get_coef(Node *node) {
    if (node->get_operation() == VAR) return 1.0;

    assert(node->get_children_number() == 2);
    Node **childs = node->get_childs();
    if (childs[0]->get_operation() == CONSTANT) {
        return childs[0]->get_value();
    } else {
        return childs[1]->get_value();
    }
}

//! \brief Work with x * a + x * b etc
// x * a + x * b = x * (a + b)
// x * a - x * b = x * (a - b)
// (x * a) / (x * b) = a / b
// A / (x * a) / (x * b) = A / (x * x * a * b)
void
Node::simp_var(char *var_name) {
    int var_name_len = strlen(var_name);
    double res = 0;
    int ind;
    int flag = 0;
    int first = -1;
    switch (operation) {
        case ADD:
            ind = 0;
            while (ind < children_number) {
                if (var_mul_coef(childs[ind], var_name)) {
                    res += get_coef(childs[ind]);
                    flag++;
                    if (flag == 1) {
                        first = ind;
                        ind++;
                    } else {
                        cut_child(ind);
                    }
                } else {
                    ind++;
                }
            }
            if (flag > 1) {
                if (childs[first]->childs[0]->operation == VAR && childs[first]->childs[0]->name_len == var_name_len
                        && !strncmp(childs[first]->childs[0]->name, var_name, var_name_len)) {
                    childs[first]->childs[1]->value = res;
                } else {
                    childs[first]->childs[0]->value = res;
                }
            }
            break;
        case SUB:
            res = 0;
            ind = 1;
            while (ind < children_number) {
                if (var_mul_coef(childs[ind], var_name)) {
                    res += get_coef(childs[ind]);
                    flag = true; 
                    cut_child(ind);
                } else {
                    ind++;
                }
            }
            if (flag) {
               if (var_mul_coef(childs[0], var_name)) {
                   res = get_coef(childs[ind]) - res;
                   free(childs[0]);
                   childs[0]->operation = MUL;
                   childs[0]->add_child(new Node(VAR, var_name));
                   childs[0]->add_child(new Node(res));
               } else {
                   add_child(new Node(MUL));
                   childs[children_number - 1]->add_child(new Node(VAR, var_name));
                   childs[children_number - 1]->add_child(new Node(res));
               } 
            }
        default:
            break;
    }
    return;
}

//! \brief Try to simplify expression
void
Node::simplify() {
    Node *old = NULL;
    do {
        if (operation == CONSTANT || operation == VAR) {
            return;
        }
        for (int i = 0; i < children_number; i++) {
            childs[i]->simplify();
        }
        rec_del(old);
        old = copy();
        remove_neitrals();    
        specific_simpling();
        calculate_values();
        union_layers();
        transform_constants();
        int var_num = 0;
        char **vars = get_node_vars(&var_num);
        for (int i = 0; i < var_num; i++) {
            transform_vars(vars[i]);
            simp_var(vars[i]);
        }
    } while (!tree_eq(old));
    return;
}
