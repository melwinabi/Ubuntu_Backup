#include<stdio.h>
#include<stdlib.h>
int main()
{
    int n,*ptr;
    printf("Enter number of elements: ");    
    scanf("%d",&n);  
    ptr = (int*)malloc(n * sizeof(int));
  
    if (ptr == NULL) {
        printf("Memory not allocated.\n");
    }
    
    printf("Enter the elements \n");
    for(int i=0;i<n;i++)
    {
    scanf("%d",ptr+i);
    }
    printf("Array and adress is \n");
    printf("%d   %p\n",*ptr,ptr);
    printf("%d   %p\n",*(ptr+1),(ptr+1));
    free(ptr);
   // ptr = NULL;
    printf("\n%p ",ptr);
    ptr = (int*)realloc(ptr,10 * sizeof(int));
    
     printf("Enter the elements \n");
    for(int i=0;i<10;i++)
    {
    scanf("%d",ptr+i);
    }
    printf("Array and adress is \n");
    printf("%d   %p\n",*ptr,ptr); printf("%d   %p\n",*(ptr+1),(ptr+1));
}
    
    
    
    
    
    
