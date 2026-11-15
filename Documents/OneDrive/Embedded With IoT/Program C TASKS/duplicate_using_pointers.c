#include <stdio.h>

int main()
{
    int n, i, j, ct = 0;
    printf("Enter n: ");
    scanf("%d", &n);
    int a[n]; 
    int *ptr = a; 
    printf("%d elements are:\n", n);
    for (i = 0; i < n; i++)
    {   scanf("%d", (ptr + i));
    }
    printf("The unique numbers are:\n");
    for (i = 0; i < n; i++)
    {
        ct = 0;
        for (j = i + 1; j < n; j++)
        {  if (*(ptr + i) == *(ptr + j))
                ct++;
        }
        if (ct == 0)
        { printf("%d\n", *(ptr + i));
        }
    }

    return 0;
}
