#include "ast.h"
#include "global.h"
#include <stdlib.h>
#include "translate.h"
#include "semantic.h"

extern char *filename;
extern int main_flag; 

int depth = 0; // indentation level
FILE *fp;



// Ausiliar function to concatenate two strings
char* concat(const char *s1, const char *s2)
{
    char *result = malloc(strlen(s1) + strlen(s2) + 1);
    strcpy(result, s1);
    strcat(result, s2);
    return result;
}




// Function to print indentation given the level
void print_tab(int d) {
    // d : indentation level
    for(int i = 0; i < d; i++)
        printf("\t");
}

// Function to print Ast
void print_ast(struct AstNode *n) {
    while(n){
        print_tab(depth);
        
        if(n -> node_type == FDEF_T || n -> node_type == FOR_T || n -> node_type == IF_T) {
            depth++;
        }

        print_node(n);

        if(n -> node_type != FDEF_T && n -> node_type != FOR_T && n -> node_type != IF_T){
            printf(";\n");
        }

        n = n->next;
    }
}

// Function to print AST node
void print_node(struct AstNode *n){
    switch(n->node_type){
        case VAL_T:
            if(n -> node.val -> val_type == STRING_T) {
                printf("\"");
                printf("%s", n -> node.val -> string_val);
                printf("\"");
            }else{
                printf("%s", n -> node.val -> string_val);
            }
            break;
        case VAR_T:
            if(n -> node.var -> by_reference) {
                printf("&");
            }
            printf("%s", n -> node.var-> name);
            if(n -> node.var -> array_dim) {
                printf("[");
                print_node(n -> node.var -> array_dim);
                printf("]");
            }
            break;
        case DECL_T:
            printf("%s ", convert_var_type(n -> node.decl -> type));
            print_list(n -> node.decl -> var);
            break;
        case EXPR_T:
            if(n -> node.expr -> expr_type == PAR_T) {
                printf("(");
                print_node(n -> node.expr -> r);
                printf(")");
            } else {
                if(n -> node.expr -> l)
                    print_node(n -> node.expr -> l);
                printf("%s", convert_expr_type(n -> node.expr -> expr_type));
                print_node(n -> node.expr -> r);
            }
            break;
        case RETURN_T:
            printf("return ");
            if(n -> node.ret -> expr) {
                print_node(n -> node.ret -> expr);
            }
            break;
        case FCALL_T:
            printf("%s(", n -> node.fcall -> name);
            if(n -> node.fcall -> args) {
                print_list(n-> node.fcall -> args);
            }
            printf(")");
            break;
        case FDEF_T:
            printf("%s ", convert_var_type(n -> node.fdef -> ret_type));
            printf("%s(", n -> node.fdef -> name );
            if (n -> node.fdef -> params){
                print_list(n -> node.fdef -> params);
            }
            printf(") {\n");
            print_ast( n -> node.fdef -> code);
            depth--;
            print_tab(depth);
            printf("}\n\n");
            break;
        case FOR_T:
            printf("for(");
            if(n -> node.forn -> init)
                print_node(n -> node.forn -> init);
            printf("; ");
            if(n -> node.forn -> cond)
                print_node(n -> node.forn -> cond);
            printf("; ");
            if(n -> node.forn -> update)
                print_node(n -> node.forn -> update);
            printf(") {\n");
            print_ast(n -> node.forn -> stmt);
            depth--;
            print_tab(depth);
            printf("}\n");
            break;
        case IF_T:
            printf("if(");
            print_node(n -> node.ifn -> cond);
            printf(") {\n");
            print_ast(n -> node.ifn -> body);
            depth--;
            print_tab(depth);
            printf("}");
            if(n -> node.ifn -> else_body) {
                printf("else{\n");
                depth++;
                print_ast(n -> node.ifn -> else_body);
                depth--;
                print_tab(depth);
                printf("}\n");
            }else{
                printf("\n");
            }
            break;
        case ERROR_NODE_T:
            printf("error node");
    }
}

