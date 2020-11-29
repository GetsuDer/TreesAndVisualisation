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
}

//! \brief Node destructor
Node::~Node() {
    free(childs);
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
    switch (this->operation) {
        case CONSTANT: 
            dprintf(fd, "%lf", this->value);
            break;
        case MUL:
            dprintf(fd, "*");
            break;
        case DIV:
            dprintf(fd, "/");
            break;
        case ADD:
            dprintf(fd, "+");
            break;
        case SUB:
            dprintf(fd, "-");
            break;
        case POWER:
            dprintf(fd, "^");
            break;
        case LN:
            dprintf(fd, "ln");
            break;
        case SIN:
            dprintf(fd, "sin");
            break;
        case COS:
            dprintf(fd, "cos");
            break;
        case VAR:
            dprintf(fd, "x");
            break;
        default:
            fprintf(stderr, "Wrong operation: %d\n", this->operation);
            return -1;
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
        if (operation == VAR) {
            dprintf(fd, "\"green\"");
        } else {
            dprintf(fd, "\"grey\"");
        }
        dprintf(fd, "];\n");
    } else {
        res = this->value;
        dprintf(fd, "\", fillcolor=\"yellow\"];\n");
        return res;
    }
    for (int i = 0; i < this->get_children_number(); i++) {
        dprintf(fd, "%d->%d;\n", this->node_id, this->childs[i]->node_id);
        if (i) {
            calculate(this->operation, &res, this->childs[i]->visualize_tree_rec(fd));
        } else {
            res = this->childs[i]->visualize_tree_rec(fd);
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
    double res = this->visualize_tree_rec(fd);
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
                res = childs[i]->visualize_tree_rec_tex(fd);
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
    double res = this->visualize_tree_rec_tex(fd);
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
           // (*begin)++; //skip operation (may be incorrect??)
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
        (*begin)++; // last ')' in node parent
        return parent;

    } else { //constant or sin or cos or ln or x
        if (!strncmp(*begin, "sin", 3)) { // ( sin ( ... ) )
            Node *parent = new Node(SIN);
            (*begin) += 3;
            parent->add_child(parse_rec(begin, end));
            (*begin)++; // ')'
            return parent;
        }
        if (!strncmp(*begin, "cos", 3)) { // ( cos ( ... ) )
            Node *parent = new Node(COS);
            (*begin) += 3;
            parent->add_child(parse_rec(begin, end));
            (*begin)++; // ')'
            return parent;
        }
        if (!strncmp(*begin, "ln", 2)) {
            Node *parent = new Node(LN);
            (*begin) += 2;
            parent->add_child(parse_rec(begin, end));
            (*begin)++; // ')';
            return parent;
        }
        if (**begin == 'x') {
            Node *parent = new Node(VAR);
            (*begin)++; // x
            (*begin)++; // )
            return parent;
        }
        char *endptr = NULL;
        errno = 0;
        double value = strtod(*begin, &endptr);
        if (errno || endptr == *begin) {
            fprintf(stderr, "Wrong input file format: wrong double value: %s\n", *begin);
            return NULL;
        }
        *begin = endptr;
        skip(begin, end);
        if (*begin >= end) {
            fprintf(stderr, "Wrong input file format: %s\n Expected ')'\n", *begin);
            return NULL;
        }
        (*begin)++;
        return new Node(value);
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
    if (this->operation) {
        root = new Node(this->operation);
    } else {
        root = new Node(this->value);
    }
    for (int i = 0; i < children_number; i++) {
        root->add_child(this->childs[i]->copy());
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
Node::derivate() {
    Node *root = NULL;
    Node *tmp = NULL;
    switch (operation) {
        case CONSTANT:
            root = new Node(0.0);
            break;
        case VAR:
            root = new Node(1.0);
            break;
        case ADD:
            root = new Node(ADD); // (a + b + c)` = a` + b` + c`
            for (int i = 0; i < children_number; i++) {
                root->add_child(childs[i]->derivate());
            }
            break;
        case SUB:
            root = new Node(SUB); // (a - b - c)` = a` - b` - c`
            for (int i = 0; i < children_number; i++) {
                root->add_child(childs[i]->derivate());
            }
            break;
        case MUL:
            root = new Node(ADD); // (a * b * c)` = a` * b * c + a * b` * c + a * b * c`
            for (int i = 0; i < children_number; i++) {
                root->add_child(new Node(MUL));
                for (int j = 0; j < children_number; j++) {
                    root->childs[i]->add_child((i == j) ? (childs[j]->derivate()) : (childs[j]->copy()));
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
            root->childs[0]->childs[0]->add_child(tmp->derivate()); // a`
            root->childs[0]->childs[0]->add_child(childs[children_number - 1]->copy()); // b
            root->childs[0]->childs[1]->add_child(tmp->copy()); // a
            root->childs[0]->childs[1]->add_child(childs[children_number - 1]->derivate()); // b`
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

                root->add_child(this->copy()); // C ^ x
                
                root->add_child(childs[children_number - 1]->derivate());
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

                root->add_child(tmp->derivate()); // x`
                break;
            }
            // (f(x) ^ g(x))` = (f ^ g) * (g` * ln(f) + g / f * f`) = 
            // = (f ^ g) * g` * ln(f) + (f ^ (g - 1)) * g * f`
            root = new Node(ADD); // +

            root->add_child(new Node(MUL)); // *
            root->childs[0]->add_child(copy()); // f ^ g
            root->childs[0]->add_child(childs[children_number - 1]->derivate()); // g`
            root->childs[0]->add_child(new Node(LN)); // ln
            root->childs[0]->childs[2]->add_child(tmp->copy()); // ln f

            root->add_child(new Node(MUL)); // *
            root->childs[1]->add_child(new Node(POWER)); // f ^ (g - 1)

            root->childs[1]->childs[0]->add_child(tmp->copy()); // f
            root->childs[1]->childs[0]->add_child(new Node(SUB)); // g - 1
            root->childs[1]->childs[0]->childs[1]->add_child(childs[children_number - 1]->copy()); // g
            root->childs[1]->childs[0]->childs[1]->add_child(new Node(1.0)); // 1
            
            root->childs[1]->add_child(tmp->copy()); // g
            root->childs[1]->add_child(childs[children_number - 1]->derivate()); // f`
            
            break;
        case LN: // (ln x)` = (1 / x) * x`
            root = new Node(MUL);
            root->add_child(new Node(DIV)); // 1 / x
            root->childs[0]->add_child(new Node(1.0)); // 1
            root->childs[0]->add_child(childs[0]->copy()); // x
            
            root->add_child(childs[0]->derivate()); // x`
            break;
        case SIN: // (sin x)` = (cos x) * x`
            root = new Node(MUL);
            root->add_child(new Node(COS)); // cos
            root->childs[0]->add_child(childs[0]->copy()); // x
            root->add_child(childs[0]->derivate()); // x`
            break;
        case COS: // (cos x)` = - sin(x) * x` = (-1) * sin (x) * x`
            root = new Node(MUL);
            root->add_child(new Node(-1.0));
            root->add_child(new Node(SIN)); // sin
            root->childs[1]->add_child(childs[0]->copy()); // x
            root->add_child(childs[0]->derivate()); // x`
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

//! \brief Comparator for child sort
//! \param [in] first,second Childs to compare
//! \return Return first > second
bool
operation_cmp(Node *first, Node *second) {
    return first->get_operation() > second->get_operation();
}

//! \brief Try to simplify expression
void
Node::simplify() {
    if (operation == CONSTANT || operation == VAR) {
        return;
    }
    for (int i = 0; i < children_number; i++) {
        childs[i]->simplify();
    }
    Node *tmp = NULL;
   // Move same operations of childs into this layer
   // One time, because childs are already simplified
    if (operation != SIN && operation != COS && operation != LN) { // these operations must have only one operand
        int children_number_old = children_number;
        for (int i = 0; i < children_number_old; i++) {
            if (childs[i]->operation == operation) {
                tmp = childs[i];
                childs[i] = tmp->childs[0]; // in the middle of child list
                for (int j = 1; j < tmp->children_number; j++) {
                    add_child(tmp->childs[j]); // at the end of child list
                }
                free(tmp); // not rec_del, because we are not copying childs, we just relink them to new parent
                tmp = NULL;
            }
        }
    } 

    if (is_constant()) {
// node and all sub-tree may be replaced by one constant
        double res = get_val();
        operation = CONSTANT;
        value = res;
        for (int i = 0; i < children_number; i++) {
            rec_del(childs[i]);
        }
        children_number = 0;
        return; // now this subtree is one node with type CONS
    }
    if (operation == ADD || operation == MUL) {
        std::sort(childs, childs + children_number, operation_cmp);
    }
// try to join constants
    int const_end = children_number - 1, const_begin = const_end;
    double cons_res = (operation == ADD || operation == SUB) ? 0 : 1;
    while ((childs[const_begin]->operation == CONSTANT)) {
        calculate(operation, &cons_res, childs[const_begin]->value);
        const_begin--; //childs[0]->operation != CONST, because !is_constant()
    }
    if (const_end - const_begin > 1) {
//replace constants by one constant
        Node *new_const = new Node(cons_res);
        for (int i = const_begin + 1; i < const_end; i++) {
            tmp = cut_child(const_begin + 1); // because children_number is decreased with each cut
            rec_del(tmp);
            tmp = NULL;
        }
        add_child(new_const);
    }

// try to join VARs (+ -> *, - -> *, * -> ^)
    int var_end = const_begin, var_begin = const_end;
    int var_num = 0;
    while ((var_begin >= 0) && (childs[var_begin]->operation == VAR)) {
        var_num++;
        var_begin--;
    } 
    if (var_end - var_begin > 1) {
        switch (operation) {
            case ADD: // x + x + x -> 3 * x
                tmp = new Node(MUL);
                tmp->add_child(new Node(VAR));            
                tmp->add_child(new Node((double)var_num));
                break;
            case SUB:
                tmp = new Node(MUL);
                tmp->add_child(new Node(VAR));
                tmp->add_child(new Node((double)(2 - var_num)));
                break;
            case MUL:
                tmp = new Node(POWER);
                tmp->add_child(new Node(VAR));
                tmp->add_child(new Node((double)var_num));
                break;
            default:
                break; // do nothing
        }
        if (tmp) { // replace VARs by one node
            for (int i = var_begin + 1; i <= var_end; i++) {
                rec_del(cut_child(var_begin + 1));
            }
            if (children_number == 0) {
                // instead making new node, link all tmp childs to *this
                for (int i = 0; i < tmp->children_number; i++) {
                    add_child(tmp->childs[i]);
                }
                operation = tmp->operation;
                free(tmp);
            } else {
                add_child(tmp);
            }
            tmp = NULL;
        }
    }
    // remove neitral elements
    int neitral = -1;
    switch (operation) {
        case ADD: // 0
        case SUB: // 0
            neitral = 0;
            break;
        case MUL: // 1
        case DIV: // 1
        case POWER:
            neitral = 1;
            break;
    }
    if (neitral != -1) {
        int ind = 0;
        while (ind < children_number) {
            if (children_number > 1  && childs[ind]->operation == CONSTANT && is_eq(neitral, childs[ind]->value)) {
                cut_child(ind);
            } else {
                ind++;
            }
        }
        if (children_number == 1) {
// replace unuseful this by its only child
            this->operation = childs[0]->operation;
            tmp = cut_child(0);
            for (int i = 0; i < tmp->children_number; i++) {
                add_child(tmp->childs[i]);
            }
            free(tmp);
            tmp = NULL;
        }
    }

    return;
}
