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
    void remove_neitrals();
    void specific_simpling();
    void calculate_values();
    void transform_constants();
    void transform_vars();
    void union_layers();
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
    Node *cut_child(int child_ind);
    bool is_constant();
    Node *copy();
    Node *derivate();
    void simplify();
    double get_val();
    bool tree_eq(Node *other);
    Node **get_childs();
};


enum Node_Types {
    CONSTANT = 0,
    VAR,
    ADD,
    SUB,
    MUL,
    DIV,
    POWER,
    LN,
    SIN,
    COS
};

Node *parse_file_create_tree(char *filename);
void rec_del(Node *root);
int get_neitral(int operation);
int get_opposite(int operation);
#endif
