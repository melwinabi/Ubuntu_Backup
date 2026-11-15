#include <stdio.h>

int main() {
         int n ;
    start:
    
        printf("Enter a number : ");
        scanf("%d",&n);

    if (n == 0)
    {
        goto stop;
        printf("You entered: %d\n", n);
    }
    else 
    {
        printf("continue\n");
        goto start;
        
    }
stop:
    printf("Exiting the program because the number is 0\n");
   return 0;
}



/*#include <stdio.h> 
int main() 
{ 
int sum=0,i; 
for(i = 0; i<=100; i++)
{ 
sum = sum+i;
if(i==15)
{ 
goto addition;
 }
 }
addition: 
printf("%d", sum); 
return 0; 
}*/
