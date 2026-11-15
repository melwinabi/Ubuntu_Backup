#include <xc.h>
#include <stdio.h>

#define _XTAL_FREQ 20000000

// CONFIGURATION BITS
#pragma config FOSC = HS
#pragma config WDTE = OFF
#pragma config PWRTE = OFF
#pragma config BOREN = OFF
#pragma config LVP = OFF
#pragma config CPD = OFF
#pragma config WRT = OFF
#pragma config CP = OFF

#define DS3232_ADDRESS 0x68

void I2C_Init(void);
void I2C_Start(void);
void I2C_Stop(void);
void I2C_Write(unsigned char);
unsigned char I2C_Read(unsigned char);
void DS3232_SetTime(unsigned char hour, unsigned char min, unsigned char sec);
void DS3232_GetTime(unsigned char* hour, unsigned char* min, unsigned char* sec);
unsigned char Decimal2BCD(unsigned char val);
unsigned char BCD2Decimal(unsigned char bcd);
void uart_send_char(char data);
void uart_send_string(const char *str);
void uart_newline(void);

void I2C_Init(void) {
    SSPCON = 0x28;         // Enable SSP, Master mode
    SSPCON2 = 0x00;
    SSPADD = 49;           // 100kHz for 20MHz Fosc
    SSPSTAT = 0x00;

    TRISC3 = 1;  // SCL as input
    TRISC4 = 1;  // SDA as input
}

void I2C_Start(void) {
    SSPCON2bits.SEN = 1;
    while (SSPCON2bits.SEN);
}

void I2C_Stop(void) {
    SSPCON2bits.PEN = 1;
    while (SSPCON2bits.PEN);
}

void I2C_Write(unsigned char data) {
    SSPBUF = data;
    while (!PIR1bits.SSPIF);
    PIR1bits.SSPIF = 0;
    while (SSPSTATbits.BF);

    if (SSPCONbits.WCOL) {
        SSPCONbits.WCOL = 0; // ? Correct for PIC16F877A
    }

    while (SSPCON2bits.ACKSTAT); // Wait for ACK from slave
}

unsigned char I2C_Read(unsigned char ack) {
    unsigned char data;
    SSPCON2bits.RCEN = 1;
    while (!SSPSTATbits.BF);
    data = SSPBUF;

    SSPCON2bits.ACKDT = (ack == 1) ? 0 : 1; // ACK=0, NACK=1
    SSPCON2bits.ACKEN = 1;
    while (SSPCON2bits.ACKEN);

    return data;
}

unsigned char Decimal2BCD(unsigned char val) {
    return (unsigned char)(((val / 10) << 4) | (val % 10));
}

unsigned char BCD2Decimal(unsigned char bcd) {
    return ((bcd >> 4) * 10) + (bcd & 0x0F);
}

void DS3232_SetTime(unsigned char hour, unsigned char min, unsigned char sec) {
    I2C_Start();
    I2C_Write(DS3232_ADDRESS << 1); // Write mode
    I2C_Write(0x00); // Start from register 0

    I2C_Write(Decimal2BCD(sec) & 0x7F);
    I2C_Write(Decimal2BCD(min));
    I2C_Write(Decimal2BCD(hour));

    I2C_Stop();
}

void DS3232_GetTime(unsigned char* hour, unsigned char* min, unsigned char* sec) {
    I2C_Start();
    I2C_Write(DS3232_ADDRESS << 1); // Write mode
    I2C_Write(0x00); // Start at seconds register

    I2C_Start(); // Repeated start
    I2C_Write((DS3232_ADDRESS << 1) | 1); // Read mode

    *sec  = BCD2Decimal(I2C_Read(1));  // Read seconds, ACK
    *min  = BCD2Decimal(I2C_Read(1));  // Read minutes, ACK
    *hour = BCD2Decimal(I2C_Read(0));  // Read hours, NACK

    I2C_Stop();
}

void uart_send_char(char data) {
    while (!PIR1bits.TXIF);
    TXREG = data;
}

void uart_send_string(const char *str) {
    while (*str != '\0') {
        uart_send_char(*str++);
    }
}

void uart_newline(void) {
    uart_send_string("\r\n");
}

void main(void) {
    // Set UART TX/RX pin directions
    TRISC6 = 0; // TX output
    TRISC7 = 1; // RX input

    // UART init for 9600 baud @ 20MHz
    TXSTA = 0x24;  // TX enable, BRGH=1
    RCSTA = 0x90;  // Serial port enable, continuous RX
    SPBRG = 129;   // Baud rate

    __delay_ms(500); // Wait after power-on

    I2C_Init();

    // OPTIONAL: Set time only once, then comment this out
    DS3232_SetTime(12, 34, 56);

    uart_send_string("I2C RTC Time Read Demo");
    uart_newline();

    unsigned char hour, min, sec;
    char buffer[30];

    while (1) {
        DS3232_GetTime(&hour, &min, &sec);

        sprintf(buffer, "Time: %02u:%02u:%02u", hour, min, sec);
        uart_send_string(buffer);
        uart_newline();

        __delay_ms(1000);
    }
}
