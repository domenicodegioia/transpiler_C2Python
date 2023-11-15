void main() {
    int a;
    float b, d;
    char* c;
    scanf("%d %f %f", &a, &b, &d);
    scanf("%s", &c);

    int v1[1];
    float v2[1];
    char* v3[1];
    scanf("%d", &v1[0]);
    scanf("%f %s", &v2[0], &v3[0]);

    printf("print int: %d\n", a);
    printf("print float: %f\n", b);
    printf("print char*: %s\n", c);
    printf("print vectors: %d, \t %f, \t %s \n", v1[0], v2[0], v3[0]);
}