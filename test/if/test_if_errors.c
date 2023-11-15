int f() {
    return 2;
}

void main() {

    // non boolean 
    if (1)
        printf("%d", 1);
    if (-1)
        printf("%d", -1);
    if (1-1)
        printf("%d", 1-1);

    if (f())
        printf("%d", f());

    int a;
    a = 1;
    if (a)
        printf("%d", a);
    if (a * 9)
        printf("%d", a);
    
    
    //no condition
    if()
        printf("no condition"); 
}