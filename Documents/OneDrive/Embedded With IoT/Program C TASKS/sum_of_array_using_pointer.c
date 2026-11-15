#include <stdio.h>

int main()
{
   int n,i,*ptr,sum=0;
    printf("Enter size of array : ");
    scanf("%d",&n);
    int a[i];
    printf("Enter %d numbers : ",n);
    for(i=0;i<n;i++)
    {
    scanf("%d",&a[i]);
    }
    printf("array a[%d] is \n",n);
    for(i=0;i<n;i++)
    {
    printf("a[%d]=%d\n",i,a[i]);
    }
    ptr=a;
    for(i=0;i<n;i++)
    {
        sum=sum+*ptr;
        ptr++;
    }
    printf("sum of array is %d",sum);
}

/*
#include <stdio.h>
int add(int a[])
{
    int i,n,sum=0;
    for(i=0;i<n;i++)
    {
    int sum=sum+a[i];
 
    }
   
    return sum;
}
    


int main()
{
   int n,i,sum=0;
    printf("Enter size of array : ");
    scanf("%d",&n);
    int a[i];
    printf("Enter %d numbers : ",n);
    for(i=0;i<n;i++)
    {
    scanf("%d",&a[i]);
    }
    printf("array a[%d] is \n",n);
    for(i=0;i<n;i++)
    {
    printf("a[%d]=%d\n",i,a[i]);
    }
   add(a);
    printf("sum of array is %d\n",add(a));
}
*/