// function redeclaration
void f() {
    printf("hello");
}
int f() {
    return 3;
}

//mismatch return type
void f1(){
    return 2.2;
}
int f2(){
    return "hahaha";
}
char* f3(float x){
    return x;
}

// missing return statement
int f4(){
    printf("no return statement");
}
float f5(){
    if(1 > 0){
        return 1.5;
    } else {
        if (2<5){
            return 2.2;
        }
    }
}

void f6(int n){
    int n; // parameter redeclaration
    printf("%d %d", m, n); // argument not declared
}

// main function must have no arguments and no return type
int main(int arg){
    int a,b;
    a=1;
    b=1;

    b(); // not a function

    // mismatch in number of argument
    f6();
    f6(a,b);

    return 0;
}