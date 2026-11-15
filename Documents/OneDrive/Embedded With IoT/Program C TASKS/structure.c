#include <stdio.h>
struct student {
    char name[10];
    int roll;
    float marks;
} s[3];

int main() {
    int i;
    printf("Enter information of students:\n");
    for (i = 0; i < 3; ++i) {
        s[i].roll = i + 1;
        printf("\nFor roll number %d,\n", s[i].roll);
        printf("Enter name: ");
        scanf("%s", s[i].name);
        printf("Enter marks: ");
        scanf("%f", &s[i].marks);
    }
    printf("\n");
    printf("Displaying Information:\n");
 // displaying information
    for (i = 0; i < 3; ++i) {
        printf("\nRoll number: %d\n", i + 1);
        printf("Name: %s\n",s[i].name);
        printf("Marks: %.2f", s[i].marks);
        printf("\n");
    }
    return 0;
}

/*//structute simple example

#include<stdio.h>
struct A{
    int x;
    char y;
};

int main()
{
    struct A a;
    a.x=11;
    a.y='a';
    printf("%d\n",a.x);
    printf("%c",a.y);
}*/
