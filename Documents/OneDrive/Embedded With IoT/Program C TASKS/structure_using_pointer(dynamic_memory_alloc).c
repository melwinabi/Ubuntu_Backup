#include <stdio.h>
#include <stdlib.h>

struct student {
    char name[10];
    int roll;
    float marks[3]; // Array to store 3 marks
};

int main() {
    struct student *s;
    int i, j, n = 3;

    s = (struct student*) malloc(n * sizeof(struct student));

    if (s == NULL) {
        printf("Memory allocation failed!\n");
        return 1;
    }

    printf("Enter information of students:\n");
    for (i = 0; i < n; i++) {
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

    free(s);  // Freeing allocated memory
    return 0;
}
