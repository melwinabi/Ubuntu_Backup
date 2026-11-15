#include <stdio.h>
#include<string.h>

int main() {
    char a[10],b[10];
    printf("Enter name 1 : ");
    scanf("%s",&a);
    printf("Enter name 2 : ");
    scanf("%s",&b);
    printf("Size of a is  %d and b is %d\n",strlen(a),strlen(b));
    strcat(a,b);
    printf("%s\n",a);
   strrev(a);
   printf("rev is %s\n",a);
   //int k = strcmp(a,b);
    printf("%d\n",strcmp(a,b));
       return 0;
}