#include<stdio.h>
int main()
{
	int sum=0,i,num;
	printf("Enter the number : ");
	scanf("%d",&num);
	for(i=0;i<=num;i++)
	{
		sum=sum+i;
	}
	printf("The sum of %d is %d",num,sum);
}