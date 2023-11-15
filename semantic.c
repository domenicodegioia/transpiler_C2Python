#include "semantic.h"
#include <stdio.h>
#include <stdlib.h>
#include "symbol_table.h"
#include "global.h"
#include "translate.h"

extern int main_flag;
extern struct symbol_list *current_symbol_table;

// flag indicating if there is a division by 0 in eval_array_dim
int div_by_zero;

// Function to perform function call checks
void check_fcall(char *name, struct AstNode *args)
{
    /*
        name : function name
        args : pointer to arguments list
    */

    struct symbol *s = find_symbol_table(current_symbol_table, name);

    if (!s)
    {
        // check if the symbol has been declared
        yyerror(error_string_format("function " BOLD "%s" RESET " undeclared", name));
    }
    else if (s->sym_type != FUNCTION)
    {
        // check if the symbol is actually a function
        yyerror(error_string_format("called object " BOLD "%s" RESET " is not a function", name));
    }
    else
    {
        struct AstNode *a = args;
        struct AstNode *p = s->params_list;

        // check on the consistency of the number and type of arguments passed to the function
        while (a && p)
        {
            if (a->node_type == VAR_T)
            {
                // if the argument is a variable
                struct symbol *arg = find_symbol_table(current_symbol_table, a->node.var->name);

                // heck if it has been declared
                if (arg == NULL)
                    break;

                if (arg->type == p->node.decl->type)
                {
                    // check on arrays
                    if (arg->array)
                    {
                        // if a is an array and f expects an array but we pass it an element
                        // example:  f(a[1]) but f is defined int f(int a[]){...}
                        if (a->node.var->array_dim && p->node.decl->var->node.var->array_dim)
                        {
                            yyerror(error_string_format("cannot use " BOLD "%s" RESET " (type " BOLD "%s" RESET ") as type " BOLD "[]%s" RESET " in argument to " BOLD "%s" RESET,
                                                        arg->name, convert_var_type(arg->type), convert_var_type(p->node.decl->type), name));
                            // if a is an array and f expects a variable but we pass it an array
                            // example: f(a) but f is defined int f(int a){...}
                        }
                        else if (!a->node.var->array_dim && !p->node.decl->var->node.var->array_dim)
                        {
                            yyerror(error_string_format("cannot use " BOLD "%s" RESET " (type " BOLD "[]%s" RESET ") as type " BOLD "%s" RESET " in argument to " BOLD "%s" RESET,
                                                        arg->name, convert_var_type(arg->type), convert_var_type(p->node.decl->type), name));
                        }
                    }
                    else
                    {
                        // if a is not an array and f expects an array
                        // example: int a; f(a) but f is defined int f(int a[]){...}
                        enum TYPE type = eval_expr_type(a).type;
                        if (type != ERROR_T && p->node.decl->var->node.var->array_dim)
                        {
                            yyerror(error_string_format("cannot use " BOLD "%s" RESET " (type " BOLD "%s" RESET ") as type " BOLD "[]%s" RESET " in argument to " BOLD "%s" RESET,
                                                        arg->name, convert_var_type(type), convert_var_type(p->node.decl->type), name));
                        }
                    }
                }
                else
                {
                    // if argument and parameter type is different
                    yyerror(error_string_format("type mismatch in function " BOLD "%s" RESET, name));
                }
            }
            else
            {
                // if the argument is an expression
                struct complex_type res = eval_expr_type(a);
                enum TYPE type = res.type;

                // check on constant truncation
                // example: int f(int a){...} and call f(1.2); -> constant truncation
                if (res.kind == N && res.type != INT_T && p->node.decl->type == INT_T)
                {
                    check_const_truncated(a);
                }
                else if (type != ERROR_T && res.kind == V && type != p->node.decl->type)
                {
                    // argument and parameter type is different
                    yyerror(error_string_format("type mismatch in function " BOLD "%s" RESET, name));
                }
                else
                {
                    if (type != ERROR_T && p->node.decl->var->node.var->array_dim)
                        // the function expects an array but the argument is an expression
                        yyerror(error_string_format("cannot use " BOLD "%s" RESET " (type " BOLD "%s" RESET ") as type " BOLD "[]%s" RESET " in argument to " BOLD "%s" RESET,
                                                    a->node.var->name, convert_var_type(type), convert_var_type(p->node.decl->type), name));
                }
            }
            a = a->next;
            p = p->next;
        }

        // if the number of arguments passed to the function is different
        if (a != p && a == NULL)
        {
            yyerror(error_string_format("too few arguments to function " BOLD "%s" RESET, name));
        }
        else if (a != p && p == NULL)
        {
            yyerror(error_string_format("too many arguments to function " BOLD "%s" RESET, name));
        }
    }
}

