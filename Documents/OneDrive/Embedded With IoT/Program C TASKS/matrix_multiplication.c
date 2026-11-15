#include<stdio.h>
int main()
{  
   int i,j,array1[3][3],array2[3][3],k;
    int product[i][j];
  int sum=0;
  
   for(i=0;i<3;i++)
   {
       for(j=0;j<3;j++)
       {
           printf("Enter array1[%d][%d] : ",i,j);
           scanf("%d",&array1[i][j]);
       }
   }
   
   printf("\n Printing the elements of array1 ..\n");
   for(i=0;i<3;i++)
   {
       printf("\n");
       for(j=0;j<3;j++)
       {
           printf("%d\t",array1[i][j]);
          
       }
   }
   printf("\n");
   printf("\n");
    for(i=0;i<3;i++)
   {
       for(j=0;j<3;j++)
       {
           printf("Enter array2[%d][%d] : ",i,j);
           scanf("%d",&array2[i][j]);
       }
   }
   printf("\n");
    printf("\n Printing the elements of array2 ..\n");
   for(i=0;i<3;i++)
   {
       printf("\n");
       for(j=0;j<3;j++)
       {
           printf("%d\t",array2[i][j]);
          
       }
   }
    
 
  for(i=0;i<3;i++)
  {
      for (j=0;j<3;j++)
      {
          for(k=0;k<3;k++)
          {
              sum+=array1[i][k]*array2[k][j];
          }
          product[i][j]=sum;
          sum=0;
          
      }
  }
  printf("\n");
   printf("\n");
  printf("Resultant matrix\n ");
  for(i=0;i<3;i++)
  {
      for(j=0;j<3;j++)
      {
          printf("%d ",product[i][j]);
      }
      printf("\n");
  }
  return 0;
 
 
}  
  


