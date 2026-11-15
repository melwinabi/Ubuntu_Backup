#include<stdio.h>
#include<string.h>
int main()
{
    char new_temp[10];
    char a[10]="abinaya";
    strcpy(new_temp,a);
    strrev(a);
    //printf("%s\n",a);
    if(strcmp(new_temp,a)==0)
    {
        printf("Palindrome");

    }
    else{
        printf("Not palindrome");
    }
}