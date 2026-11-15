#include<stdio.h>
int main()
{
    int row =5;
    int i,j,k;
    
      for(i=1;i<=row;i++)
        {
             for(j=1;j<=row-i;j++) 
     
        {
            printf("  ");
        }
        for(k=1;k<=i;k++)
        {
            printf("* ",k);
        }
        
        printf("\n");
    }
    
}