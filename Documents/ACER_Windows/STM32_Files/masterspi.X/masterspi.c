#pragma config FOSC = EXTRC     
#pragma config WDTE = OFF       
#pragma config PWRTE = OFF      
#pragma config BOREN = OFF      
#pragma config LVP = OFF      
#pragma config CPD = OFF        
#pragma config WRT = OFF        
#pragma config CP = OFF        

#include <xc.h>
#define _XTAL_FREQ 20000000

void SPI_Init_Master(void) {
    TRISC5 = 0; // MOSI output
    TRISC3 = 0; // SCK output
    TRISC4 = 1; // MISO input
    SSPSTAT = 0x40;   // CKE = 1
    SSPCON = 0x20;    // Master mode, Fosc/4, SSPEN = 1
}

unsigned char SPI_Transfer(unsigned char data) {
    SSPBUF = data;             // Send data
    while (!SSPIF);            // Wait for completion
    SSPIF = 0;                 // Clear interrupt flag
    return SSPBUF;             // Return received data
}

void main(void) {
    unsigned char received;
    
    TRISA5 = 0; // SS pin
    RA5 = 0;    // Enable slave

    TRISB0 = 0; // LED pin
    PORTB = 0x00;

    SPI_Init_Master();

    while(1) {
        received = SPI_Transfer(0x55);  // Send and receive

        if(received == 0xAA) {
            RB0 = 1;
            __delay_ms(300);
            RB0 = 0;
            __delay_ms(300);
        }
    }
}