// Function to check if the return type of the function coincide the one of the function declaration
void check_return(struct AstNode *expr)
{
    /*
        expr : expression in the return statement
               N.B. NULL if there is return ; */
    struct symbol *s = find_symbol_table(current_symbol_table, "return"); // return type 

    if (expr) {
        if (s->type == VOID_T) {
            // void function but return expression
            yyerror(BOLD "return" RESET " with a value, in function returning void");
            yynote(error_string_format("function declaration was here"), s->lineno, s->line);
        } else {
            // check on the expression type
            struct complex_type ret_expr = eval_expr_type(expr);

            if (ret_expr.kind == N)
            {
                if (s->type==INT_T){
                    if (ret_expr.type==FLOAT_T)
                        check_const_truncated(expr);
                    else if (ret_expr.type==STRING_T)
                        yyerror(error_string_format("cannot use a " BOLD "%s" RESET " as type " BOLD "%s" RESET " in return argument", convert_var_type(ret_expr.type), convert_var_type(s->type)));
                        yynote(error_string_format("function declaration was here"), s->lineno, s->line);
                }
                else if (s->type==FLOAT_T && (ret_expr.type!=FLOAT_T && ret_expr.type!=INT_T)){
                    yyerror(error_string_format("cannot use a " BOLD "%s" RESET " as type " BOLD "%s" RESET " in return argument", convert_var_type(ret_expr.type), convert_var_type(s->type)));
                    yynote(error_string_format("function declaration was here"), s->lineno, s->line);
                }
                else if (s->type==CHAR_T && ret_expr.type!=STRING_T){
                    yyerror(error_string_format("cannot use a " BOLD "%s" RESET " as type " BOLD "%s" RESET " in return argument", convert_var_type(ret_expr.type), convert_var_type(s->type)));
                    yynote(error_string_format("function declaration was here"), s->lineno, s->line);
                }
                
            }
            else // ret_expr.kind == N
            {
                // if the expression is not constant, the two types must be equal
                if (s->type != ret_expr.type)
                {
                    yyerror(error_string_format("cannot use a " BOLD "%s" RESET " as type " BOLD "%s" RESET " in return argument", convert_var_type(ret_expr.type), convert_var_type(s->type)));
                    yynote(error_string_format("function declaration was here"), s->lineno, s->line);
                }
            }
        }
    } else {
        // if the expression is NULL and the function type is not void
        if (s->type != VOID_T)
        {
            yyerror(BOLD "return" RESET " with no value, in function returning non-void");
            yynote(error_string_format("function declaration was here"), s->lineno, s->line);
        }
    }
}

// Function to check if there is a return statement inside the function
void check_func_return(enum TYPE type, struct AstNode *code)
{
    /*
        type : type declared in the function
        code : pointer to the body function */

    int ret = 0;

    if (type != VOID_T)
    {
        for (struct AstNode *tmp = code; tmp != NULL; tmp = tmp->next)
        {
            // check if the last statement in the function is an IF or a RETURN
            if (tmp->next == NULL && tmp->node_type == IF_T)
            {
                ret = check_if_return(tmp); // if it is an IF, check if there is a RETURN in the IF branches
            }
            // if the last statement in the function is a RETURN
            if (tmp->next == NULL && tmp->node_type == RETURN_T)
            {
                ret = 1;
            }
        }

        if (!ret)
        {
            yyerror("missing return at end of function");
        }
    }
}

