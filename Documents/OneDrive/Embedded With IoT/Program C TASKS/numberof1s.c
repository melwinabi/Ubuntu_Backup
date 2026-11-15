#include<stdio.h>
int main()
{   int n,x,a,b,s,bin=0,rem=0,place=1;
    printf("Enter no : ");
    scanf("%d",&n);
    while(n)
    {
        rem=n%2;
        n=n/2;
        bin=bin+(rem*place);
        place=place*10;
    }
     int num=bin;
    printf("Binary is %d\n",num);
    a=0;
    b=0;
    s=num;
    while(num>0)
    {
        x=num%10;
        num=num/10;
        if(x==0)
        a++;
        else if(x==1)
        b++;
    }
        printf("The number is %d\n",s);
         printf("The number of 1s are %d\n",b);
          printf("The number of 0s are %d\n",a);
}