#include<stdio.h>
 int main()
{
    int n,i,ct=0,j;
    printf("enter n : ");
    scanf("%d",&n);
    int a[n];
    printf("%d elements are\n",n);

    for(i=0;i<n;i++)
    {
        scanf("%d",&a[i]);
    }
     printf("The numbers are  ");
    for(i=0;i<n;i++){
         ct=0;
        for(j=i+1;j<n;j++)
        {
           if(a[i]==a[j])
           ct++;
        }
        if(ct==0)
        {
        printf("%d\n",a[i]);
        }
    }
}