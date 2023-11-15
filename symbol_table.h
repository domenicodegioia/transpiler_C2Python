#ifndef SYMBOL_TABLE_H
#define SYMBOL_TABLE_H

#include "uthash.h"

#include "ast.h"

// Symbol types
enum symbol_type {
    VARIABLE,
    FUNCTION,
    PARAMETER,
    RETURN_S 
};

// Symbol structure
struct symbol {
    char *name;                         // symbol name (key)
    enum TYPE type;                     // data type (from ast.h)
    enum symbol_type sym_type;
    struct AstNode *params_list;        // pointer to the function parameters list
    int used;                           // flag indicating whether or not a variable is used within a scope
    int array;                          // flag indicating whether a variable or parameter is an array or less
    int lineno;                         // line number in which the symbol is declared
    char *line;                         // copy of the line in which the symbol is declared

    UT_hash_handle hh;                  // makes this structure hashable
};

// Table list
struct symbol_list {
    int scope;                          // level
    struct symbol *symbol_table;        // pointer to the symbol table
    struct symbol_list *next;           // pointer to the next symbol table
};

// Functions for table operations
struct symbol_list* create_symbol_table(int scope, struct symbol_list *next);
struct symbol_list* delete_symbol_table(struct symbol_list *symlist);
struct symbol* find_symbol_table(struct symbol_list *symlist, char *name);
void print_symbol_table(struct symbol_list *symlist);
void check_usage(struct symbol_list *symlist);

// Functions for symbol operations
struct symbol* find_symbol(struct  symbol_list* symlist, char *name);
void insert_symbol(struct symbol_list* symlist, char *name, enum TYPE type, enum symbol_type symbol_type, struct AstNode *params_list, int lineno, char *line);
void remove_symbol(struct  symbol_list* symlist, struct symbol *s);

#endif