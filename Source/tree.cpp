#include <string>
#include <cstdio>
#include <cassert>

#include "tree.h"

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
    for (int i = 0; i < this->children_number; i++) {
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
