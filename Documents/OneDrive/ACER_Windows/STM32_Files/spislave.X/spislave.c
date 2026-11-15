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

void SPI_Init_Slave(void) {
    TRISC5 = 1; // MOSI input
    TRISC4 = 0; // MISO output
    TRISC3 = 1; // SCK input
    TRISA5 = 1; // SS input

    SSPSTAT = 0x40;   // CKE = 1
    SSPCON = 0x24;    // Slave mode, SS enabled, SSPEN = 1
    SSPIF = 0;        // Clear flag
}

unsigned char SPI_Transfer_Slave(void) {
    while (!SSPIF);            // Wait for data
    SSPIF = 0;                 // Clear interrupt flag
    return SSPBUF;             // Read received data
}

void main(void) {
    unsigned char received;

    TRISB0 = 0; // LED
    PORTB = 0x00;

    SPI_Init_Slave();

    while(1) {
        received = SPI_Transfer_Slave();

        if(received == 0x55) {
            SSPBUF = 0xAA;  // Respond in next transfer
            RB0 = 1;
            __delay_ms(200);
            RB0 = 0;
        }
    }
}
