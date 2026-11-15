#include<stdio.h>
 struct A{
     char x;
     int y;
 };
 
void keyValue(char* x,int* y)
 {
    printf("enter\n");
     scanf(" %c  %d",x,y);
 }
 
 int main()
 {
     //int j;
     struct A a;
     keyValue(&a.x,&a.y);
     printf(" %c and %d",a.x,a.y);
     return 0;
 }