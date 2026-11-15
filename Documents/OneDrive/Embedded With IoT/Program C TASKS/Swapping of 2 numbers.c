#include<stdio.h>
#include<stdlib.h>


int main() 
{
    int a,b,temp;
    printf("The value of a is ");
    scanf("%d",&a);
    printf("The value of b is ");
    scanf("%d",&b);
    
    temp=a;
    a=b;
    b=temp;
    printf("Now the swapped numbers that is the value of a is %d and b is %d",a,b);
}