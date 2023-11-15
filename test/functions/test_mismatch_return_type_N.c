// VOID_T
void f(){
    return ;
}
void f1(){
    return 2; //error
}
void f2(){
    return 2.2; //error
}
void f3(){
    return "abc"; //error
}

// INT_T
int f4(){
    return 2;
}
int f5(){
    return 2.0;
}
int f6(){
    return 5.7; // constant truncated
}
int f7(){
    return "abc"; //error
}
int f8(){
    return ; //error
}

// FLOAT_T
float f9(){
    return 2;
}
float f10(){
    return 2.0;
}
float f11(){
    return 5.7;
}
float f12(){
    return "abc"; //error
}
float f13(){
    return ; //error
}

// CHAR_T
char* f14(){
    return 2; //error
}
char* f15(){
    return 2.0; //error
}
char* f16(){
    return 5.7; //error
}
char* f17(){
    return "abc";
}
char* f18(){
    return ; //error
}