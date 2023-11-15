#ifndef SEMANTIC_H
#define SEMANTIC_H

#include "ast.h"


// Type of built-in function
enum BUILTIN_TYPE {
    PRINTF_T,
    SCANF_T
};

// Complex type used in eval_expr_type
struct complex_type{
    enum TYPE type;
    enum KIND{
        V,
        N
    }kind;
};

// Control functions for functions
void check_fcall(char * name, struct AstNode *args);
void check_return(struct AstNode *expr);
void check_func_return(enum TYPE type, struct AstNode *code);
int check_if_return(struct AstNode *tmp);
void check_main(struct AstNode *fdecl);

// Control functions for arrays
void check_array(struct AstNode *dim);
int check_array_dim(struct AstNode *expr);
int eval_array_dim(struct AstNode *expr);

// Function to set the use of the variable in the scope
void check_var_reference(struct AstNode *var);

// Control functions for format string
char* replace_format(char *string, char *old, char *nw);
struct AstNode * extract_format_specifier(struct AstNode *format_string);
void check_format_string(struct AstNode* format_string, struct AstNode* args, enum BUILTIN_TYPE f_type);

// Control functions for expressions
struct complex_type eval_expr_type (struct AstNode *expr);
struct complex_type eval_comparison_op_type(struct complex_type l, struct complex_type r, enum EXPRESSION_TYPE expr_type);
struct complex_type eval_arithmetic_op_type(struct complex_type l, struct complex_type r);
struct complex_type eval_ass_op(struct complex_type l, struct complex_type r, struct AstNode *r_node);
void check_cond(enum TYPE type);
//struct AstNode * check_expr_statement(struct AstNode *expr);
float eval_constant_expr(struct AstNode *expr);
void check_division(struct AstNode *expr);
float check_const_truncated(struct AstNode * expr );

#endif