//function

#include<stdio.h>
int add(int a, int b)
{
    int c=a+b;
    return c;
}
int main()
{ 
    int a,b;
    printf("Enter 2 numbers : ");
    scanf("%d %d",&a,&b);
    printf("%d",add(a,b));
    
}


/*//no arguement with no return value

#include<stdio.h>
void add();
 
int main()
{
   
    add();
    //return 0;
}

void add()
{
    int a,b,c;
    printf("\nEnter The Value of A & B :");
    scanf("%d%d",&a,&b);
    c=a+b;
    printf("\nTotal : %d",c);
}




// without argument with  return value

#include<stdio.h>
int add()
{
    int a=8,b=5;
   int c= a+b;
   return c;
   
}
int main()
{
   printf("%d",add());
   
}




//with arguements with no return value

#include<stdio.h>
void add(int a,int b);
 
int main()
{
    int a,b;
    printf("\nEnter The Value of A & B : ");
    scanf("%d%d",&a,&b);
    
    add(a,b);
    return 0;
}

void add(int x,int y)  
{
    int c;
    c=x+y;
    printf("\nTotal : %d",c);
}
 
 
 
 
 //with arguement and return value

#include<stdio.h>
int add(int,int);

int main()
{
    int a,b;
    printf("\nEnter The Value of A & B : ");
    scanf("%d%d",&a,&b);
    a=add(a,b);
    printf("\nTotal : %d",a);
    return 0;
}
 
int add(int x,int y)
{
    return x+y;
}
 
 
 
 
 //pass by value
#include <stdio.h>

void swapx(int a, int b) {
    int temp;
    temp = a;
    a = b;
    b = temp;

    printf("Inside swapx: a = %d b = %d\n", a, b);
}

// Main function
int main() {
    int a = 10, b = 20;

    // Pass by Values
    swapx(a, b);
    printf("Inside main: a = %d b = %d", a, b);
    return 0;
}




//pass by reference

#include <stdio.h>

void swapx(int* a, int* b) {
    int temp;
     temp = *a;
    *a = *b;
    *b = temp;
    printf("Inside swapx: a = %d b = %d\n", *a, *b);
}

int main() {
    int a = 10, b = 20;

    // Pass by reference
    swapx(&a, &b); 

    printf("Inside main: a = %d b = %d", a, b);
    return 0;
}*/
 