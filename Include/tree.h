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
    char *name;
    int name_len;
    int visualize(int fd);
    double visualize_tree_rec(int fd);
    double visualize_tree_rec_tex(int fd);
    void remove_neitrals();
    void specific_simpling();
    void calculate_values();
    void transform_constants();
    void transform_vars(char *var_name);
    void union_layers();
    void simp_var(char *var_name);
    char **get_node_vars(int *var_num);
public:
    static int id;
    Node(int _operation);
    Node(int _operation, char *name);
    Node(double _value);
    ~Node();
    int export_dot(int fd, char *graph_name = NULL);
    int export_tex(int fd);
    int add_child(Node *child);
    int get_children_number();
    int get_operation();
    int get_name_len();
    char *get_name();
    double get_value();
    bool change_operation(int new_operation);
    Node *cut_child(int child_ind);
    bool is_constant();
    Node *copy();
    Node *derivate(char *var_name);
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
    COS,
    ASSIGNMENT,
    MORE,
    LESS,
    EQ,
    IF,
    WHILE,
    FOR,
    DO_IN_ORDER,
    FUNC_CALL,
    FUNC_DEF,
    RETURN
};

Node *parse_file_create_tree(char *filename);
void rec_del(Node *root);
int get_neitral(int operation);
int get_opposite(int operation);


struct Env {
    const char *str;
    int current_ind;
    int str_size;
    int error;
    char expected_symbol;
};


enum Env_Errors {
    OK = 0,
    NO_SYMBOL,
    WRONG_SYMBOL,
    TOO_LONG_ID,
};

Node *Parse_All(char *str, int str_length);
constexpr int ID_NAME_SIZE = 15;
constexpr int FUNC_NAME_SIZE = ID_NAME_SIZE;
#endif
