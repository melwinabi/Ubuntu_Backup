#include<stdio.h>
#include<string.h>

struct A{
    char gender[10];
    int height;
};
  
 void function(struct A *a)
 {
    
    //for female
   if (strcmp(a->gender, "Female") == 0 || strcmp(a->gender, "female") == 0 || strcmp(a->gender, "FEMALE") == 0) 

   {
     if(a->height<=150)
     {
     printf("Not Eligible for selection\n");
     }
     else
     {
     printf("Eligible for selection\n");
     }
     }
     
     
     //for male
      if (strcmp(a->gender, "Male") == 0 || strcmp(a->gender, "male") == 0 || strcmp(a->gender, "MALE") == 0)
     {
     if(a->height<=170)
     {
     printf("Not Eligible for selection\n");
     }
     else
     {
     printf("Eligible for selection\n");
     }
     }
 }
 
 
 int main()
 {
     
     struct A a;
     for(int i=0;i<100;i++)
     {
     printf("\nEnter gender : ");
     scanf("%s",a.gender);
     printf("Enter height : ");
     scanf("%d",&a.height);
     function(&a);
     }
 }