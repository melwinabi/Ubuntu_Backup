#include <stdio.h>
//`#pragma pack(1) //USED TO ELIMINATE PADDING METHORD  ....... ........  ........
struct Example {
    char a;   
    int b;    
    char c ;   
}s; 

int main() {
    printf("%zu", sizeof(s));
    return 0;
}


