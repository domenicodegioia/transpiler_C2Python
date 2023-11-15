#ifndef GLOBAL_H
#define GLOBAL_H

#include <string.h>


/* colors of error messages */

#define RESET   "\033[0m"
#define YELLOW  "\033[1m\033[33m"
#define RED     "\033[1m\033[31m"
#define BLUE    "\033[1m\033[34m"
#define BOLD    "\033[1m\033[37m"


/* error management */

void yyerror(const char *s);
void yywarning(char *s);
void yynote(char *s, int lineno, char *line);
char* error_string_format(char *msg, ...);


#endif