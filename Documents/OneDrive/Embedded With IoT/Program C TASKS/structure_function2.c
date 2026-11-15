#include<stdio.h>
#include<string.h>

struct A{
    char gender[10];
    int height;
};

 void keyValue(struct A *a)
 {
     printf("Enter gender : ");
     scanf("%s",a->gender);
      printf("Enter height : ");
     scanf("%d",&a->height);
    if (strcmp(a->gender, "Female") == 0 || (strcmp(a->gender,"female")==0))
     {
     if(a->height<=150)
     {
     printf("Not Eligible for selection");
     }
     else
     {
     printf("Eligible for selection");
     }
     }
     
     if (strcmp(a->gender, "Male") == 0|| (strcmp(a->gender,"male")==0))
     {
     if(a->height<=170)
     {
     printf("Not Eligible for selection");
     }
     else
     {
     printf("Eligible for selection");
     }
     }
 }
 int main()
 {
     
     struct A a;
     keyValue(&a);
 }