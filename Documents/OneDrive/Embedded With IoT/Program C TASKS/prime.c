#include <stdio.h>
#include<math.h>

int main()	
{
    int a=23,i,val1,val2,count=0;
    val1=ceil(sqrt(a));
    val2=a;
  //  printf("%d",val2);
    for(i=2;i<=val1;i++)
    {
        if(a%i==0)
        count=1;
        
    }
    if(count==0&&val2!=1||val2==2||val2==3)
    {
        printf("%d is a prime number",a);
        
    }
    else
    {
         printf("%d is not a prime number",a);
    }
        
}