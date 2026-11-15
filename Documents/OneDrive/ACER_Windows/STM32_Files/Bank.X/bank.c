#pragma config FOSC = EXTRC     
#pragma config WDTE = OFF       
#pragma config PWRTE = OFF      
#pragma config BOREN = OFF      
#pragma config LVP = OFF      
#pragma config CPD = OFF        
#pragma config WRT = OFF        
#pragma config CP = OFF        

#include <xc.h>
#include <string.h>
#include <stdio.h>   
#include <stdlib.h>  

#define _XTAL_FREQ 20000000

void uart_send_char(char data) {
    while (!TXIF);
    TXREG = data;
}

void uart_send_string(const char *str) {
    while (*str != '\0') {
        uart_send_char(*str++);
    }
}

void uart_newline() {
    uart_send_string("\r\n");
}



typedef enum 
{
    STATE_WAIT_COMMAND,
    STATE_ENTER_PIN,
    STATE_ENTER_ACCOUNT,
    STATE_ENTER_MOBILE,
    STATE_ENTER_AMOUNT,
    STATE_COMPLETE,
    STATE_CHECK_BALANCE,
    STATE_CHECK_ACCOUNT
} ATMState;

ATMState state = STATE_WAIT_COMMAND;

char buffer[40];
int index = 0;

// Credentials
const char* correctPin = "100702";
const char* correctAccount = "170802";
const char* correctMobile = "6374886240";

void reset_buffer()
{
    index = 0;
    buffer[0] = '\0';
}

void main(void)
{
    char received;
    int balance = 20000;
    int pinAttempts = 0;

    TXSTA = 0x24;
    RCSTA = 0x90;
    SPBRG = 129;

    uart_send_string("Welcome to Secure Bank ATM\r");
    uart_send_string("Type '1' to withdraw amount\r");
    uart_send_string("Type '2' to check balance: ");

    while (1) {
        if (RCIF)
        {
            received = RCREG;

            if (received == '\r') 
            {
                buffer[index] = '\0';

                switch (state)
                {
                    case STATE_WAIT_COMMAND:
                        if (strcmp(buffer, "1") == 0)
                        {
                            uart_newline();
                            uart_send_string("Enter your PIN to withdraw amount: ");
                            state = STATE_ENTER_PIN;
                        } 
                        else if (strcmp(buffer, "2") == 0)
                        {
                            uart_newline();
                            uart_send_string("Enter your PIN to check balance: ");
                            state = STATE_CHECK_BALANCE;
                        }
                        else 
                        {
                            uart_newline();
                            uart_send_string("Invalid input. Type '1' or '2' to continue: ");
                        }
                        break;

                    case STATE_CHECK_BALANCE:
                        if (strcmp(buffer, correctPin) == 0) {
                            uart_send_string("Enter your account number: ");
                            state = STATE_CHECK_ACCOUNT;
                        }
                        else 
                        {
                            uart_send_string("\rInvalid PIN.\rREMEMBER YOU HAVE ONLY 2 CHANCES\rAFTER 3RD YOUR ACCOUNT WILL BE BLOCKED\r Retype your PIN :");
                        }
                        break;

                    case STATE_CHECK_ACCOUNT:
                        if (strcmp(buffer, correctAccount) == 0)
                        {
                            uart_newline();
                            uart_send_string("PIN verified.\r\nYour balance is: ");
                            char balStr[10];
                            sprintf(balStr, "%d", balance);
                            uart_send_string(balStr);
                            uart_send_string("\r\n");
                            state = STATE_COMPLETE;
                        } 
                        else 
                        {
                            uart_newline();
                            uart_send_string("Incorrect account number. Retype your account number.");
                        }
                        break;

                    case STATE_ENTER_PIN:
                        if (strcmp(buffer, correctPin) == 0)
                        {
                            uart_newline();
                            uart_send_string("PIN verified.\r\nEnter Bank Account Number: ");
                            state = STATE_ENTER_ACCOUNT;
                            pinAttempts = 0;  // Reset attempts after successful login
                        }
                        else
                        {
                            pinAttempts++;
                            if (pinAttempts >= 3)
                            {
                                uart_newline();
                                uart_send_string("Too many incorrect attempts.\r\nYour account has been blocked.\r\n");
                                state = STATE_COMPLETE;
                            }
                            else
                            {
                                char msg[40];  // Reduced to safe size
                                uart_newline();
                                uart_send_string("Invalid PIN\r\nYOU HAVE ONLY 3 ATTEMPTS\r\n");
                                sprintf(msg, "Attempt %d of 3.\r\nRetype your PIN: ", pinAttempts);
                                uart_send_string(msg);
                            }
                        }
                        break;

                    case STATE_ENTER_ACCOUNT:
                        if (strcmp(buffer, correctAccount) == 0)
                        {
                            uart_newline();
                            uart_send_string("Account verified.\r\nEnter Registered Mobile Number: ");
                            state = STATE_ENTER_MOBILE;
                        } 
                        else 
                        {
                            uart_newline();
                            uart_send_string("Invalid account number. Retype your account number ");
                        }
                        break;

                    case STATE_ENTER_MOBILE:
                        if (strcmp(buffer, correctMobile) == 0)
                        {
                            uart_newline();
                            uart_send_string("Mobile verified.\r\nEnter Amount to Withdraw: ");
                            state = STATE_ENTER_AMOUNT;
                        } 
                        else
                        {
                            uart_newline();
                            uart_send_string("Incorrect mobile number. Retype your mobile number ");
                        }
                        break;

                    case STATE_ENTER_AMOUNT: 
                    {
                        uart_newline();

                        // Sanitize input
                        for (int i = 0; buffer[i] != '\0'; i++) {
                            if (buffer[i] < '0' || buffer[i] > '9') {
                                buffer[i] = '\0';
                                break;
                            }
                        }

                        int amount = atoi(buffer);

                        if (amount <= 0)
                        {
                            uart_send_string("Invalid amount entered. Try again.\r\n");
                        } 
                        else if (amount <= balance) 
                        {
                            char amountStr[10];
                            sprintf(amountStr, "%d", amount);
                            uart_send_string("Processing...\r\nPlease collect ");
                            uart_send_string(amountStr);
                            uart_send_string(" from the dispenser.\r\nClick ENTER for new transaction.\r\nThank you!");
                            balance -= amount;
                        }
                        else
                        {
                            uart_send_string("Sorry, insufficient balance to withdraw this amount.\r\nClick ENTER to continue");
                        }

                        state = STATE_COMPLETE;
                        break;
                    }

                    case STATE_COMPLETE:
                        uart_newline();
                        uart_send_string("Type '1' to withdraw amount\r\n");
                        uart_send_string("Type '2' to check balance: ");
                        state = STATE_WAIT_COMMAND;
                        break;
                }

                reset_buffer();
            }
            else
            {
                if (index < sizeof(buffer) - 1)
                {
                    buffer[index++] = received;
                    uart_send_char(received);  // Echo back to user
                }
            }
        }
    }
}