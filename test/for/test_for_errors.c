int f() {
    return 2;
}

void main() {
    int i;
    // non boolean 
    for(i = 0; i; i = i + 1)
        printf("error");
        
    for(i = 0; 1; i = i + 1)
        printf("error");

    for(i = 0; -1; i = i + 1)
        printf("error");

    for(i = 0; 1 - 1; i = i + 1)
        printf("error");

    for(i = 0; 1 * 1; i = i + 1)
        printf("error");

    for(i = 0; f(); i = i + 1)
        printf("error");
}