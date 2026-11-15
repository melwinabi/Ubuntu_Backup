#include <stdio.h>
#include <string.h>


char Name[20];
char confermation[4];  // Change from pointer to array of 4 characters
long int Acc, Pass;
int Amount;

void main()
{
    printf("Enter the Account Holder Name: ");
    scanf("%s", Name);
    if (strcmp(Name, "melwin") == 0 || strcmp(Name, "MELWIN") == 0 || strcmp(Name, "Melwin") == 0)
    {
        printf("Enter the Account Number: ");
        scanf("%d", &Acc);
        if (Acc == 123456789)
        {
            printf("Enter the PIN Number: ");
            scanf("%d", &Pass); 
            if (Pass == 100702)
            {
                printf("Enter the Amount: ");
                scanf("%d", &Amount);     
                if (Amount < 100)
                {
                    printf("................................\n");
                    printf("Check the Details Given Below\n");
                    printf("Account Number      : %ld\n", Acc);
                    printf("Account Holder Name : %s\n", Name);
                    printf("Transaction Amount  : %d\n", Amount);
                    printf("................................\n");
                    printf("Enter Yes to Confirm > ");
                    scanf("%s", confermation);
                    if (strcmp(confermation, "Yes") == 0 || strcmp(confermation, "yes") == 0 || strcmp(confermation, "Yes") == 0 )

                    {
                        printf("Your Amount Is Transferred Successfully\n");
                    }
                    else
                    {
                        printf("Cancelling The Processes.........\n ............\n"); 
                        
                        printf("Transaction Cancelled\n");
                    }
                }
                else if (Amount > 1000)
                {
                    printf("Your Amount Is Too High, Contact The Nearest Branch\n");   
                }
            }
            else
            {
                printf("You Entered the Wrong PIN!\n");     
            }
        }
        else 
        {
            printf("You Entered the Wrong Account Number!\n");
        }
    }
    else 
    {
        printf("Wrong Name Entered!\n");
    }
}
