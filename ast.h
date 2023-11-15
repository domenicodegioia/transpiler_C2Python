#ifndef AST_H
#define AST_H


// Node type
enum NODE_TYPE {
    EXPR_T,
    IF_T,
    VAL_T,
    VAR_T,
    DECL_T,
    RETURN_T,
    FCALL_T,
    FDEF_T,
    FOR_T,
    ERROR_NODE_T
};

// Expression type
enum EXPRESSION_TYPE {
    ASS_T,
    ADD_T,
    SUB_T,
    DIV_T,
    MUL_T,
    NOT_T,
    AND_T,
    OR_T,
    G_T, 
    GE_T,
    L_T,
    LE_T,
    EQ_T,
    NE_T, 
    NEG_T,
    PAR_T
};

// Data type
enum TYPE {
    CHAR_T,
    BOOL_T,
    INT_T,
    FLOAT_T,
    VOID_T,
    STRING_T,  
    ERROR_T
};


// Generic node of Abstract Syntax Tree
struct AstNode {
    enum NODE_TYPE node_type;

    union node{
        struct variable *var;
        struct value *val;
        struct expression *expr;
        struct ifNode *ifn;
        struct forNode *forn;
        struct declaration *decl;
        struct returnNode *ret;
        struct funcCall *fcall;
        struct funcDef *fdef;
    }node;

    // pointer to the sibling node of the current node
    struct AstNode *next;
};

// Structure of the expression/assignment node
struct expression {
    enum EXPRESSION_TYPE expr_type;
    struct AstNode *l;
    struct AstNode *r;
};

// Structure of the if node
struct ifNode {
    struct AstNode *cond;
    struct AstNode *body;
    struct AstNode *else_body;
};

// Structure of the for node
struct forNode {
    struct AstNode *init;
    struct AstNode *cond;
    struct AstNode *update;
    struct AstNode *stmt;
};

// Structure of the value node
struct value {
    enum TYPE val_type;
    char *string_val;
};

// Structure of the variable node
struct variable {
    char *name;
    int by_reference; // flag indicating if the reference operator & has been used
    struct AstNode *array_dim; // flag indicating array size (or index)
};

// Structure of the declaration node
struct declaration {
    enum TYPE type;
    struct AstNode *var;
};

// Structure of the return node
struct returnNode {
    struct AstNode *expr;
};

// Structure of the function call node
struct funcCall {
    char *name;
    struct AstNode *args;
};

// Structure of the function definition node
struct funcDef {
    enum TYPE ret_type;
    char *name;
    struct AstNode *params;
    struct AstNode *code;
};

// Functions to create new nodes
struct AstNode* new_value_node(enum NODE_TYPE nodetype, enum TYPE val_type, char *string_val);
struct AstNode* new_return_node(enum NODE_TYPE nodetype, struct AstNode *expr);
struct AstNode* new_variable_node(enum NODE_TYPE nodetype, char* name, struct AstNode *array_dim);
struct AstNode* new_expression_node(enum NODE_TYPE nodetype, enum EXPRESSION_TYPE expr_type, struct AstNode* l, struct AstNode* r);
struct AstNode* new_declaration_node(enum NODE_TYPE nodetype, enum TYPE type, struct AstNode *var);
struct AstNode* new_func_call_node(enum NODE_TYPE nodetype, char *name, struct AstNode* args);
struct AstNode* new_func_def_node(enum NODE_TYPE nodetype, enum TYPE ret_type, char *name, struct AstNode* params, struct AstNode* code);
struct AstNode* new_for_node(enum NODE_TYPE nodetype, struct AstNode *init, struct AstNode* cond, struct AstNode* update, struct AstNode* stmt);
struct AstNode* new_if_node(enum NODE_TYPE nodetype, struct AstNode *cond, struct AstNode *body, struct AstNode *else_body);
struct AstNode* new_error_node(enum NODE_TYPE nodetype);

// Function to modify node attributes
void by_reference(struct AstNode *node);

// Functions to link two nodes
struct AstNode* link_AstNode(struct AstNode *node, struct AstNode *next);
struct AstNode* append_AstNode(struct AstNode *node, struct AstNode *next);

#endif