// Function to check if there is a return statement inside the if-else construct
int check_if_return(struct AstNode *tmp)
{
    /*
        tmp : if construct
    */
    int ret;

    // check on the if body
    for (struct AstNode *body = tmp->node.ifn->body; body != NULL; body = body->next) {
        // if the last statement is an IF, check recursively
        if (body->next == NULL && body->node_type == IF_T)
            ret = check_if_return(body);
        // if the last statement is a RETURN, RETURN is in the IF branch
        if (body->next == NULL && body->node_type == RETURN_T)
            ret = 1;
        // if the last statement is not a RETURN, there is not a RETURN in the IF branch
        if (body->next == NULL && body->node_type != RETURN_T && body->node_type != IF_T)
            ret = 0;
    }

    // check on the else body only if there is a RETURN in the IF branch
    if (ret) {
        // if there is not else, there is not a RETURN in the ELSE branch
        if (tmp->node.ifn->else_body == NULL)
        {
            ret = 0;
        }
        else
        {
            for (struct AstNode *else_body = tmp->node.ifn->else_body; else_body != NULL; else_body = else_body->next) {
                // if the last statement is an IF, check recursively
                if (else_body->next == NULL && else_body->node_type == IF_T)
                    ret = check_if_return(else_body);
                // if the last statement is a RETURN, RETURN is in the IF branch
                if (else_body->next == NULL && else_body->node_type == RETURN_T)
                    ret = 1;
                // if the last statement is not a RETURN, there is not a RETURN in the IF branch
                if (else_body->next == NULL && else_body->node_type != RETURN_T && else_body->node_type != IF_T)
                    ret = 0;
            }
        }
    }
    return ret;
}

// Function to check if the main function does not have arguments and is void type
void check_main(struct AstNode *fdecl)
{
    /*
        fdecl : function declaration node
    */
    if (!strcmp(fdecl->node.fdef->name, "main")) {
        main_flag = 1;
        if (fdecl->node.fdef->ret_type != VOID_T)
            yyerror("function main must have no return values");
        if (fdecl->node.fdef->params)
            yyerror("function main must have no arguments");
    }
}

// Verify the correctness of array size or index
void check_array(struct AstNode *dim)
{
    /*
        dim : array size or index
    */
    if (check_array_dim(dim)) {
        div_by_zero = 0;
        if (eval_array_dim(dim) < 0) {
            yyerror("cannot use negative array index or size");
        }
    }
}

/* Verify if the expression is constant:
        - returns 1: if the expression is integer and costant
        - returns 0: otherwise
        - errors: variables, values and functions with non integer type return;
                operators that return boolean value */
int check_array_dim(struct AstNode *expr)
{
    /*
        expr : expression representing the array size or index
    */
    switch (expr->node_type) {
        case VAL_T:
            if (expr->node.val->val_type != INT_T)
            {
                yyerror("invalid array index or size, non-integer len argument");
                return 0;
            }
            return 1;

        case VAR_T:
            struct symbol *var = find_symbol_table(current_symbol_table, expr->node.var->name);
            if (var != NULL && var->type != INT_T)
            {
                yyerror("invalid array index or size, non-integer len argument");
            }
            return 0;

        case FCALL_T:
            struct symbol *f = find_symbol_table(current_symbol_table, expr->node.fcall->name);
            if (f != NULL && f->type != INT_T)
                yyerror("invalid array index or size, non-integer len argument");
            return 0;

        case EXPR_T:
            int l = 1;
            int r = 1;

            if (expr->node.expr->l)
                l = check_array_dim(expr->node.expr->l);

            if (expr->node.expr->r)
                r = check_array_dim(expr->node.expr->r);

            return l && r;
    }
}

// Function called only if the expression is constant and integer;
// it verifies that the size/index is an integer >= 0
int eval_array_dim(struct AstNode *expr)
{
    /*
        expr : expression representing the array size or index
    */
    switch (expr->node_type)
    {
        int l, r;
    case VAL_T:
        if (expr->node.val->val_type == INT_T)
            return atoi(expr->node.val->string_val);
    case EXPR_T:
        if (expr->node.expr->l)
            l = eval_array_dim(expr->node.expr->l);

        if (expr->node.expr->r)
            r = eval_array_dim(expr->node.expr->r);

        if (div_by_zero)
            return 0;

        switch (expr->node.expr->expr_type)
        {
        case ADD_T:
            return l + r;
        case SUB_T:
            return l - r;
        case DIV_T:
            if (r == 0) {
                div_by_zero = 1;
                return 0;
            }
            return l / r;
        case MUL_T:
            return l * r;
        case NEG_T:
            return -r;
        case PAR_T:
            return r;

            case NOT_T:
                return !r;
            case AND_T:
                return l && r;
            case OR_T:
                return l || r;
            case G_T:
                return l > r;
            case GE_T:
                return l >= r;
            case L_T:
                return l < r;
            case LE_T:
                return l <= r;
            case EQ_T:
                return l == r;
            case NE_T:
                return l != r;
        }
    }
}

