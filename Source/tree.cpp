#include <string>
#include <cstdio>

#include "tree.h"

Node::Node(char *name) {
    children_number = 0;
    childs = NULL;
    parent = NULL;
    node_name = name;
}

Node::~Node() {
    free(childs);
}
//! \brief Write description of node and labels of node in graphivz-readable format
//! \param [in] fd File descriptor
//! \return Returns 0 in success
int
Node::visualize(int fd) {
    for (int i = 0; i < children_number; i++) {
        dprintf(fd, "%s->%s\n", node_name, childs[i]->node_name);
    } 
    return 0;
}

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
