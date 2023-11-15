%{
	// Prologue

    #include <stdio.h>
    #include <stdlib.h>
    #include <string.h>
    #include <ctype.h>
    #include "symbol_table.h"
    #include "ast.h"
    #include "global.h"
    #include "semantic.h"
    #include "translate.h"

    char *filename;  // name of input file
    int error_num;  // number of errors
    int main_flag;  // flag indicating if there is a main function
    int current_scope_lvl;
    struct symbol_list *current_symbol_table;

    extern int yylex();
    extern FILE *yyin;  // pointer to the input file
	extern int yylineno;	
    extern char *line;

    struct AstNode *root; // root node of the AST
    struct AstNode *param_list = NULL; // pointer to the list of function parameters (to be included in the scope)
    enum TYPE ret_type = -1; // function return type (to be included in the scope)

    void scope_enter();
    void scope_exit();

    void fill_symbol_table(struct symbol_list * symlist, struct AstNode *node, enum TYPE type, enum symbol_type sym_type);

    int print_symtab_flag = 0; // -s
    int print_ast_flag = 0; // -t
    void print_usage(); // compiler usage

%}

// Bison declarations

%define parse.error verbose

// The %union declaration specifies the entire collection of possible data types for semantic values.
%union {
    //char *name_token;       // for identifiers and strings
	//int integer_token;      // for integers
	//float float_token;      // for floats

    char* s;  // the numbers will be converted later if necessary
    struct AstNode *ast;
    int t;
}

// token kind name (terminal symbol)
%token      <s> INT_VALUE  //numerical constants (integer)
%token      <s> FLOAT_VALUE  //numerical constants (float)
%token      <s> STRING_VALUE  //strings
%token      <s> ID
%token      <s> SCANF PRINTF
%token      CHAR INT FLOAT
%token      IF ELSE FOR VOID RETURN

// Precedence and associativity ()
%left       '+' '-'
%left       '*' '/'
%right      '='
%left       AND OR NOT // NOT is a unary operator
%nonassoc   MINUS
%left       '>' '<' GE LE EQ NE 
%left 		'[' ']' '(' ')' ','

// The declaration says there should be 1 shift/reduce conflicts and no reduce/reduce conflicts.
// Bison reports an error if the number of shift/reduce conflicts differs from n, or if there are any reduce/reduce conflicts. 
%expect 1

%type <ast> number var var_declaration global_declaration_list global_declaration  expr assignment_statement assignment
%type <ast> return_statement expr_statement func_call args format_string printf_statement  if_cond
%type <ast> declarator_list initializer_list
%type <ast> func_declaration param_list param compound_statement statement_list statement
%type <ast> scanf_statement scanf_var_list iteration_statement init cond update selection_statement
%type <ast> embedded_statement single_statement
%type <t> type

//%start program 

%%
// Grammar rules


program
    : { scope_enter(); } global_declaration_list                   { root = $2; scope_exit(); }
    ;

global_declaration_list
    : global_declaration 
    | global_declaration global_declaration_list                    { $$ = link_AstNode($1, $2); }
    ;

// in the global scope, only variable or function declarations can be present
global_declaration
    : var_declaration
    | func_declaration                 { check_main($1); }
    ;

var_declaration
	: type declarator_list ';'         { $$ = new_declaration_node(DECL_T, $1, $2); fill_symbol_table(current_symbol_table, $$, -1, VARIABLE); }
	| type initializer_list ';'         { $$ = new_declaration_node(DECL_T, $1, $2); fill_symbol_table(current_symbol_table, $$, -1, VARIABLE); }
    | ID declarator_list ';'           { $$ = new_error_node(ERROR_NODE_T); yyerror(error_string_format("Unknown type name: " BOLD "%s" RESET, $1 )); }
    ;

declarator_list
	: var                               
	| var ',' declarator_list   { $$ = link_AstNode($1, $3); }
	;

initializer_list
    : assignment                       
	| assignment ',' initializer_list   {$$ = link_AstNode($1, $3); }
	;

type
    : INT                              { $$ = INT_T; }
    | FLOAT                            { $$ = FLOAT_T; }
    | CHAR                             { $$ = CHAR_T; }
    | VOID                             { $$ = VOID_T; } 
    ;

var
    : ID                               { $$ = new_variable_node(VAR_T, $1, NULL); }
    | ID '[' expr ']'                  { $$ = new_variable_node(VAR_T, $1, $3); check_array($3); }
    ;

