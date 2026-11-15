#include<stdio.h>

int main()
{
 char*String[20];
 printf("Enter Your Name: ");
 scanf("%[^\n]s",String);
 printf(String); 
}