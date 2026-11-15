#include <stdio.h>
#include <string.h>

char string(char *a) 
{
    int i, len;
    char temp;
    
    printf("Enter a string: ");
    scanf("%[^\n]s", a);

    len = strlen(a);
    printf("%d\n",len);
    
    for (i = 0; i < len / 2; i++) 
    {
        temp = a[i];
        a[i] = a[len - i - 1];
        a[len - i - 1] = temp;
    }
    return a[100];
}

int main() 
{
    char a[100];

    string(a);
    printf("Reversed string: %s\n", a);

    return 0;
}