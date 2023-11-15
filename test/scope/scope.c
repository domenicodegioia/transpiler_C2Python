// variable declared in global scope
int b;

void f(int c){
    // global variable accessed from within a function
    printf("FUNCTION: local var:%d  \t global var: %d \n", c, b);

    // local variable
    for(int b=0; b<2; b = b+ 1){
        printf("For scope %d \t func variable %d \n", b, c);
    }
}

void main(){
    int a;
    a = 1;

    b = 2;

    printf("MAIN: local var:%d  \t global var: %d \n", a, b);

    f(3);
}