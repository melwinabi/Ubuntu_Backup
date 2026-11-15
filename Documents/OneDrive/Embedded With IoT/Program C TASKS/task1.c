#include <stdio.h>

int main() {
    int n,count,total=0;
while(1)
{
    printf("\nEnter n\n");
    scanf("%d",&n);
    if(n<0)
    {
        printf("Negative factor are not possible \n");
    }
    else
    {
    printf("The factors of %d are\n",n);
    
    for(count=1;count<=n;count++)
    {
        if(n%count==0)
        {
         printf(" %d\n",count);
         total++;
    }
    }
    printf("total factor is %d\n",total);
    if(n==1||n==0)
    {
        printf("Number should be greater than 1 to find prime\n");
    }
   else if(total==2)
    {
        printf("Its prime\n");
        
    }
    else
    {
        printf("Not a prime");
    } 

}
}
 return 0;
}
