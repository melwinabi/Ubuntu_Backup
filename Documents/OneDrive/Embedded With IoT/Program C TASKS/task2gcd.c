#include<stdio.h>
int main()
{
    int a,b,gcd,i;
    printf("Enter a ");
    scanf("%d",&a);
      printf("Enter b ");
    scanf("%d",&b);
    printf("Gcd is ");
    for(i=1;i<=a&&i<=b;i++)
    {
        if(a%i==0&&b%i==0)
        {
            gcd=i;
        }
        
}
printf("%d",gcd);
    }
    