#include <stdio.h>
  

void counter() {
  
    // Static variable retains value between calls
    auto int count=0 ;
    count++;
 
    printf("Count = %d\n", count);
}
int main() {
    
   
      counter();  
      counter(); 
      counter();
      counter();
      counter();  
      counter();
      counter(); 
    return 0;
}

/*
//register storage class

#include <stdio.h>

int main() {
 int a=11;
printf("%d\n",&a);
    register int i;  
    for (i = 0; i < 5; i++) {
        printf("%d \n", i);
    }
    
    return 0;
}*/