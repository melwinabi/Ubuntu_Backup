#include <stdio.h>
#include <string.h>

char Name[20],da[5],db[5];
char confirmation[4];  // Change from pointer to array of 4 characters
long int Acc, Pass;
int Amount,Deducted_amount,New_amount;

void main()
{
    printf("WELCOME TO ABCD BANK   \n");
    printf("Enter the Account Holder Name: ");
    scanf("%[^\n]s", Name);
    if (strcmp(Name, "Abinaya")  || strcmp(Name, "ABINAYA")  || strcmp(Name, "abinaya") )
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
                printf(" \n");
                printf("15 percent of ur amount is deducted for xy scheme\n");
                printf("So kindly ignore if any delay occurs in ur transaction\n");
                 printf(" \n");
                 printf("Kindly type ok to confirm>>>>");
                 scanf("%s",&da);
                 
                 if (strcmp(da, "ok")==0  || strcmp(da, "Ok")==0 )

                    {
                        printf("Work in progress.Kindly wait\n");
                        printf(" \n");
                    }
                    else 
                    {
                        printf("Cancelling The Processes.........\n ............\n"); 
                        
                        printf("Transaction Cancelled\n");
                    }
                Deducted_amount=((float)15/100*Amount);
               
                printf("The amount deducted is %d\n",Deducted_amount);
                 printf(" \n");
                New_amount =Amount-Deducted_amount;
                printf("New amount is %d \n",New_amount);
                
                
                
                 printf("Kindly type ok to confirm>>>");
                 scanf("%s",&db);
                 
                 if (strcmp(db, "ok")==0  || strcmp(db, "Ok")==0 )

                    {
                        printf("Work in progress.Kindly wait\n");
                        printf(" \n");
                    }
                    else 
                    {
                        printf("Cancelling The Processes.........\n ............\n"); 
                        
                        printf("Transaction Cancelled\n");
                    }
                if (Amount <=100000)
                {
                    printf("................................\n");
                    printf("Check the Details Given Below\n");
                    printf("Account Number      : %ld\n", Acc);
                    printf("Account Holder Name : %s\n", Name);
                    printf("Transaction Amount  : %d\n",New_amount);
                    printf("................................\n");
                    printf("Enter Yes to Confirm > ");
                    scanf("%s", confirmation);
                    if (strcmp(confirmation, "Yes")==0  || strcmp(confirmation, "yes")==0 )

                    {
                        printf("Your Amount Is Transferred Successfully\n");
                    }
                    else 
                    {
                        printf("Cancelling The Processes.........\n ............\n"); 
                        
                        printf("Transaction Cancelled\n");
                    }
                }
                else if (Amount > 100000)
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
    printf("THANK YOU\n");
     printf("HAVE A WONDERFUL DAY !\n");
}