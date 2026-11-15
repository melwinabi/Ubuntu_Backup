//integer pointer

#include <stdio.h>
int main()
{
   int* pc, c;
   
   c = 22;
   printf("Address of c: %p\n", &c);
   printf("Value of c: %d\n\n", c);  // 22
   
   pc = &c;
   printf("Address of pointer pc: %p\n", pc);
   printf("Content of pointer pc: %d\n\n", *pc); // 22
   
   c = 11;
   printf("Address of pointer pc: %p\n", pc);
   printf("Content of pointer pc: %d\n\n", *pc); // 11
   
   *pc = 2;
   printf("Address of c: %p\n", &c);
   printf("Value of c: %d\n\n", c); // 2
   return 0;
}

/*// array pointer

#include <stdio.h>

int main()
{
    // declaring some temp variables
    int var1 = 10;
    int var2 = 20;
    int var3 = 30;

    // array of pointers to integers
    int* ptr_arr[3] = { &var1, &var2, &var3 };
    printf("%d\n",*ptr_arr[1]);

    // traversing using loop
    for (int i = 0; i < 3; i++) {
        printf("Value of var%d: %d\tAddress: %p\n", i + 1, *ptr_arr[i], ptr_arr[i]);
    }

    return 0;
}




//function pointer

#include <stdio.h>

// A simple addition function
int add(int a, int b) {
    return a + b;
}

// A simple subtraction function
int subtract(int a, int b) {
    return a - b;
}

void calc(int a, int b, int (*op)(int,int)) {
    printf("%d\n", op(a, b));
}

int main() {
  
    // Passing different functions to 'operate'
    calc(10, 5, add);
  
      calc(10, 5, subtract);

    return 0;
}

/*

/

//double pointer or multi pointer

/*#include <stdio.h>
int main()
{
	int var = 10;

	int* ptr1;
	int** ptr2;
	int*** ptr3;

	ptr1 = &var;

	ptr2 = &ptr1;

	ptr3 = &ptr2;

	printf("Value of variable "
		"var = %d\n",
		var);
	printf("Value of variable var using"
		" pointer ptr1 = %d\n",
		*ptr1);
	printf("Value of variable var using"
		" pointer ptr2 = %d\n",
		**ptr2);
	printf("Value of variable var using"
		" pointer ptr3 = %d\n",
		***ptr3);

	return 0;
}



// C NULL pointer 

#include <stdio.h>

int main()
{
    // declaring null pointer
    int* ptr = NULL;

    // derefencing only if the pointer have any value
    if (ptr == NULL) {
        printf("Pointer does not point to anything");
    }
    else {
        printf("Value pointed by pointer: %d", *ptr);
    }
    return 0;
}



//  wild pointers
#include<stdio.h>
//int main()
{
	/* wild pointer */
	int* p	;
	//int a=10;
	/* Some unknown memory location is being corrupted.
	This should never be done. */
//	p=&a;
	*p = 12;
    
//	printf("%d",a);






//void pointer

/*#include <stdio.h>
int main()
  {
    int a = 10;
    char b = 'x';

    // void pointer holds address of int 'a'
    void * p = &a;
    printf("address of a is %p\n",p);
     printf("value of a is %d\n",*(int*)p);
    // void pointer holds address of char 'b'
    p = &b;
      printf("address of b is %p\n",p);
       printf("value of b is %c",*(char*)p);
    
   }




//constant pointer

#include <stdio.h>

//int main()	
{
    int num1, num2;

    // Constant pointer to num1
    int * const const_ptr = &num1; 

    // Assign 10 to num1 using pointer
    // Modification of value pointed by pointer is allowed
    *const_ptr = 10;

    // Re-assignment of constant pointer
    // Modification of pointer value is not allowed
    const_ptr = &num2; // Error

   // printf("Num1 = %d\n", num1);
    //printf("Num1 = %d\n", *const_ptr);

    return 0;
}



//pointer to constant

#include<stdio.h>
int main ()
{
    int a = 10, b = 20;
    const int *p;
    p = &a;
    printf ("value of a is %d, *p is %d \n", a, *p);
    *p = 5;  //wrong you can not change value at address
    printf ("value of a is %d, *p is %d \n", a, *p);
    p = &b;  //you can change address
    return 0;
}

*/