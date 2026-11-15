#include<stdio.h>
int main()
{
    int a,b,c,d,e,Avg,Tot;
    printf("Enter the mark of Maths : ");
    scanf("%d",&a);
     printf("Enter the mark of Science : ");
      scanf("%d",&b);
      printf("Enter the mark of Tamil : ");
       scanf("%d",&c);
       printf("Enter the mark of Malayalam : ");
        scanf("%d",&d);
        printf("Enter the mark of English : ");
         scanf("%d",&e);
         Tot=a+b+c+d+e;
         Avg=(a+b+c+d+e)/5;
           printf("The total mark obtained is %d out of 500\n",Tot);
         printf("The average is %d\n",Avg);
         printf("The status of the student is : ");
         if(Tot>=480)
         {
             printf("Excellent");
         }
         else if(Tot>=450)
         {
             printf("Very good");
         }
         else if(Tot>=425)
         {
             printf("Good");
         }
          else if(Tot>=400)
         {
             printf("Try more");
         }
          else if(Tot<400)
         {
             printf("Bad");
         }
         
    
}