// Function to check if the variable has been declared and set his used flag (when the variable is used)
void check_var_reference(struct AstNode *var)
{
    struct symbol *s = NULL;
    s = find_symbol_table(current_symbol_table, var->node.var->name);

    if (!s) {
        // if it has not been declared, error
        yyerror(error_string_format("variable " BOLD "%s" RESET " undeclared", var->node.var->name));
    } else {
        // if it has been declared and used
        s->used = 1;
    }
}

// Function to modify part of a string
char *replace_format(char *string, char *old, char *new)
{
    /*
        string : string to modify
        old : part of the string to replace
        new : new characters
    */
    char *tmp = strdup(string);
    char *result = malloc(strlen(tmp) + 1);

    // p points to the first occurency of the string to replace
    char *p = strstr(tmp, old);

    while (p) {
        *p = '\0';             // add the string end character
        strcat(result, tmp);   // concatenate up to the string end character
        strcat(result, new);   // concatenate the new part of the tsring
        tmp = p + strlen(old); // tmp points after the first occurrency of the replaced string
        p = strstr(tmp, old);  // p points to the first occurrency of the string to replace
    }

    strcat(result, tmp);
    return result;
}

// Function that returns the format specifier list in the format string
struct AstNode *extract_format_specifier(struct AstNode *format_string)
{
    /*
        format_string : AST node from which to extract the format string
        f_type : indicating the function: scanf o printf
    */
    char *s = format_string->node.val->string_val;
    // pointer to the last created element in the specifiers type list+
    struct AstNode *tmp = NULL;
    // pointer to the start of the specifiers type list
    struct AstNode *format_specifier = NULL;

    for (int i = 0; i < strlen(s) - 1; i++)
    {
        if (s[i] == '%')
        {
            switch (s[i + 1])
            {
                char *c;
            case 'd':
            case 'i':
                c = malloc(sizeof(char) + 1);
                strncpy(c, &s[i + 1], 1);
                if (format_specifier) {
                    tmp = append_AstNode(tmp, new_value_node(VAL_T, INT_T, c));
                } else {
                    format_specifier = new_value_node(VAL_T, INT_T, c);
                    tmp = format_specifier;
                }
                break;
            case 'f':
            case 'F':
            case 'e':
            case 'E':
            case 'g':
            case 'G':
                c = malloc(sizeof(char) + 1);
                strncpy(c, &s[i + 1], 1);
                if (format_specifier) {
                    tmp = append_AstNode(tmp, new_value_node(VAL_T, FLOAT_T, c));
                } else {
                    format_specifier = new_value_node(VAL_T, FLOAT_T, c);
                    tmp = format_specifier;
                }
                break;
            case 's':
                c = malloc(sizeof(char) + 1);
                strncpy(c, &s[i + 1], 1);
                if (format_specifier) {
                    tmp = append_AstNode(tmp, new_value_node(VAL_T, STRING_T, c));
                } else {
                    format_specifier = new_value_node(VAL_T, STRING_T, c);
                    tmp = format_specifier;
                }
                break;
            case '%': // print % symbol
                i++;   // not to recognize the second % as type specifier: "%% "   (jump)
                break; 
            case ' ':
                yywarning("conversion lacks type of format string");
                break;
            default:
                // if there is a % not followed by a recognized type
                c = malloc(sizeof(char) + 1);
                strncpy(c, &s[i + 1], 1);
                yywarning(error_string_format("unknown conversion type character " BOLD "%s" RESET " in format string", c));
            }
        }
    }

    return format_specifier;
}

