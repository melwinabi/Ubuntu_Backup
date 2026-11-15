#include <stdio.h>

int main()
{
    char character;

    printf("Enter any character: ");
    scanf("%c", &character);


    /* Alphabet check */
    if((character >= 65 && character <= 90) || (character >= 97 && character <= 122))
    {
        printf("%c is alphabet.", character);
    }
    else if(character >= 48 && character <= 57)
    {
        printf("%c is digit.", character);
    }
    else 
    {
        printf("%c is special character.", character);
    }

    return 0;
}