
#include <stdio.h>

int main() {
   int num=25,p;
   printf("Enter position : ");
   scanf("%d",&p);
   num=num|(1<<p);
   printf("After operation is %d\n",num);
   if((num>>5)&1==1)
   {
   printf("Its 1");
   }
   else
   {
   printf("Its 0");
   
}
    return 0;
}