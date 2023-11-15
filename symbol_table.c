#include "symbol_table.h"
#include "global.h"
#include "translate.h"

#include <stdio.h>
#include "uthash.h"

// Function to create a new symbol table
struct symbol_list* create_symbol_table(int scope, struct symbol_list *next) {
    /*  
        scope : scope identification number
        next : pointer to previous table (outer scope)
    */

    struct symbol *symbol_table = NULL; // a new empty table is created
    struct symbol_list *symlist = malloc(sizeof(struct symbol_list));

    symlist -> scope = scope;
    symlist -> symbol_table = symbol_table;
    symlist -> next = next;

    return symlist;
}


// Function to delete a symbol table
struct symbol_list* delete_symbol_table(struct symbol_list *symlist) {
    /*  
        symlist : table to delete
    */

    struct symbol *s, *tmp;

    HASH_ITER(hh, symlist -> symbol_table, s, tmp) {
        HASH_DEL(symlist -> symbol_table, s);

        free(s -> line);
        free(s);
    }
    
    struct symbol_list *next;
    next = symlist -> next;
    free(symlist);
    return next;
}


// Function to search symbols within all currently open scopes starting from the current scope
struct symbol* find_symbol_table(struct symbol_list *symlist, char *name) {
    /*  
        symlist : current scope table 
        name : name of the symbol to search for 
    */

    struct symbol_list *tmp = symlist;
    struct symbol *s;
    
    while(tmp) {
        s = find_symbol(tmp, name);
        
        if(s) 
            return s;
        tmp = tmp -> next;
    }
    
    return NULL;
}


// Function to print the symbol table
void print_symbol_table(struct symbol_list *symlist) {
    /*  
        symlist : table to display
    */

    struct symbol *s, *tmp;

    printf("\nSYMBOL TABLE \t level: %d \n", symlist -> scope);
    printf("---------------------------\n");
    HASH_ITER(hh, symlist -> symbol_table, s, tmp) {
        printf("symbol: %s \t type: %s \n", s->name, convert_var_type(s-> type));
    }
    printf("---------------------------\n\n");
}

// Function to check if all the variables present in the Symbol Table have been used 
void check_usage(struct symbol_list *symlist){
    
    //  symlist = symbol table to check 
    
    struct symbol *s;
    
    // hh.next = it is used to iterate on the hashes, it is equivalent to doing the next of the symbol 
    for (s = symlist -> symbol_table; s != NULL; s = s -> hh.next){
        if (s -> sym_type == VARIABLE && s -> used == 0){
            yywarning(error_string_format("variable " BOLD "%s" RESET " declared but not used", s -> name));
            yynote(error_string_format("declaration of " BOLD "%s" RESET " was here", s -> name), s -> lineno, s -> line);
        }
    }
}





// continuare da qui





// Function to search a symbol in the current scope
struct symbol* find_symbol(struct  symbol_list* symlist, char *name) {
    /*  
        symlist : current symbol table
        name : identifier to search for
    */
    struct symbol *s;

    HASH_FIND_STR(symlist -> symbol_table, name, s);

    if(s)
        return s;
    
    return NULL;
}


// Function to insert a symbol in the current scope
void insert_symbol(struct symbol_list *symlist, char *name, enum TYPE type, enum symbol_type sym_type, struct AstNode *params_list,
        int lineno, char *line) {
    /* 
        symlist : current scope table
        name : identifier of the symbol to insert
        type : data type
        sym_type : symbol type
        params_list : pointer to the list of parameters (if the symbol is a function)
        lineno : symbol declaration line number
        line : symbol declaration line
    */

    struct symbol *s;

    s = find_symbol(symlist, name);

    if(s) {
        // semantic error: double declaration
        yyerror(error_string_format("%s redeclared in this scope", name));
        yynote(error_string_format("previous declaration of %s was here", name), s -> lineno, s -> line);
    } else {
        s = malloc(sizeof(struct symbol));
        s -> name = name;
        s -> type = type;
        s -> sym_type = sym_type;
        s -> params_list = params_list;
        s -> lineno = lineno;
        s -> line = strdup(line);
        s -> used = 0;
        s -> array = 0;

        HASH_ADD_STR(symlist -> symbol_table, name, s);
    }
}


// Function to remove a symbol from the passed symbol table 
void remove_symbol(struct  symbol_list* symlist, struct symbol *s) {
    /*  
        syml : symbol table
        s : symbol to remove
    */
    HASH_DEL(symlist -> symbol_table, s);

    free(s -> line);
    free(s);
}