// Function to print AST node
void print_list(struct AstNode *l) {
    while(l){
        print_node(l);
        l = l->next;
        
        if(l){
            printf(", ");
        }
    }
}












// Main function for the translation phase
void translate(struct AstNode * n){
    fp = fopen("translation.py", "w");
    if(!fp) {
        fprintf(stderr, RED "error:" RESET " %s: ", filename);
        perror("");
        exit(1);
    }

    translate_ast(n);

    if(main_flag) {
        // entry point of the program
        fprintf(fp, "\n");
        fprintf(fp, "if __name__ == \"__main__\":\n");
        fprintf(fp, "\tmain()\n");
    }

    fclose(fp);
}

// Function to translate AST
void translate_ast(struct AstNode *n){
    while(n){
        if(n -> node_type != DECL_T){
            translate_tab(depth);
        }
        if(n -> node_type == FDEF_T || n -> node_type == FOR_T || n -> node_type == IF_T) {
            depth++;
        }

        translate_node(n);

        if(n -> node_type != DECL_T && n -> node_type != FDEF_T && n -> node_type != FOR_T && n -> node_type != IF_T){
            fprintf(fp, "\n");
        }

        n = n->next;
    }
}

// Function to translate AST node
void translate_node(struct AstNode *n){
    switch(n -> node_type){
        case VAL_T:
            if(n -> node.val -> val_type == STRING_T) {
                fprintf(fp, "\"");
                fprintf(fp, "%s", n -> node.val -> string_val);
                fprintf(fp, "\"");
            }else{
                fprintf(fp, "%s", n -> node.val -> string_val);
            }
            break;

        case VAR_T:
            if(n -> node.var -> by_reference) {
                fprintf(fp, "&");
            }
            fprintf(fp,"%s", n -> node.var-> name);
            if(n -> node.var -> array_dim) {
                fprintf(fp, "[");
                translate_node(n -> node.var -> array_dim);
                fprintf(fp, "]");
            }
            break;

        case DECL_T:
            if (n -> node.decl -> var -> node.var -> array_dim)
            {
                // array declaration
                translate_tab(depth);
                fprintf(fp, "%s = ", n -> node.decl -> var -> node.var -> name);
                if (n ->node.decl -> type == CHAR_T) {
                    // for strings: array = ["" for _ in range(array_dim)]
                    fprintf(fp, "[\"\" for _ in range(");
                    translate_node(n -> node.decl -> var -> node.var -> array_dim);
                    fprintf(fp, ")]\n");
                } else {
                    // for int: array = [int()] * int(array_dim)
                    // for float: array = [float()] * int(array_dim)
                    fprintf(fp, "[%s()] * int(", convert_python_type(n ->node.decl -> type));
                    translate_node(n -> node.decl -> var -> node.var -> array_dim);
                    fprintf(fp, ")\n");
                }
            } else {
                // normal declaration
                translate_decl(n -> node.decl -> var, convert_python_type(n ->node.decl -> type));
            }
            break;

        case EXPR_T:
            if(n -> node.expr -> expr_type == PAR_T) {
                // translate parenthesis
                fprintf(fp,"(");
                translate_node(n -> node.expr -> r);
                fprintf(fp, ")");
            } else {
                if(n -> node.expr -> l) // if there is the left operator
                    translate_node(n -> node.expr -> l);
                fprintf(fp, "%s", convert_expr_type(n -> node.expr -> expr_type));
                translate_node(n -> node.expr -> r);
            }
            break;

        case RETURN_T:
            if(n -> node.ret -> expr) {
                fprintf(fp,"return ");
                translate_node(n -> node.ret -> expr);
            }
            break;

        case FCALL_T:
            // normal function
            if (strcmp(n -> node.fcall -> name,"printf")!=0 && strcmp(n -> node.fcall -> name,"scanf")!=0) {
                fprintf(fp,"%s(", convert_func_name(n -> node.fcall -> name));
                if(n -> node.fcall -> args) {
                    translate_list(n-> node.fcall -> args, n -> node.fcall -> name);
                }
                fprintf(fp, ")");
            } 

            // printf
            if (!strcmp(n -> node.fcall -> name,"printf")){
                fprintf(fp,"%s(", convert_func_name(n -> node.fcall -> name));
                if(n -> node.fcall -> args) {
                    translate_list(n-> node.fcall -> args, n -> node.fcall -> name);
                }
                fprintf(fp, ")");
            }

            // scanf
            if (!strcmp(n -> node.fcall -> name,"scanf")){
                struct AstNode * l = n-> node.fcall -> args;

                // extract format specifier
                struct AstNode *fs = extract_format_specifier(l);
                if (fs) {
                    char* s1 = "";
                    char* s2 = concat(convert_fs_type(fs->node.val->string_val), "(input())");
                    s1 = concat(s1,s2);
                    fs = fs -> next;
                    while (fs) {
                        s2 = concat(convert_fs_type(fs->node.val->string_val), "(input())");
                        s2 = concat(", ",s2);
                        s1 = concat(s1,s2);

                        fs = fs -> next;
                    }

                    // on variables
                    l = l->next;
                    fprintf(fp, "%s", l -> node.var ->name);
                    l = l->next;
                    while(l){
                        fprintf(fp,", ");
                        fprintf(fp, "%s", l->node.var ->name);
                        l = l->next;
                    }
                    fprintf(fp," = %s", s1);
                }
                
            }
            break;

        case FDEF_T:
            // def function_name (arg1: type1, arg2: type2, ecc.) -> return_type:
            fprintf(fp,"def ");
            fprintf(fp,"%s(", n -> node.fdef -> name );
            if (n -> node.fdef -> params){
                translate_param_list(n -> node.fdef -> params);
            }
            fprintf(fp, ")");
            if (n -> node.fdef -> ret_type != VOID_T)
            {
                fprintf(fp, " -> %s", convert_python_type(n -> node.fdef -> ret_type));
            }
            fprintf(fp,":\n");
            translate_ast( n -> node.fdef -> code);
            fprintf(fp,"\n");
            depth--;
            translate_tab(depth);
            break;

        case FOR_T:
            //transforming a for loop in a while loop

            // initialization
            // while condition:
            //      ...
            //      update

            if(n -> node.forn -> init) {
                translate_node(n -> node.forn -> init);
                fprintf(fp, "\n");
            }

            translate_tab(depth-1);
            fprintf(fp,"while ");
            if(n -> node.forn -> cond)
                translate_node(n -> node.forn -> cond);
            else
                fprintf(fp,"True");
            fprintf(fp,":\n");

            translate_ast(n -> node.forn -> stmt);
            
            translate_tab(depth);
            if(n -> node.forn -> update) {
                translate_node(n -> node.forn -> update);
                fprintf(fp, "\n");
            }
            depth--;
            break;

        case IF_T:
            fprintf(fp, "if (");
            translate_node(n -> node.ifn -> cond);
            fprintf(fp,"):\n");
            
            translate_ast(n -> node.ifn -> body);
            depth--;
            
            if(n -> node.ifn -> else_body) {
                translate_tab(depth);
                fprintf(fp,"else:\n");
                depth++;
                translate_ast(n -> node.ifn -> else_body);
                depth--;
            }
            break;
    }
}


