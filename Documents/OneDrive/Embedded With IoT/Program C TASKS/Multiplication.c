
#include<stdio.h>
int main()
{
	int count,num;
	printf("Enter the multiplication number : ");
	scanf("%d",&num);
	for(count=1;count<=10;count++)
	{
		printf("%d * %d = %d\n",count,num,count*num);
	}
}
