void f() {
    printf("%s", s);
}

void main() {
    char* s;  // // Scope of this variable is within main() function only
    s = "hello";
    printf("%s", s);
}