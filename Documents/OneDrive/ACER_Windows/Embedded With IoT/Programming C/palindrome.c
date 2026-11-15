#include <stdio.h>

int main()
{
    int a,b;
    int reverse = 0; 

    printf("                        Palindriome Sequence  \n");
    printf("                     ..........................  \n");
    printf("Enter The Number > ");
    scanf("%d", &a); // Fix scanf
    b=a;
    while (a != 0)
    {
        int part1 = a % 10; // > 1228 % 10 = 8
        reverse = reverse * 10 + part1; // > 0 * 10 + 8
        a /= 10; // > 1228 / 10 = 122
    }

    printf("%d\n", reverse);
    if(reverse == b)
    {
        printf("The Number %d is Palindrome",b);
    }else
    {
        printf("The Number %d is Not Palindrome",b);
    }
}