// Function to translate variables (non array)
void translate_decl(struct AstNode *n, char* type){
    translate_tab(depth);
    
    char* s1 = n -> node.var-> name; // first var
    char* s2 = concat(type,"()"); // first type
    
    n = n -> next;
    while(n){
        if(n -> node_type == VAR_T) {
            // other vars
            char* x = concat(", ", n -> node.var-> name);
            s1 = concat(s1,x);
            // other types
            char* y = concat(", ", type);
            y = concat(y, "()");
            s2 = concat(s2,y);
        } else if(n -> node_type == EXPR_T) {
            translate_node(n -> node.expr -> l);
            fprintf(fp, " %s %s ", type, convert_expr_type(n -> node.expr -> expr_type));
            translate_node(n -> node.expr -> r);
        }
        n = n -> next;
    }

    fprintf(fp, "%s = %s \n", s1, s2);
}


// Function to translate node list
void translate_list(struct AstNode *l, char * func_name) {
    if (strcmp(func_name, "scanf")) {
        translate_node(l);
    }
    l = l->next;
    int f = 0;
    if (!strcmp(func_name, "printf") && l) {
        fprintf(fp," %% (");
        f = 1;
    }
    while(l){
        translate_node(l);
        l = l->next;
        if(l){
            fprintf(fp,", ");
        }
    }

    if (!strcmp(func_name, "printf") && f==1) {
        fprintf(fp,")");
    }
}

