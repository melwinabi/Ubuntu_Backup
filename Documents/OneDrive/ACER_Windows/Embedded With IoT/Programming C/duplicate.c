

#include<stdio.h>
int main()
{
    int n, i, j, ct=0;
    
    printf("enter n : ");
    scanf("%d", &n);

    int a[n];  // Declare array after getting 'n'

    printf("elements are\n");

    for(i = 0; i < n; i++)
    {
        scanf("%d", &a[i]);  // Removed \n from scanf
    }

    printf("The numbers are\n");

    for(i = 0; i < n; i++)
    {
        ct = 0;
        for(j = i + 1; j < n; j++)
        {
            if(a[i] == a[j])
            {
                ct++;
            }
        }
        
        if(ct == 0)
        {
            printf("%d\n", a[i]);
        }
    }

    return 0;
}
