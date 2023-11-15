void main() {
    // missing specificier
    printf("% ");

    // unknown conversion type
    printf("%x %!");

    int i,j;
    float k;
    char* s;

    // too few arguments to format string
    printf("%d %i %i", i, j);

    // too many arguments to format string
    printf("%s", "abcd", k);

    // mismatch specifiers
    printf("%i \t %i", k, s);
    printf("%f \t %f \n", i, s);
    printf("%s \t %s \n", i, k);

    //printf("%i %i %f %s", i, j, k, s); //correct
}