// Function to check the coherence between type specifier and arguments of printf/scanf functions
void check_format_string(struct AstNode *format_string, struct AstNode *args, enum BUILTIN_TYPE f_type)
{
    /*
        format_string : AST node from which to extract the format string
        args : function arguments
        f_type : indicating the function: scanf o printf
    */

    struct AstNode *fs = extract_format_specifier(format_string);
    struct AstNode *a = args;

    // Python uses only %d for integers (%i not allowed)
    format_string->node.val->string_val = replace_format(format_string->node.val->string_val, "%i", "%d");
    // Python uses only '%' to print the symbol %
    format_string->node.val->string_val = replace_format(format_string->node.val->string_val, "%%", "%");

    if (f_type == SCANF_T) {
        if (!fs) {
            yyerror(BOLD"scanf" RESET " with no format specifier");
        }
        while (a && fs) {
            int type = eval_expr_type(a).type;

            if (!a->node.var->by_reference) {
                // error if the reference operator is not used in scanf
                yywarning(error_string_format("format %" BOLD "%s" RESET " expects argument of type " BOLD "%s *" RESET ", but argument " BOLD "%s" RESET " has type " BOLD "%s" RESET,
                                              fs->node.val->string_val, convert_var_type(fs->node.val->val_type), a->node.var->name, convert_var_type(type)));
            } else if (convert_var_type(type) != convert_var_type(fs->node.val->val_type) && type != ERROR_T) {
                // different types between argument and format specifier
                yyerror(error_string_format("format %" BOLD "%s" RESET " expects argument of type " BOLD "%s *" RESET ", but argument " BOLD "%s" RESET " has type " BOLD "%s *" RESET,
                                              fs->node.val->string_val, convert_var_type(fs->node.val->val_type), a->node.var->name, convert_var_type(type)));
            }

            a = a->next;
            fs = fs->next;
        }
    } else if (f_type == PRINTF_T) {
        while (a && fs) {
            int type = eval_expr_type(a).type;

            // different types between argument and format specifier
            if (convert_var_type(type) != convert_var_type(fs->node.val->val_type) && type != ERROR_T){
                yywarning(error_string_format("format %" BOLD "%s" RESET " expects argument " BOLD "%s" RESET " of type " BOLD "%s" RESET ", but argument has type " BOLD "%s" RESET,
                                               fs->node.val->string_val, convert_var_type(fs->node.val->val_type), a->node.var->name, convert_var_type(type)));
            }

            a = a->next;
            fs = fs->next;
        }
    }

    // different numbers between argument and format specifier
    if (a != fs && a == NULL) {
        yyerror("too few arguments to format string");
        while (fs) {
            yywarning(error_string_format("format %" BOLD "%s" RESET " expects a matching " BOLD "%s" RESET " argument",
                                          fs->node.val->string_val, convert_var_type(fs->node.val->val_type)));
            fs = fs->next;
        }
    } else if (a != fs && fs == NULL) {
        yyerror("too many arguments to format string");
    }
}

