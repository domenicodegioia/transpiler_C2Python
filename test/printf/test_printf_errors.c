void main() {
    // too few arguments to function printf
    printf();

    int v1[1];
    float v2[1];
    char* v3[1];
    
    // cannot use a type []int(/float/char*) as type int(/float/char*)
    // printf("%d %f %s", v1[0], v2[0], v3[0]);    -->     OK
    printf("%d %f %s", v1, v2, v3);
}