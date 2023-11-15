void main() {
    // too few arguments to function scanf
    scanf();

    // scanf with no format specifier
    scanf("hello");

    int v1[1];
    float v2[1];
    char* v3[1];

    // cannot use a type []int(/float/char*) as type int(/float/char*)
    // scanf("%d %f %s", v1[0], v2[0], v3[0]);    -->     OK
    scanf("%d %f %s", &v1, &v2, &v3);

    int i;
    float k;
    char* s;

    // mismatch specifiers
    scanf("%i \t %i", &k, &s);
    scanf("%f \t %f \n", &i, &s);
    scanf("%s \t %s \n", &i, &k);

    // too few arguments to format string
    scanf("%d %i %i", &i, &j);

    // too many arguments to format string
    scanf("%s", &s, &k);r
}
