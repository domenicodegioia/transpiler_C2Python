#ifndef TRANSLATE_H
#define TRANSLATE_H

#include <stdio.h>

// Functions to print Ast

void print_ast(struct AstNode *n);
void print_node(struct AstNode *n);
void print_list(struct AstNode *l);

// Functions to translate C in Python

void translate_tab(int depth);
void translate(struct AstNode *n);
void translate_ast(struct AstNode *n);
void translate_node(struct AstNode *n);
void translate_decl(struct AstNode *n, char *type);
void translate_input_list(struct AstNode *n);
void translate_list(struct AstNode *l, char * func_name);
void translate_param_decl(struct AstNode *p);
void translate_param_list(struct AstNode *l);

// Functions to translate variable, expressions, nodes, ...

char * convert_var_type(int t);
char * convert_expr_type(int t);
char * convert_func_name(char *  name);
char * convert_python_type(int t);
char * convert_fs_type(char* fs);

#endif