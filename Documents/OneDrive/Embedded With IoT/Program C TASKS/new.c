#include<stdio.h>
#include<string.h>
int main()
{
    char a[10],b[10];
    printf("Enter string 1 : " );
    scanf("%s",&a);
     printf("Enter string 2 : " );
    scanf("%s",&b);
     printf("len of a if %d\n",strlen(a));
     printf("len of b if %d\n",strlen(b));
   
    printf("join is %s\n",strcat(a,b));
    printf("len of join if %d\n",strlen(a));
    
}