void main() {
    int x;
    float y;
    char* z;
    x = 10;
    y = 10.5;
    z = "hello";

    // if + else  // OK
    if (1.5 <= 1.2) {
        printf("ok");
    } else {
        printf("no");
    }
    if (x > 10)
        printf("x is greater than 10\n");
    else
        printf("x is equal or smaller than 10\n");

    // if + else if  // OK
    if (x <= 1.2) {
        printf("ok");
        y = y + 2;
    } else if (y == 100){
        printf("no");
    } else {
        printf("bho");
    }

    // nested if
    if (x == 10) { 
        if (x < 15) {
            printf("x is smaller than 15\n"); 
            if (x < 12) 
                printf("x is smaller than 12 too\n");
        }
        else
            printf("x is greater than 15\n"); // CORREGGERE
    } 
}