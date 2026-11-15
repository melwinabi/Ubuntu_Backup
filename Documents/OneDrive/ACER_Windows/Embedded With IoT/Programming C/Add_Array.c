#include <stdio.h>


int main() {
    int n;
    printf("Enter size of array: ");
    scanf("%d", &n);
    
    int a[n];  
    
    for (int i = 0; i < n; i++) {  
        printf("Enter the %dth Number: ", i);
        scanf("%d", &a[i]);
    }
    
    operate(a, n);  

    return 0;
}

void operate(int add[], int lng) {
    int sum = 0;
    for (int i = 0; i < lng; i++) {
        sum += add[i];
    }
    printf("Sum of array elements: %d\n", sum);
}


//printf("The 2nd place number is: %d\n", a[1]);