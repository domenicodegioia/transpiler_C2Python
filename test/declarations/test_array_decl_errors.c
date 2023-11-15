int f(int i){
    int i; // parameter redeclaration
    return i;
}

void main(){
    int a[-2]; // negative size
    float b[2-5]; // negative size (from expression)

    float f;
    f = 10.2;
    int d[f]; // non-integer size

    int i;
    i = 1;
    int e[i + f + 1]; // float element in the expression

    char* h["ciao"];  // non-integer size
    
    int c[(1 != 1) <= 0];

    z + 4;  // undeclared variable
    
    //int x[i=0];  // invalid operator (syntax error)

    i = 10.2323; // constant truncated
    i = (23 == 8);

    float f; //variable redeclaration
}