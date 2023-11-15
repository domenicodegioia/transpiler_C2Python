compiler: parser.c scanner.c token.h ast.h ast.c symbol_table.h symbol_table.c semantic.h semantic.c translate.h translate.c
	gcc parser.c scanner.c ast.c symbol_table.c semantic.c translate.c -lfl -o compiler

parser.c: parser.y
	bison --defines=token.h -o parser.c parser.y

scanner.c: scanner.l
	flex -o scanner.c scanner.l

clean:
	rm scanner.c parser.c token.h compiler translation.py