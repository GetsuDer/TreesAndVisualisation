#ifndef TREE_H
#define TREE_H
class Node
{
private:
    int children_number;
    Node *parent;
    Node **childs;
    char *node_name;
public:
    Node(char *name);
    ~Node();
    int visualize(int fd);
    int add_child(Node *child);
};

#endif
