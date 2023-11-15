int i,j,k,N;

void f() {
    printf("hello");
}

void main() {
    // for loop without curly braces
    for (i = 1; i <= 10; i=i+1)
        printf("%d ", i);

    //simple for loop
    N=5;
    for (i=0; i<N; i=i+1) {
        printf("hello");
        f();
    }

    // loop without assignment (only expression)
    for (j=0; k <3; j+3) {
        int a;
        a=4+1;
        printf("hello");
        printf("%d %d %d", i, j, k);
    }

    // // Infinite for Loop/NULL Parameter Loop
    for (;;) // no condition -> while True:
    {
        printf("hello");
        f();
    }
}