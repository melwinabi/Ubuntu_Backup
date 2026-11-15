#include <stdio.h>
#include <string.h>

void reverseString(char *a) {
    int i, len;
    char temp;
    char new_temp[100]; 

    strcpy(new_temp, a); 

    len = strlen(a);
    for (i = 0; i < len / 2; i++) {
        temp = a[i];
        a[i] = a[len - i - 1];
        a[len - i - 1] = temp;
    }

    if (strcmp(a, new_temp) == 0) {
        printf("The string is a palindrome.\n");
    } else {
        printf("The string is NOT a palindrome.\n");
    }
}

int main() {
    char a[100];

    printf("Enter a string: ");
    scanf("%[^\n]s", a); 

    reverseString(a);

    printf("Reversed string: %s\n", a);
    return 0;
}
