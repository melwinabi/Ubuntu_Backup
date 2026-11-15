#include<stdio.h>
int main()
{
    int a,b,lcm,low;
    printf("Enter a ");
    scanf("%d",&a);
      printf("Enter b ");
    scanf("%d",&b);
    printf("lcm is ");
    low=(a<b)?a:b;
   while(1)
   {
       if(low%a==0&&low%b==0)
       {
           lcm=low;
           break;
       }
       low++;
   }
   printf("%d",lcm);
    }
    