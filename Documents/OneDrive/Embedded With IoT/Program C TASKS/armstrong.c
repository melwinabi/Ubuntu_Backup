#include <stdio.h>
int main()	
{
    int a=371,result=0,count=0,mult=1,rem;
    int q=a;
    
    while(q!=0)
    {
        q=q/10;
        count++;
    }
    int new_count=count;
   
    q=a;
    while(q!=0)
    {
        rem=q%10;
        while(new_count!=0)
        {
            mult=mult*rem;
            new_count--;
        }
        result=result+mult;
        new_count=count;
       q=q/10;
        mult=1;
    }
  
    if(result==a)
    {
        printf("%d is a armstrong number ",a);
    }
    else
    {
        printf("%d is not a armstrong number ",a);
    }
}