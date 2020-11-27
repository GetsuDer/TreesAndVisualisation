#ifndef TREE_H
#define TREE_H
class Node
{
private:
    int children_number;
    int node_id;
    Node *parent;
    Node **childs;
    int operation;
    double value;
    int visualize(int fd);
    double visualize_tree_rec(int fd);
    double visualize_tree_rec_tex(int fd);
public:
    static int id;
    Node(int _operation);
    Node(double _value);
    ~Node();
    int export_dot(int fd, char *graph_name = NULL);
    int export_tex(int fd);
    int add_child(Node *child);
    int get_children_number();
    int get_operation();
    Node **get_childs();
};


enum Node_Types {
    CONSTANT = 0,
    ADD,
    SUB,
    MUL,
    DIV,
    POWER
};

Node *parse_file_create_tree(char *filename);
void rec_del(Node *root);
#endif
