#include<stdio.h>
int main()
{
    int i,n,position;
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
   
   printf("Original array is : ");
   for(i=0;i<n;i++)
   printf("%d ",a[i]);
   if(position>=n+1)
   printf("Deletion not posible");
   else
    for(i=position-1;i<n-1;i++)
    {
       a[i]= a[i+1];
    }
    printf("\nArray after deletion is : ");
    for(i=0;i<n-1;i++)
    {
        printf("%d ",a[i]);
    }
}