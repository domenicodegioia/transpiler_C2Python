void f1(int a){
    printf("%d \n", a);
}
void f2(float a){
    printf("%f \n", a);
}
void f3(char* a){
    printf("%s \n", a);
}

int f4(){
    return 2;
}
float f5(){
    return 2.345;
}
char* f6(){
    return "ABC";
}

void f(float x){
    printf("%f", x);
}

void main() {  
    int x1, x2;
    float y;
    char* z;
    x1 = 1;
    x2 = x1 + 2;
    y = 2.2;
    z = "asd";

    f1(x1+x2);
    f2(1.1+y);
    f3(z);

    printf("%i \t %f \t %s \n", f4(), f5(), f6());

    f(f5() + f5());
}