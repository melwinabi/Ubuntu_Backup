#include <stdio.h>

int main() {
    int a = 121,q,result=0,remainder;
    q=a;
    while( q!=0)
    {   
       
        remainder=q%10;
        result=result*10+remainder;
        q=q/10;
        
    }
   
   if(result==a)
   {
       printf("%d a palindrome",a);
   }
   else
   {
       printf("%d not a palindrome",a);
   }
    return 0;
}