func_declaration
    : type ID '(' param_list ')'       { param_list = $4; ret_type = $1; insert_symbol(current_symbol_table, $2, $1, FUNCTION, $4, yylineno, line); } 
            compound_statement         { $$ = new_func_def_node(FDEF_T, $1, $2, $4, $7); check_func_return($1, $7); }
    | type ID '(' ')'                  { ret_type = $1; insert_symbol(current_symbol_table, $2, $1, FUNCTION, NULL, yylineno, line); } 
            compound_statement         { $$ = new_func_def_node(FDEF_T, $1, $2, NULL, $6); check_func_return($1, $6); }
    | type ID '(' VOID ')'             { ret_type = $1; insert_symbol(current_symbol_table, $2, $1, FUNCTION, NULL, yylineno, line); } 
            compound_statement         { $$ = new_func_def_node(FDEF_T, $1, $2, NULL, $7); check_func_return($1, $7); }
    
    ;

compound_statement
    : scope_enter statement_list scope_exit     { $$ = $2; }
    ;

scope_enter
    : '{'                                       { scope_enter(); }
    ;

scope_exit
    : '}'                                       { scope_exit(); }
    ;

param_list
    : param  
    | param ',' param_list             { $$ = link_AstNode($1, $3); }
    ;

param
    : type ID                          { $$ = new_declaration_node(DECL_T, $1, new_variable_node(VAR_T, $2, NULL)); }
    | type ID '[' ']'                  { $$ = new_declaration_node(DECL_T, $1,new_variable_node(VAR_T, $2, new_value_node(VAL_T, INT_T, "0"))); }
    ;

statement_list
    : statement   
    | statement statement_list         { $$ = link_AstNode($1, $2); }
    ;

statement
    : var_declaration
    | compound_statement
    | expr_statement
    | selection_statement
    | iteration_statement
    | return_statement
    | printf_statement
    | scanf_statement
    | assignment_statement
    ;


embedded_statement
    : compound_statement               
    | single_statement
    ;

single_statement
    : var_declaration                                               { yyerror("embedded statement cannot be a declaration"); }
    | expr_statement
    | selection_statement
    | iteration_statement
    | return_statement
    | printf_statement
    | scanf_statement
    | assignment_statement
    ;

expr_statement
    : expr ';'                                                      { eval_expr_type($1); $$ = $1; }
    ;

selection_statement
    : IF '(' if_cond ')' embedded_statement                         { $$ = new_if_node(IF_T, $3, $5, NULL); }
    | IF '(' if_cond ')' embedded_statement ELSE embedded_statement { $$ = new_if_node(IF_T, $3, $5, $7); }
    ;

if_cond
    : expr                                                          { check_cond(eval_expr_type($1).type); $$ = $1; }                                                           
    ;

iteration_statement
    : FOR '(' init ';' cond ';' update ')' embedded_statement       { $$ = new_for_node(FOR_T, $3, $5, $7, $9); scope_exit(); }
    ;

init
    : assignment                                                    { scope_enter(); eval_expr_type($1); $$ = $1;}
    | type assignment                                               { scope_enter(); $$ = new_declaration_node(DECL_T, $1, $2); fill_symbol_table(current_symbol_table, $$, -1, VARIABLE); } 
    | /* empty */                                                   { scope_enter(); $$ = NULL; }
    ;

cond
    : expr                                                          { check_cond(eval_expr_type($1).type); $$ = $1; }
    | /* empty */                                                   { $$ = NULL; }
    ;    

update
    : assignment                                                    { eval_expr_type($1); $$ = $1; }
    | expr                                                          { eval_expr_type($1); $$ = $1; }
    | /* empty */                                                   { $$ = NULL; } 
    ;

return_statement
    : RETURN ';'                                                    { $$ = new_return_node(RETURN_T, NULL); check_return(NULL); }
    | RETURN expr ';'                                               { $$ = new_return_node(RETURN_T, $2); check_return($2); }
    ;  

printf_statement
    : PRINTF '(' format_string ')' ';'                              { $$ = new_func_call_node(FCALL_T, $1, $3); check_format_string($3, NULL, PRINTF_T); }
    | PRINTF '(' format_string ',' args ')' ';'                     { $$ = new_func_call_node(FCALL_T, $1, link_AstNode($3,$5)); check_format_string($3, $5, PRINTF_T); }
    | PRINTF '(' ')' ';'                                            { $$ = new_error_node(ERROR_NODE_T); yyerror("too few arguments to function" BOLD " printf" RESET); }
    ;      

