#include<stdio.h>
int main()
{  
   int i,j,array[3][3],transpose[3][3];
  
   for(i=0;i<3;i++)
   {
       for(j=0;j<3;j++)
       {
           printf("Enter array[%d][%d] : ",i,j);
           scanf("%d",&array[i][j]);
       }
   }
   
   printf("\n Printing the elements ..\n");
   for(i=0;i<3;i++)
   {
       printf("\n");
       for(j=0;j<3;j++)
       {
           printf("%d\t",array[i][j]);
          
       }
   }
   printf("\n");
    for (int i = 0; i < 3; ++i)
     for (int j = 0; j < 3; ++j) {
     transpose[j][i] = array[i][j];
  }
  printf("\nTranspose of the matrix:\n");
  printf("\n");
  for (int i = 0; i < 3; ++i)
  for (int j = 0; j < 3; ++j) {
    printf("%d  ", transpose[i][j]);
   if (j == 2)
   printf("\n");
  }

 
}  
  


/*#include<stdio.h>
#include<string.h>
int main()
{
    int i;
    char students_name[5][10]={"Abinaya","Abi","remya","Arun"};
    printf("Students name are \n");
    for(i=0;i<4;i++)
    {
        printf("%s\n",students_name[i]);
    }
   
   strcpy(students_name[1], "Pavi");
   printf("Name changes to %s",students_name[1]);
}*/