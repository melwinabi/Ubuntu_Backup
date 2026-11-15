#include <stdio.h>

int main()
{
    
    char a[100]="*a_(4b7i?n)!?a+y25  a  v";
    printf("Entered words are : %s\n",a);
    


    /* Alphabet check */
    for(int i=0;i<100;i++)
    {
     if((a[i] >= 65 && a[i] <= 90) || (a[i] >= 97 && a[i] <= 122))
    //if((a[i] >= 'a' && a[i] <= 'z') || (a[i] >= 'A' && a[i] <= 'Z'))
       
         printf("%c", a[i]);  
    }
   

    return 0;
}