void main(){
    int a;

    a = -3 * -3;
    printf("%d \n", a);

    a = (a + 2 * 2) / ((6 + 2 )* 2);
    printf("%d \n", a);

    // float const to int
    int b, c, d;
    b = 9.0;
    c =  12.4 - 3.4;
    d = 10 * (2.8 - 3.8) + 11;

    printf("%d \t %d \t %d \n", a, b, c);

    // expression without assignment (legit)
    a + b;
    1 + 2;

    // logical operators on bool
    (1<7) && (3 <= 9);
}