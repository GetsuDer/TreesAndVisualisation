#include <string>
#include <cstdio>
#include <cassert>
#include <sys/mman.h>

#include "tree.h"
#include "in_and_out.h"

//! \brief Node constructor
Node::Node(int _operation) {
    children_number = 0;
    childs = NULL;
    parent = NULL;
    operation = _operation;
}

Node::Node(double _value) {
    children_number = 0;
    childs = NULL;
    parent = NULL;
    operation = CONSTANT;
    value = _value;
}

//! \brief Node destructor
Node::~Node() {
    free(childs);
}

int
Node::get_children_number() {
    return children_number;
}

//! \brief Write description of node in graphivz-readable format
//! \param [in] fd File descriptor
//! \return Returns 0 in success -1 else
int
Node::visualize(int fd) {
    assert(fd > 0);
    switch (this->operation) {
        case CONSTANT: 
            dprintf(fd, "\"%lf\"", this->value);
            break;
        case MUL:
            dprintf(fd, "\"*\"");
            break;
        case DIV:
            dprintf(fd, "\"/\"");
            break;
        case ADD:
            dprintf(fd, "\"+\"");
            break;
        case SUB:
            dprintf(fd, "\"-\"");
            break;
        case POWER:
            dprintf(fd, "\"^\"");
            break;
        default:
            fprintf(stderr, "Wrong operation: %d\n", this->operation);
            return -1;
    } 
    return 0;
}

//! \brief Recursive function for tree visualization generation
//! \param [in] fd File descriptor
//! \return Returns 0 in success -1 else
int
Node::visualize_tree_rec(int fd) {
    for (int i = 0; i < this->get_children_number(); i++) {
        this->visualize(fd);
        dprintf(fd, "->");
        this->childs[i]->visualize(fd);
        dprintf(fd, ";\n");
        if (this->childs[i]->visualize_tree_rec(fd)) {
            fprintf(stderr, "Error during recursive visualization\n");
            return -1;
        }
    }
    return 0;
}

Node **
Node::get_childs() {
    return childs;
}
//! \brief Writes tree description in dot-readable format
//! \param [in] root Tree root
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
    this->visualize_tree_rec(fd);
    dprintf(fd, "}\n");
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

static void
skip(char **begin, char *end) {
    while (*begin < end && isspace(**begin)) (*begin)++;
    return;
}

void
rec_del(Node *root) {
    for (int i = 0; i < root->get_children_number(); i++) {
        rec_del(root->get_childs()[i]);
    }
    delete root;
    return;
}

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
        default:
            return -1;
    }
    return -1;
}

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
    (*begin)++;
    skip(begin, end);
    if (*begin >= end) {
        fprintf(stderr, "Wrong input file format: %s\n Expected symbol\n", *begin);
        return NULL;
    }
    if (**begin == '(') { //not constant
        //now should parse childs: ( (child1) op (child2) )
        Node *first_child = parse_rec(begin, end);
        skip(begin, end);
        if (*begin >= end) {
            fprintf(stderr, "No operation in input file: %s\n", *begin);
            rec_del(first_child);
            return NULL;
        }
        //find operation
        skip(begin, end);
        int operation = find_operation(begin);
        if (operation < 0) {
            fprintf(stderr, "Can not work with this operation or can not recognize: %c\n", **begin);
            rec_del(first_child);
            return NULL;
        }
        Node *parent = new Node(operation);

        Node *second_child = parse_rec(begin, end);

        if (!second_child) {
            fprintf(stderr, "Only operation with two operands are permiterd!%s\n", *begin);
            rec_del(first_child);
            delete parent;
            return NULL;
        }
        parent->add_child(first_child);
        parent->add_child(second_child);
        return parent;

    } else { //constant
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