// Function to evaluate the expression type and if the expression is a constant one or not;
// it returns a complex_type with kind = N if the expression is constant, otherwise kind = V.
struct complex_type eval_expr_type(struct AstNode *expr)
{
    /*
        expr : expression to evaluate
        complex_type => {   kind = V or N
                            type = INT_T,... or ERROR_T }
    */
    switch (expr->node_type)
    {
        struct complex_type res;
        case VAL_T:
            res.kind = N;
            res.type = expr->node.val->val_type;
            return res;

        case VAR_T:;
            struct symbol *var = find_symbol_table(current_symbol_table, expr->node.var->name);
            res.kind = V;
            if (var)
            {
                res.type = var->type;
                // check if the variable is an array
                if (var->array)
                {
                    // check if we can access an array element
                    if (expr->node.var->array_dim)
                        res.type = var->type;
                    else
                    {
                        res.type = ERROR_T;
                        yyerror(error_string_format("cannot use a type " BOLD "[]%s" RESET " as type " BOLD "%s" RESET, convert_var_type(var->type), convert_var_type(var->type)));
                    }
                }
                else
                {
                    // check if used indexing on a variable that is not an array
                    if (expr->node.var->array_dim)
                    {
                        yyerror(error_string_format("invalid operation: " BOLD "%s[%s]" RESET " (type %s does not support indexing)", var->name, expr->node.var->array_dim->node.val->string_val, convert_var_type(var->type)));
                        res.type = ERROR_T;
                    }
                }
            }
            else
                res.type = ERROR_T;
            return res;

        case FCALL_T:;
            struct symbol *fcall = find_symbol_table(current_symbol_table, expr->node.fcall->name);
            res.kind = V;
            if (fcall)
            {
                res.type = fcall->type;
            }
            else
                res.type = ERROR_T;
            return res;
            
        case EXPR_T:;
            struct complex_type l;
            struct complex_type r;

            if (expr->node.expr->l)
            {
                l = eval_expr_type(expr->node.expr->l);
                if (l.type == ERROR_T)
                    return l;
            }

            if (expr->node.expr->r)
            {
                r = eval_expr_type(expr->node.expr->r);
                if (r.type == ERROR_T)
                    return r;
            }

            /* comparison operators */
            if (expr->node.expr->expr_type >= G_T && expr->node.expr->expr_type <= NE_T)
            {
                res = eval_comparison_op_type(l, r, expr->node.expr->expr_type); // OK
                return res;
            }

            /* arithmetical operators */
            if (expr->node.expr->expr_type > ASS_T && expr->node.expr->expr_type <= MUL_T)
            {
                res = eval_arithmetic_op_type(l, r);
                return res;
            }
            else if (expr->node.expr->expr_type == ASS_T)
            {
                res = eval_ass_op(l, r, expr->node.expr->r);
                return res;
            }

            /* parenthesis or minus unary operator */
            if (expr->node.expr->expr_type == PAR_T || expr->node.expr->expr_type == NEG_T)
            {
                // parentesi o op. meno unario non cambiano il tipo di r
                return r;
            }

            /* logical opeators (on boolean operands) */
            if (expr->node.expr->expr_type == NOT_T)
            {
                if (r.type == BOOL_T)
                {
                    res.kind = V;
                    res.type = BOOL_T;
                    return res;
                }
                else
                {
                    res.kind = V;
                    res.type = ERROR_T;
                    yyerror(error_string_format("invalid operation: operator " BOLD "%s" RESET " defined only on bool", convert_expr_type(expr->node.expr->expr_type)));
                    return res;
                }
            }

            if (expr->node.expr->expr_type == AND_T || expr->node.expr->expr_type == OR_T)
            {
                if (l.type == BOOL_T && r.type == BOOL_T)
                {
                    res.kind = V;
                    res.type = BOOL_T;
                    return res;
                }
                else
                {
                    res.kind = V;
                    res.type = ERROR_T;
                    yyerror(error_string_format("invalid operation: operator " BOLD "%s" RESET " defined only on bool", convert_expr_type(expr->node.expr->expr_type)));
                    return res;
                }
            }
    }
}

// Function to evaluate the return type by a comparison operator;
// in case the types of the operands are not comparable, it returns error
struct complex_type eval_comparison_op_type(struct complex_type l, struct complex_type r, enum EXPRESSION_TYPE expr_type)
{
    /*
        l : first operand
        r : second operand
        expr_type : operator
    */
    struct complex_type res;

    // the only comparison operators defined on booleans are == and !=
    // example: (1 > 2) == (3 < 4) -> ok    -----  (5 > 6) < (7 > 8) -> no
    if (expr_type != NE_T && expr_type != EQ_T && (l.type == BOOL_T || r.type == BOOL_T)) {
        yyerror("invalid operation: operation not defined on bool");
        res.type = ERROR_T;
        res.kind = V;
        return res;
    }

    if (expr_type != NE_T && expr_type != EQ_T && ((l.type == CHAR_T || l.type == STRING_T) || (r.type == CHAR_T || r.type == STRING_T))) {
        yyerror("invalid operation: operation not defined on strings");
        res.type = ERROR_T;
        res.kind = V;
        return res;
    }

    // only variables of the same type can be compared
    if (l.kind == V && r.kind == V)
    {
        if (l.type == r.type)
        {
            res.kind = V;
            res.type = BOOL_T;
            return res;
        }
        else
        {
            res.type = ERROR_T;
            res.kind = V;
            yyerror(error_string_format("invalid operation: mysmatched types " BOLD "%s" RESET " and " BOLD "%s" RESET,
                                        convert_var_type(l.type), convert_var_type(r.type))); // update
            return res;
        }
    }

