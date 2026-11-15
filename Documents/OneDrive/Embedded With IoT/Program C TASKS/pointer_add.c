#include <stdio.h>
void add(int* ptr1, int* ptr2)
{
   int  c = (*ptr1) + (*ptr2) ;
  // return c;
    printf("%d",c);
}

int main() {
  
      // Storing string as pointer
      int a,b,*p1,*p2;
      printf("enter a : ");
      scanf("%d",&a);
      printf("enter b : ");
      scanf("%d",&b);
      p1=&a;
      p2=&b;
    // ptr1=&a , ptr2=&b;
      add(p1,p2);
     
    return 0;
}




/*#include <stdio.h>

int main() {
  
      // Storing string as pointer
      int b=10,*ptr;
      char *s = "Geeks";
      while(*s!='\0')
  {
      printf("address is %d\n",s);
      printf("value is %c\n\n",*s);
      printf("ptr address of 10 is %d\n",ptr);
      printf("ptr value is %d\n",*ptr);
     *s++;
     printf("Thank you\n\n\n");
}

   // printf("%s", s);
    return 0;
}*/