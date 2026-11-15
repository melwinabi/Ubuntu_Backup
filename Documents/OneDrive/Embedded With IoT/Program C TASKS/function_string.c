#include <stdio.h>
#include<string.h>
 

char* function(char *a,char *b , int k)
{
   static char m[15];
    printf("Enter new string : ");
    scanf("%s",m);
   printf("%d     %d  %d\n",a,b,&k);
   printf("%d     %d %d\n",a,b,&k);
   strcpy(b,"world");
   return m;

    

}

int main() {
   char a[]="Hi" ;
    char  b[]="hello" ;
    int k=25;
    char m[15];
    char* ptr = function(a,b,k);
    
    printf(" New string is %s\n",ptr);

     printf("%d    %d  %d\n",a,b,&k);
    
   
//   printf("%s",ptr);
   
    return 0;
}