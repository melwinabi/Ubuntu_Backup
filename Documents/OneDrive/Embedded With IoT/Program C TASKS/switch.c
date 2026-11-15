#include <stdio.h>
#include <stdlib.h>

int main(){
    
    int x, y;
    char operator;
    printf("Enter the Operator (+,-,*,/)\n");
    scanf("%c", &operator);

    printf("Enter the two numbers: ");
    scanf("%d  %d", &x, &y);
    switch (operator) {
    case '+':
        printf("%d + %d = %d\n", x, y, x + y);
        break;

    case '-':
        printf("%d - %d = %d\n", x, y, x - y);
        break;

    case '*':
        printf("%d * %d = %d\n", x, y, x * y);
        break;
    case '/':
        printf("%d / %d = %d\n", x, y, x / y);
        break;
    default:
        printf("Invalid Operator Input\n");
    }
    printf("%c is successfully completed",operator);
  
    return 0;
}