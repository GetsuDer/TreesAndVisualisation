#ifndef TREE_H
#define TREE_H
class Node
{
private:
    int children_number;
    Node *parent;
    Node **childs;
    int operation;
    double value;
    int visualize(int fd);
    int visualize_tree_rec(int fd);
public:
    Node(int _operation);
    Node(double _value);
    ~Node();
    int export_dot(int fd, char *graph_name = NULL);
    int add_child(Node *child);
};

enum Node_Types {
    CONSTANT = 0,
    ADD,
    SUB,
    MUL,
    DIV,
    POWER
};
#endif
