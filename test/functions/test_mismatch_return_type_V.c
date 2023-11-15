// variables
int a;
// a=1;
float b;
// b=2.2;
char* c;
// c = "hello";

// VOID_T
void f(){
    return ;
}
void f1(){
    return a; //error
}
void f2(){
    return b; //error
}
void f3(){
    return c; //error
}

// INT_T
int f4(){
    return a;
}
int f5(){
    return b; //error
}
int f6(){
    return b; //error
}
int f7(){
    return c; //error
}
int f8(){
    return ; //error
}

// FLOAT_T
float f9(){
    return a; //error
}
float f10(){
    return b;
}
float f11(){
    return b;
}
float f12(){
    return c; //error
}
float f13(){
    return ; //error
}

// CHAR_T
char* f14(){
    return a; //error
}
char* f15(){
    return b; //error
}
char* f16(){
    return b; //error
}
char* f17(){
    return c;
}
char* f18(){
    return ; //error
}