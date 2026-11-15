#include <stdio.h>

int main(void) {
    char ch;

    
    printf("Enter a character: ");
    scanf(" %c", &ch);  

    
    if ((ch >= 'A' && ch <= 'Z') || (ch >= 'a' && ch <= 'z')) {
        
        if (ch >= 'A' && ch <= 'Z') {
            printf("'%c' is an uppercase letter.\n", ch);
        } else {
            printf("'%c' is a lowercase letter.\n", ch);
        }
    }
    
    else if (ch >= '0' && ch <= '9') {
        printf("'%c' is a Intiger.\n", ch);
    }

    else {
        printf("'%c' is a special character.\n", ch);
    }

    return 0;
}
