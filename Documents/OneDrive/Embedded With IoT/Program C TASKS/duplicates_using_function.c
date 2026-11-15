#include <stdio.h>
//input array
void function(int *arr, int n) {
    printf("Enter %d elements:\n", n);
    for (int i = 0; i < n; i++) {
        scanf("%d", &arr[i]);
    }
}

// Function to print unique numbers
void printUniqueNumbers(int *arr, int n) {
    int ct;
    printf("The unique numbers are:\n");
    for (int i = 0; i < n; i++) {
        ct = 0;
        for (int j = i + 1; j < n; j++) {
            if (arr[i] == arr[j])
                ct++;
        }
        if (ct == 0) {
            printf("%d\n", arr[i]);
        }
    }
}

int main() {
    int n;
    printf("Enter n: ");
    scanf("%d", &n);

    int a[n];
    printUniqueNumbers(a, n);
    return 0;
}
