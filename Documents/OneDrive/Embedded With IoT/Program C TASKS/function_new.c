#include <stdio.h>
#include<string.h>
 

int function(char *a,char *b )
{
   int m;
    printf("Enter number : ");
    scanf("%d",&m);
   return m;
}

int main() {
   int a,b;
    printf(" New num is %d\n",function(a,b));
    return 0;
}