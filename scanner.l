/* to maintain the line count */
%option yylineno

/* state for COMMENTS */
%x MULTI_LINE_COMMENT
%x QUOTE
/* INITIAL is predefined */

%{
    #include "token.h"
    #include "symbol_table.h"
    #include "global.h"
    #include <stdio.h>
    #include <stdlib.h>
    #include <stdarg.h>
    #include <string.h>

    extern char *filename; 

    extern int error_num;

    char *line;
    void copy_line();

%}

%%


^.*                                 { copy_line(); }


    /* SINGLE-LINE COMMENTS */
"//".*							    { }


    /* MULTI-LINE COMMENTS */
"/*"                                            { BEGIN MULTI_LINE_COMMENT; }
<MULTI_LINE_COMMENT>[^*\n]*                     { }
<MULTI_LINE_COMMENT>"*/"                        { BEGIN INITIAL; }
<MULTI_LINE_COMMENT><<EOF>>                     { yyerror("Unterminated comment."); BEGIN INITIAL; }   /* BEGIN INITIAL -> avoid loop */
    /* <<EOF>> means "end of file" */


    /* STRINGS */
\"\"                                { yylval.s = " "; return STRING_VALUE; }
\"                                  { BEGIN QUOTE; }
<QUOTE>([^"\\\n]|\\.)*              { yylval.s = strdup(yytext); return STRING_VALUE; }
<QUOTE>\"                           { BEGIN INITIAL; }
<QUOTE>\n |
<QUOTE><<EOF>>                      { yyerror("missing terminating \" character"); BEGIN INITIAL; }


    /* NUMERICAL CONSTANTS */
[0]+ |
[1-9][0-9]*                         { yylval.s = strdup(yytext); return INT_VALUE; }  /* strdup duplicates strings */
[0]+[0-9]+                          { yyerror("octal literal not allowed"); } /* ex. 01, 010 */
(e|E)(\+|-)?[0-9]+ |
([0-9]+)?(\.[0-9]+)|([0-9]+)\. |
([0-9]+|([0-9]+)?(\.[0-9]+)|([0-9]+)\.)(e|E)(\+|-)?[0-9]+ { yylval.s = strdup(yytext); return FLOAT_VALUE; }


    /* KEYWORD */
"if"                                { return IF; }
"else"                              { return ELSE; }
"for"                               { return FOR; }

"scanf"                             { yylval.s = strdup(yytext); return SCANF; }
"printf"                            { yylval.s = strdup(yytext); return PRINTF; }

"void"                              { return VOID; }
"return"                            { return RETURN; }

"char*"      					    { return CHAR; }
"int"                               { return INT; }
"float"                             { return FLOAT; }


    /* IDENTIFIERS */
[_a-zA-Z][_a-zA-Z0-9]*              { yylval.s = strdup(yytext); return ID; }


    /* ARITHMETICAL OPERATORS */
"+" |
"-" |
"*" |
"/" |
"="                                 { return yytext[0]; }

    /* LOGICAL OPERATORS */
"!"                                 { return NOT; }
"&&"                                { return AND; }
"||"                                { return OR; }

    /* MATCHING OPERATORS */
">" |
"<"                                 { return yytext[0]; }                    
">="                                { return GE; }
"<="                                { return LE; }
"=="                                { return EQ; }
"!="                                { return NE; }

    /* OTHERS */
"{" |
"}" |
"(" |
")" |
"[" |
"]" |
";" |
"," |
":" |
"." |
"&"                                 { return yytext[0]; }

    /* WHITESPACES */
[ \t\v\f\r\n]                       { }


    /* CHARACTER NOT RECOGNIZED */
.                           { yyerror(error_string_format("Character" BOLD " %s not recognized" RESET, yytext)); }


%%


// Function to copy input lines
void copy_line() {
    if(line) {
        free(line);
    }

    line = malloc(sizeof(char) * (yyleng + 1)); 
    strcpy(line, yytext);

    yyless(0); // returns the line to the input buffer to match it with the successive rules
}

// Variadic function to support the functions for printing errors, warnings and notes
char* error_string_format(char *msg, ...) {
    va_list args;

    va_start(args, msg);
    int size = vsnprintf(NULL, 0, msg, args) + 1;
    va_end(args);

    char *buffer = malloc(size);
    va_start(args, msg);
    vsprintf(buffer, msg, args);
    va_end(args);
    
    return buffer;
}

// Function to print errors on the standard error, updating the error counter
void yyerror(const char *s) {
    fprintf(stderr, "%s:%d " RED "error:" RESET " %s\n", filename, yylineno, s);
    fprintf(stderr, "%s\n", line);
    error_num++;
}

// Function to print warnings on the standard error
void yywarning(char *s) {
    /* per stamparlo su stderr */
    fprintf(stderr, "%s:%d " YELLOW "warning:" RESET " %s\n", filename, yylineno, s);
    fprintf(stderr, "%s\n", line);
}

// Function to print notes on the standard error
void yynote(char *s, int lineno, char *line){
    fprintf(stderr, "%s:%d " BLUE "note:" RESET " %s\n", filename, lineno, s);
    fprintf(stderr, " %s\n", line);
}