scanf_statement
    : SCANF '(' format_string ',' scanf_var_list ')' ';'            { check_format_string($3, $5, SCANF_T); $$ = new_func_call_node(FCALL_T, $1, link_AstNode($3, $5)); }
    | SCANF '(' format_string ')' ';'                               { check_format_string($3, NULL, SCANF_T); $$ = new_func_call_node(FCALL_T, $1, $3); }
    | SCANF '(' ')' ';'                                             { $$ = new_error_node(ERROR_NODE_T); yyerror("too few arguments to function" BOLD " scanf" RESET); }
    ;

scanf_var_list
    : '&' var                                                       { check_var_reference($2); by_reference($2); $$ = $2; }
    | var                                                           { check_var_reference($1); $$ = $1; }
    | '&' var ',' scanf_var_list                                    { check_var_reference($2); by_reference($2); $$ = link_AstNode($2, $4); }
    | var ','   scanf_var_list                                      { check_var_reference($1); link_AstNode($1, $3); }
    ;

format_string
    : STRING_VALUE                                                  { $$ = new_value_node(VAL_T, STRING_T, $1); }
    ;

assignment_statement
    : assignment ';'                                                { $$ = $1; eval_expr_type($1); }
    ;

assignment
    : var '=' expr                                                  { $$ = new_expression_node(EXPR_T, ASS_T, $1, $3); check_var_reference($1); }
    ;

expr
    : var                                                           { $$ = $1; check_var_reference($1); }
    | number
    | format_string
    | func_call
    | expr '+' expr                                                 { $$ = new_expression_node(EXPR_T, ADD_T, $1, $3); }
    | expr '-' expr                                                 { $$ = new_expression_node(EXPR_T, SUB_T, $1, $3); }
    | expr '*' expr                                                 { $$ = new_expression_node(EXPR_T, MUL_T, $1, $3); }
    | expr '/' expr                                                 { $$ = new_expression_node(EXPR_T, DIV_T, $1, $3); check_division($3); }
    | NOT expr                                                      { $$ = new_expression_node(EXPR_T, NOT_T, NULL, $2); }
    | expr AND expr                                                 { $$ = new_expression_node(EXPR_T, AND_T, $1, $3); }
    | expr OR expr                                                  { $$ = new_expression_node(EXPR_T, OR_T, $1, $3); }
    | expr '>' expr                                                 { $$ = new_expression_node(EXPR_T, G_T, $1, $3); }
    | expr '<' expr                                                 { $$ = new_expression_node(EXPR_T, L_T, $1, $3); }
    | expr GE expr                                                  { $$ = new_expression_node(EXPR_T, GE_T, $1, $3); }
    | expr LE expr                                                  { $$ = new_expression_node(EXPR_T, LE_T, $1, $3); }
    | expr EQ expr                                                  { $$ = new_expression_node(EXPR_T, EQ_T, $1, $3); }
    | expr NE expr                                                  { $$ = new_expression_node(EXPR_T, NE_T, $1, $3); }
    | '(' expr ')'                                                  { $$ = new_expression_node(EXPR_T, PAR_T, NULL, $2); }
    | '-' expr %prec MINUS                                          { $$ = new_expression_node(EXPR_T, NEG_T, NULL, $2); }
    ;

number
    : INT_VALUE                                                     { $$ = new_value_node(VAL_T, INT_T, $1); }
    | FLOAT_VALUE                                                   { $$ = new_value_node(VAL_T, FLOAT_T, $1); }
    ;

func_call
    : ID '(' args ')'                                               { $$ = new_func_call_node(FCALL_T, $1, $3); check_fcall($1, $3); }
    | ID '(' ')'                                                    { $$ = new_func_call_node(FCALL_T, $1, NULL); check_fcall($1, NULL); }
    ;

args
    : expr
    | expr ',' args                                                 { $$ = link_AstNode($1, $3); }
    ;


%%


// Function to open a new scope
void scope_enter() {
    // create a new empty table 
    current_symbol_table = create_symbol_table(current_scope_lvl, current_symbol_table);

    // increment the current scope level
    current_scope_lvl++;

    // if the new scope is a function scope, add 1) possible parameters
    if(param_list) { 
        fill_symbol_table(current_symbol_table, param_list, -1, PARAMETER); 
        param_list = NULL;
    }
    // and 2) function return type
    if(ret_type != -1) {
        insert_symbol(current_symbol_table, "return", ret_type, RETURN_S, NULL, yylineno, line);
        ret_type = -1;
    }
}


