#include<stdio.h>
int main()
{
    int i,n,position,value;
   printf("Enter n : ");
   scanf("%d",&n);
    int a[n];
    printf("Enter array : ");
    
    for(i=0;i<n;i++)
    {
   scanf("%d",&a[i]);
    }
   
    printf("Enter position : ");
   scanf("%d",&position);
   
   printf("Enter value : ");
   scanf("%d",&value);
   printf("Original array is : ");
   for(i=0;i<n;i++)
   printf("%d ",a[i]);
   
    for(i=n-1;i>=position-1;i--)
    {
        a[i+1]=a[i];
    }
    a[position-1]=value;
    printf("\nArray after insertion is : ");
    for(i=0;i<=5;i++)
    {
        printf("%d ",a[i]);
    }
}