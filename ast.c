#include "ast.h"
#include "global.h"
#include <stdlib.h>


// Function to create a new value node
struct AstNode* new_value_node(enum NODE_TYPE node_type, enum TYPE val_type, char *string_val) {
    /*  
        nodetype : VAL_T
        val_type : data type
        string_val : value
    */

    struct value *val = malloc(sizeof(struct value));
    struct AstNode *node = malloc(sizeof(struct AstNode));

    val -> val_type = val_type;
    val -> string_val = string_val;

    node -> node_type = node_type;
    node -> node.val = val;
    node -> next = NULL;

    return node;
}


// Function to create a new return node
struct AstNode* new_return_node(enum NODE_TYPE node_type, struct AstNode *expr) {
    /*  
        nodetype : RETURN_T
        expr : expression or returned value
    */

    struct returnNode *return_node = malloc(sizeof(struct returnNode));
    struct AstNode *node = malloc(sizeof(struct AstNode));
    
    return_node -> expr = expr;
    
    node -> node_type = node_type;
    node -> node.ret = return_node;
    node -> next = NULL;

    return node;
}


// Function to create a new variable node
struct AstNode* new_variable_node(enum NODE_TYPE node_type, char* name, struct AstNode *array_dim) {
    /*  
        nodetype : VAR_T
        name : variable name
        array_dim : array size
    */

    struct variable *var = malloc(sizeof(struct variable));
    struct AstNode *node = malloc(sizeof(struct AstNode));

    var -> name = name;
    var -> array_dim = array_dim;
    var -> by_reference = 0;

    node -> node_type = node_type;
    node -> node.var = var;
    node -> next = NULL;

    return node;
}


// Function to create a new expression node
struct AstNode* new_expression_node(enum NODE_TYPE node_type, enum EXPRESSION_TYPE expr_type, struct AstNode* l, struct AstNode* r) {
    /*  
        nodetype : EXPR_T
        expr_type : expression type
        r : right operator
        l : left operator
    */

    struct expression *expr = malloc(sizeof(struct expression));
    struct AstNode *node = malloc(sizeof(struct AstNode));

    expr -> expr_type = expr_type;
    expr -> l = l;
    expr -> r = r;

    node -> node_type = node_type;
    node -> node.expr = expr;
    node -> next = NULL;

    return node;
}


// Function to create a new declaration node
struct AstNode* new_declaration_node(enum NODE_TYPE node_type, enum TYPE type, struct AstNode *var) {
    /*  
        nodetype : DECL_T
        type : declaration type (int, float, ...)
        var : pointer to the declared variables list 
        (it can also be an EXPR_T node in case of declarations with assignment)
    */

    struct declaration *decl = malloc(sizeof(struct declaration));
    struct AstNode *node = malloc(sizeof(struct AstNode));
    
    decl -> type = type;
    decl -> var = var;

    node -> node_type = node_type;
    node -> node.decl = decl;
    node -> next = NULL;

    // check on variable type (with loop for multiple declarations)
    if (type == VOID_T){
        for(struct AstNode *tmp = var; tmp; tmp = tmp -> next){
            yyerror(error_string_format("variable %s declared void", tmp -> node.var -> name));
        }
    }
    
    return node;
}


// Function to create a new function call node
struct AstNode* new_func_call_node(enum NODE_TYPE node_type, char *name, struct AstNode* args) {
    /*  
        nodetype : FCALL_T
        name : function name
        args : pointer to the list of the arguments of the function call
    */
    
    struct funcCall *fcall = malloc(sizeof(struct funcCall));
    struct AstNode *node = malloc(sizeof(struct AstNode));

    fcall -> name = name;
    fcall -> args = args;

    node -> node_type = node_type;
    node -> node.fcall = fcall;
    node -> next = NULL;

    return node;
}


// Function to create a new function definition node
struct AstNode* new_func_def_node(enum NODE_TYPE node_type, enum TYPE ret_type, char *name, struct AstNode* params, struct AstNode* code) {
    /*  nodetype : FDEF_T
        ret_type : return type of the function
        name : identifier of the function
        params : pointer to the list of function parameters
        code : pointer to the list of instructions contained in the function definition
    */
   
    struct funcDef *fdef = malloc(sizeof(struct funcDef));
    struct AstNode *node = malloc(sizeof(struct AstNode));

    fdef -> ret_type = ret_type;
    fdef -> name = name;
    fdef -> params = params;
    fdef -> code = code;

    node -> node_type = node_type;
    node -> node.fdef = fdef;
    node -> next = NULL;

    return node;
}


// Function to create a new for node
struct AstNode* new_for_node(enum NODE_TYPE node_type, struct AstNode *init, struct AstNode* cond, struct AstNode* update, struct AstNode* stmt) {
    /*  
        nodetype : FOR_T
        init : pointer to the AST node containing the for init
        cond : pointer to the AST node containing the for condition
        update : pointer to the AST node containing the for update
        stmt : pointer to the AST node containing the for body
    */

    struct forNode *forn = malloc(sizeof(struct forNode));
    struct AstNode *node = malloc(sizeof(struct AstNode));

    forn -> init = init;
    forn -> cond = cond;
    forn -> update = update;
    forn -> stmt = stmt;

    node -> node_type = node_type;
    node -> node.forn = forn;
    node -> next = NULL;

    return node;
}


// Function to create a new if node
struct AstNode* new_if_node(enum NODE_TYPE node_type, struct AstNode *cond, struct AstNode *body, struct AstNode *else_body) {
    /*  
        nodetype : IF_T
        cond : pointer to the AST node containing the if condition
        body : pointer to the AST node containing the if body
        else_body : pointer to the AST node containing the else body (if present) 
    */
    struct ifNode *ifn = malloc(sizeof(struct ifNode));
    struct AstNode *node = malloc(sizeof(struct AstNode));

    ifn -> cond = cond;
    ifn -> body = body;
    ifn -> else_body = else_body;

    node -> node_type = node_type;
    node -> node.ifn = ifn;
    node -> next = NULL;

    return node;
}


// Function to create a new error node
struct AstNode* new_error_node(enum NODE_TYPE node_type) {
    /* 
        nodetype : ERROR_NODE_T
    */
    struct AstNode *node =  malloc(sizeof(struct AstNode));
    
    node -> node_type = node_type;
    node -> next = NULL;
    return node;
}


// Function to modify the by_reference attribute value of a variable node
void by_reference(struct AstNode *node) {
    node -> node.var -> by_reference = 1;
}


// Function to create a list of node starting from the last one
struct AstNode* link_AstNode(struct AstNode *node, struct AstNode *next) {
    node -> next = next;
    return node;
}


// Function to create a list of node starting from the first one
struct AstNode* append_AstNode(struct AstNode *node, struct AstNode *next) {
    node -> next = next;
    return next;
}