// Function to close a scope
void scope_exit() {
    // eventually print the Symbol Table
    if(print_symtab_flag)
        print_symbol_table(current_symbol_table);

    check_usage(current_symbol_table); 

    // delete the current table
    current_symbol_table = delete_symbol_table(current_symbol_table);

    // increment the current scope number
    current_scope_lvl--;
}


// Function to insert one or more symbols in the Symbol Table
void fill_symbol_table(struct symbol_list * symlist, struct AstNode *node, enum TYPE type, enum symbol_type sym_type){
    /*  
        symlist : pointer to current_symbol_table
        n : pointer to the AST node
        type : data type
        sym_type : symbol type (only parameter or variable)
    */

    while(node){      
        if(node -> node_type == VAR_T) {
            insert_symbol(symlist, node -> node.var -> name, type, sym_type, NULL, yylineno, line);
            if(node -> node.var -> array_dim){
                // if it is an array (so array_dim in the node is not null), modify the flag in the symbol table
                struct symbol *s = find_symbol(symlist, node -> node.var -> name);
                s -> array = 1;
            }
        }else if(node -> node_type == EXPR_T){
            // Declarations with assignment, so in this case EXPR_T will always be an assignment
            insert_symbol(symlist, node -> node.expr -> l -> node.var -> name, type, sym_type, NULL, yylineno, line);
            
            if(node -> node.expr -> l -> node.var -> array_dim){
                // if it is an array (so array_dim in the node is not null), modify the flag in the symbol table
                yyerror("invalid array initializer");
                struct symbol *s = find_symbol(symlist, node -> node.expr -> l -> node.var -> name);
                s -> array = 1;
            }
            
            // evaluate the expression type
            eval_expr_type(node);

        }else if(node -> node_type == DECL_T){
            // the first passed node will always be DECL_T type from which type will be extracted to be used in subsequent cases as well   
            type = node -> node.decl -> type;
            fill_symbol_table(symlist, node -> node.decl -> var, type, sym_type);
        } 

        node = node -> next;
    }
}


int main(int argc, char **argv) {
    //debug
    /* #ifdef YYDEBUG
        yydebug = 1;
    #endif */

    int file_count = 0;

    if(argc < 2) {
        // no input file
        fprintf(stderr, RED "fatal error:" RESET " no input file\n");
        exit(1);
    }else{
        for(int i = 1; i < argc; i++){
            // -s to print symbol tables
            if(strcmp(argv[i], "-s") == 0)
                print_symtab_flag = 1;
                // -t to print AST
            else if(strcmp(argv[i], "-t") == 0)
                print_ast_flag = 1;
                // -h --help to print usage
            else if(strcmp(argv[i], "-h") == 0 || strcmp(argv[i], "--help") == 0){
                print_usage();
                exit(0);
            }else {
                if(argv[i][0] == '-'){
                    // unrecognized command line option
                    fprintf(stderr, RED "error:" RESET " unrecognized command line option " BOLD "%s \n" RESET, argv[i]);
                    print_usage();
                    exit(1);
                }else{
                    // check on the number of input files
                    if(file_count == 0){
                        filename = argv[i];
                        file_count++;
                    }else{
                        // more than one input file
                        fprintf(stderr, RED "error:" RESET " only one input file accepted \n");
                        exit(1);
                    }
                }
            }
        }
    }

    if(file_count == 0){
        // no input file
        fprintf(stderr, RED "fatal error:" RESET " no input file\n");
        exit(1);
    }else{
        yyin = fopen(filename, "r");
    }

    if(!yyin) {
        fprintf(stderr, RED "error:" RESET " %s: ", filename);
        perror("");
        fprintf(stderr, RED "fatal error:" RESET" no input file\n");
        exit(1);
    }


    // global variables initialization
    error_num = 0;
    current_scope_lvl = 0;
    current_symbol_table = NULL;

    // yyparse: function to parse an input stream
    if(yyparse() == 0) {
        if(print_ast_flag) {
            printf("\nABSTRACT SYNTAX TREE\n\n");
            print_ast(root); 
        }
        if(error_num == 0){
            translate(root);
        }
    }
    
    fclose(yyin);
}


//  If the --help or -h flags have been used, this function displays the compiler usage
void print_usage(){
    printf("Usage: ./compiler [options] file  \n");
    printf("options: \n");
    printf(" --help \t Diplay this information. \n");
    printf(" -h \t\t Diplay this information. \n");
    printf(" -s \t\t Print Symbol Table. \n");
    printf(" -t \t\t Print Abstract Syntax Tree. \n");
}