    // you can compare variables and values only if the number can be represented in the type of the variable
    // float x; ...   x < 1 ==> ok   ----   int y; ...  y > 2.3 ==> no
    if (l.kind == V && r.kind == N)
    {
        switch (l.type)
        {
        case FLOAT_T:
        case BOOL_T:
        case INT_T:
            if (r.type == INT_T || r.type == FLOAT_T || r.type == BOOL_T)
            {
                res.kind = V;
                res.type = BOOL_T;
                return res;
            }
            else
            {
                res.type = ERROR_T;
                res.kind = V;
                yyerror(error_string_format("invalid operation: mysmatched types " BOLD "%s" RESET " and " BOLD "%s" RESET,
                                            convert_var_type(l.type), convert_var_type(r.type)));
                return res;
            }
            break;
        case CHAR_T:
        case STRING_T:
            if (r.type == CHAR_T || r.type == STRING_T)
            {
                res.kind = V;
                res.type = BOOL_T;
                return res;
            }
            else
            {
                res.type = ERROR_T;
                res.kind = V;
                yyerror(error_string_format("invalid operation: mysmatched types " BOLD "%s" RESET " and " BOLD "%s" RESET,
                                            convert_var_type(l.type), convert_var_type(r.type)));
                return res;
            }
            break;
        default:
            break;
        }
    }

    if (l.kind == N && r.kind == V)
    {
        switch (r.type)
        {
        case FLOAT_T:
        case BOOL_T:
        case INT_T:
            if (l.type == INT_T || l.type == FLOAT_T || l.type == BOOL_T)
            {
                res.kind = V;
                res.type = BOOL_T;
                return res;
            }
            else
            {
                res.type = ERROR_T;
                res.kind = V;
                yyerror(error_string_format("invalid operation: mysmatched types " BOLD "%s" RESET " and " BOLD "%s" RESET,
                                            convert_var_type(l.type), convert_var_type(r.type))); // update
                return res;
            }
            break;
        case CHAR_T:
        case STRING_T:
            if (l.type == CHAR_T || l.type == STRING_T)
            {
                res.kind = V;
                res.type = BOOL_T;
                return res;
            }
            else
            {
                res.type = ERROR_T;
                res.kind = V;
                yyerror(error_string_format("invalid operation: mysmatched types " BOLD "%s" RESET " and " BOLD "%s" RESET,
                                            convert_var_type(l.type), convert_var_type(r.type)));
                return res;
            }
            break;
        default:
            break;
        }
    }

    // you can compare any values (numeric constant or string)
    if (l.kind == N && r.kind == N)
    {
        if (l.type == r.type)
        {
            res.kind = V; // lasciare V
            res.type = BOOL_T;
            return res;
        }
        else
        {
            res.type = ERROR_T;
            res.kind = V; // lasciare V
            yyerror(error_string_format("invalid operation: mysmatched types " BOLD "%s" RESET " and " BOLD "%s" RESET,
                                        convert_var_type(l.type), convert_var_type(r.type)));
            return res;
        }
    }
}

// Function to evaluate the return type by an arithmetic operator;
// in case the types of the operands are not comparable, it returns error
struct complex_type eval_arithmetic_op_type(struct complex_type l, struct complex_type r)
{
    /*
        l : first operand
        r : second operand
    */

    struct complex_type res;

    // arithmetic operators are not defined on booleans
    if (l.type == BOOL_T || r.type == BOOL_T)
    {
        yyerror("invalid operation: operation not defined on bool");
        res.type = ERROR_T;
        res.kind = V;
        return res;
    }

    if (l.type == CHAR_T || l.type == STRING_T || r.type == CHAR_T || r.type == STRING_T)
    {
        yyerror("invalid operation: operation not defined on strings");
        res.type = ERROR_T;
        res.kind = V;
        return res;
    }

    // it is possible to perform arithmetic operations only if the variables are of the same type
    if (l.kind == V && r.kind == V)
    {
        if (l.type == r.type)
        {
            return l;
        }
        else
        {
            res.type = ERROR_T;
            res.kind = V;
            yyerror(error_string_format("invalid operation: mysmatched types " BOLD "%s" RESET " and " BOLD "%s" RESET,
                                        convert_var_type(l.type), convert_var_type(r.type)));
            return res;
        }
    }

    // it is possible to perform arithmetic operations between variables and numbers only if the number can be represented in the type of the variable
    if (l.kind == V && r.kind == N)
    {
        if (l.type >= r.type)
        {
            return l;
        }
        else
        {
            res.type = ERROR_T;
            res.kind = V;
            yyerror("constant truncated");
            return res;
        }
    }

