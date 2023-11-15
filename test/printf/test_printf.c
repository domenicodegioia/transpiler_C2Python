int f1(int x){
    return x;
}
float f2(float x){
    return x;
}
char* f3(char* x){
    return x;
}

void main(){
    int a;
    float b;
    char* c;
    a = 1;
    b = 2.2;
    c = "asd";

    int v1[0];
    float v2[1<a];
    char* v3[5*a];
    v1[0] = 100;
    v2[0] = 57.5789;
    v3[0] = "abcdefg";

    printf("printf int: %d \t %i \n", a, a);
    printf("printf float: %f \t %F \t %e \t %E \t %g \t %G\n", b, b, b, b, b, b);
    printf("printf char*: %s \n", c);
    printf("printf vectors: int %d, \t float %f, \t char* %s \n", v1[0], v2[0], v3[0]);

    printf("printf symbol %% \n");
    printf("printf symbol %%s \n");

    int x, y;
    x = 1;
    y = 1;

    printf("%d \n", x + y );
    printf("%d \n", x * y );
    printf("%d \n", x - y );
    printf("%d \n", x / y );

    printf("%d \t %f \t %s \n", f1(a), f2(b), f3(c));
}