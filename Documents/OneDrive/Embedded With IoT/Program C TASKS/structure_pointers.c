#include <stdio.h>
#include <stdlib.h>

struct student {
    char name[10];
    int roll;
    float marks[3]; // Array to store 3 marks
};

int main() {
    struct student *s,a[3];
    int i, j, n = 3;
    s=a;
   
    printf("Enter information of students:\n");
    for(i = 0; i < n; i++) {
        (s + i)->roll = i + 1;
        printf("\nFor roll number %d,\n", (s + i)->roll);
        printf("Enter name: ");
        scanf("%s", (s + i)->name);

        for (j = 0; j < 3; j++) {  // Corrected marks input
            printf("Enter marks %d: ", j + 1);
            scanf("%f", &(s + i)->marks[j]);
        }
    }

    printf("\nDisplaying Information:\n");
    for (i = 0; i < n; ++i) {
        printf("\nRoll number: %d\n", (s + i)->roll);
        printf("Name: %s\n", (s + i)->name);

        for (j = 0; j < 3; j++) {  // Corrected marks output
            printf("Marks %d : %.2f\n", j + 1, (s + i)->marks[j]);
        }
        printf("\n");
    }

    //free(s);  // Freeing allocated memory
    return 0;
}

/*#include<stdio.h>
struct A{
    int x;
    int y;
};
int main()
{
    struct A a={1,2};
    struct A *ptr=&a;
    printf("%d and %d",a.x,a.y);
   // printf("%d and %d",ptr->x,ptr->y);  //ptr->x = a.x
}
   



#include<stdio.h>
struct A{
    int x;
    float y;
};
struct B {
    char* name;
    struct A a;
};
int main()
{
    struct B z;
    struct B *ptr=&z;
  ptr->name ="abi";
  ptr->a.x=10;
  printf("%s\n", z.name);
  printf("%d\n",ptr->a.x);
    
    
}*/