    if (l.kind == N && r.kind == V)
    {
        if (l.type <= r.type)
        {
            return r;
        }
        else
        {
            res.type = ERROR_T;
            res.kind = V;
            yyerror("constant truncated");
            return res;
        }
    }

    // it is always possible to perform arithmetic operations between numeric constants
    if (l.kind == N && r.kind == N)
    {
        if (l.type && r.type)
        {
            return r;
        }
        else
        {
            return l;
        }
    }
}

// Function to evaluate the return type by an assignment operator;
// in case the types of the operands are not comparable, it returns error
struct complex_type eval_ass_op(struct complex_type l, struct complex_type r, struct AstNode *r_node)
{
    /*
        l : first operand (type of the variable to assign the expression to)
        r : second operand (type of the expression to assign)
        r_node : expression to assign
        example: int a = 1 + 2; ===> r = a
                                    l = complex type returned by 2+1
                                    r_node = 2 + 1
        */

    struct complex_type res;

    // if the expression to be assigned is not constant, the two operands must be of the same type
    if (r.kind == V)
    {
        //printf("assignment %s %s \n", convert_var_type(l.type), convert_var_type(r.type)); // test
        if (l.type == r.type)
            return l;
        else
        {
            res.type = ERROR_T;
            res.kind = V;
            //printf("test\n");
            yyerror(error_string_format("invalid operation: mysmatched types in assignment " BOLD "%s" RESET " and " BOLD "%s" RESET,
                                        convert_var_type(l.type), convert_var_type(r.type)));
            return res;
        }
    }

    // if the expression r to be assigned to l is constant
    if (r.kind == N)
    {
        if (r.type != INT_T && l.type == INT_T)
        {
            /*  if the variable to assign the expression to is integer
                while the expression is constant but not integer, check if there is a truncation
                example: int a = 10.2 + 1; -> truncation (ERROR_T)
                         int a = 10.2 + 0.8 -> no truncation (OK)
            */
            float result = check_const_truncated(r_node);
            if (result != (int)result)
                res.type = ERROR_T;
            else
                res.type = INT_T;
            res.kind = V;
            return res;
        }
        else if (l.type >= r.type)
        {
            // check if the type of the expression can be represented in the type of the variable
            return l;
        }
        else if (l.type == CHAR_T && r.type == STRING_T)
        {
            res.type = CHAR_T;
            res.kind = N;
            return res;
        }
        else
        {
            res.type = ERROR_T;
            res.kind = V;
            yyerror("constant truncated in assignment");
            return res;
        }
    }
}

// Function to check if the type of if/for condition is boolean
void check_cond(enum TYPE type)
{
    if (type != BOOL_T && type != ERROR_T)
    {
        yyerror("non-bool expression used as condition");
    }
}

// Function to compute the result of a constant expression
float eval_constant_expr(struct AstNode *expr)
{
    /*
        expr : expression to evaluate
    */
   
    switch (expr->node_type)
    {
        float l, r;
        case VAL_T:
            return atof(expr->node.val->string_val);
        case EXPR_T:
            if (expr->node.expr->l)
                l = eval_constant_expr(expr->node.expr->l);

            if (expr->node.expr->r)
                r = eval_constant_expr(expr->node.expr->r);
            
            switch (expr->node.expr->expr_type) {
                case ADD_T:
                    return l + r;
                case SUB_T:
                    return l - r;
                case MUL_T:
                    return l * r;
                case DIV_T:
                    return l / r;
                case NEG_T:
                    return -r;
                case PAR_T:
                    return r;
            }
    }
}

// Function to check if a constant expression contains a division by zero
void check_division(struct AstNode *expr)
{
    /*
        expr : expression to evaluate
    */
    if (eval_expr_type(expr).kind == N) {
        if (eval_constant_expr(expr) == 0) {
            yyerror("division by zero");
        }
    }
}

// Function to check for truncation of constants in an expression
// It checks if the expression can be assigned to an integer
float check_const_truncated(struct AstNode *expr)
{
    /*
        expr : expression
    example int a = 12.0 -> ok
            return 9.0 (f int) -> ok
            return 9.2 o int a = 12.1 -> error
    */
    float result = eval_constant_expr(expr);

    if (result != (int)result)
        yyerror(error_string_format("constant " BOLD "%f" RESET " truncated to integer", result));
    return result;
}