// Function to translate parameter list

void translate_param_list(struct AstNode *l){
    while(l){
        translate_param_decl(l);
        l = l->next;
        
        if(l){
            fprintf(fp,", ");
        }
    }
}

// Function to translate parameter declaration
void translate_param_decl(struct AstNode *p){
    // def f1(a: int, b: float, c: str, d: list[int], e: list[float], f: list[str]) -> int:
    fprintf(fp,"%s: ", p -> node.decl -> var -> node.var-> name);
    if(p -> node.decl -> var -> node.var -> array_dim){
        fprintf(fp, "list[");
    }
    fprintf(fp,"%s", convert_python_type(p -> node.decl -> type));
    if(p -> node.decl -> var -> node.var -> array_dim){
        fprintf(fp, "]");
    }
}

// Function to translate indentation given the level
void translate_tab(int d) {
    for(int i = 0; i < d; i++)
        fprintf(fp, "\t");
}









// Function to convert C data types (enum TYPE)
char * convert_var_type (int t){
    switch(t) {
        case INT_T:
            return "int";
        case FLOAT_T:
            return "float";
        case STRING_T:
        case CHAR_T:
            return "char*";
        case VOID_T:
            return "void";
        case BOOL_T:
            return "bool";
        default:
            return "unrecognized data type";
    }
}

// Functino to convert expression type (enum EXPRESSION_TYPE)
char * convert_expr_type(int t){
    switch(t){
        case ASS_T:
            return "=";
        case ADD_T:
            return "+";
        case DIV_T:
            return "/";
        case MUL_T:
            return "*";
        case NOT_T:
            return "!";
        case AND_T:
            return "and";
        case OR_T:
            return "or";
        case G_T:
            return ">";
        case GE_T:
            return ">=";
        case L_T:
            return "<";
        case LE_T:
            return "<=";
        case EQ_T:
            return "==";
        case NE_T:
            return "!=";
        case SUB_T:
        case NEG_T:
            return "-";
    }
}

// Function to convert I/O function ID
char * convert_func_name(char *name){
    if (!strcmp("printf", name))
            return "print";
    if (!strcmp("scanf", name))
            return "input";
    return name;
}

// Function to convert C data types in the corresponding Python data types
char * convert_python_type (int t){
    switch(t) {
        case STRING_T:
        case CHAR_T:
            return "str";
        case INT_T:
            return "int";
        case FLOAT_T:
            return "float";
        case VOID_T:
            return "";
        case BOOL_T:
            return "bool";
        default:
            return "unrecognized type";
    }
}

// Function to convert format specifier to the relative input type
char* convert_fs_type(char* fs){
    char str[2];
    str[0] = fs[0];
    str[1] = '\0';
    if (!strcmp(str,"d") || !strcmp(str,"i")) {
        return "int";
    }
    if (!strcmp(str,"f") || !strcmp(str,"F") || !strcmp(str,"g") || !strcmp(str,"G") || !strcmp(str,"e") || !strcmp(str,"E")){
        return "float";
    }
    if (!strcmp(str,"s")) {
        return "str";
    }
    return "unrecognized fs";
}