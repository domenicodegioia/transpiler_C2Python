void main() {
    int x;
    float y;
    char* z;

    x = 3 * 2 - 6.5;  // constant truncated to integer

    // bool assigned to int/float/char*
    x = 1<2;
    y = (1<7) && (3 <= 9);
    z = (3 > x) || (y < 1);

    // arithmetic operator not defined on bool and strings
    (1 > 4) / (3 != 6); 
    (2 == 4) >= ("7" < 2);

    // division by zero
    x = 5 / 0;
    x = "y" * 5.5 / (3-6+3);
}