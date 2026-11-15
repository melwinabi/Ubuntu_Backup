#include<stdio.h>
int main()
{
    int i,j;
    int n[15];
    for(i=1;i<=10;i++)
    {
        n[i]=i*2;
    }
    for(j=1;j<=10;j++)
    {
        printf("Element[%d]=%d\n",j,n[j]);
    }
    printf(" The element %d",n[1]);
}