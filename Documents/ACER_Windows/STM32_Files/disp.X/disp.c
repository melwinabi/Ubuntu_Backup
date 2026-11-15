#pragma config FOSC = HS        
#pragma config WDTE = OFF       
#pragma config PWRTE = ON       
#pragma config BOREN = ON       
#pragma config LVP = OFF        

#include <xc.h>
#include <stdint.h>
#include <stdio.h>

#define _XTAL_FREQ 20000000     

#define DHT11_PIN RB0
#define DHT11_PIN_DIR TRISB0

// LCD function declarations
void lcd_enable();
void lcd_send_nibble(unsigned char nibble);
void lcd_command(unsigned char cmd);
void lcd_data(unsigned char data);
void lcd_init();
void lcd_print(const char *str);
void lcd_goto(unsigned char col, unsigned char row);
void lcd_print_number(uint8_t num);

// DHT11 function declarations
void startSignal(void);
uint8_t checkResponse(void);
uint8_t readByte(void);
uint8_t readDHT11(uint8_t* humidity, uint8_t* temperature);

void main(void) {
    uint8_t humidity = 0, temperature = 0;

    TRISB = 0x00;   // LCD + DHT11
    TRISC = 0x00;   // LCD data lines
    PORTB = 0x00;
    PORTC = 0x00;

    lcd_init();
    lcd_command(0x80);
    lcd_print("Temp: ");
    lcd_command(0xC0);
    lcd_print("Humidity: ");

    while (1) 
    {
        __delay_ms(100); // spacing delay before request

        if (readDHT11(&humidity, &temperature)) {
            // Print temperaturez
            lcd_command(0x86); 
 
            lcd_print_number(temperature);
            lcd_data(0xDF); // ° symbol
            lcd_data('C');

            // Print humidity
           lcd_command(0xC9);

            lcd_print_number(humidity);
            lcd_data('%');
        } else
        {
           lcd_command(0x86);

            lcd_print("Err   ");
             lcd_command(0xC9);

            lcd_print("      ");
        }

        __delay_ms(2000);
    }
}

void startSignal(void) 
{
    DHT11_PIN_DIR = 0; // Output
    DHT11_PIN = 0;
    __delay_ms(18);
    DHT11_PIN = 1;
    __delay_us(30);
    DHT11_PIN_DIR = 1; // Input
}

uint8_t checkResponse(void) {
    uint16_t wait = 0;

    while (DHT11_PIN && wait++ < 200) __delay_us(1);
    
    if (wait >= 200) return 0;

    wait = 0;
    
    while (!DHT11_PIN && wait++ < 200) __delay_us(1);
    if (wait >= 200) return 0;

    wait = 0;
    while (DHT11_PIN && wait++ < 200) __delay_us(1);
    if (wait >= 200) return 0;

    return 1;
}

uint8_t readByte(void) {
    uint8_t i, result = 0;

    for (i = 0; i < 8; i++) {
        while (!DHT11_PIN);         // Wait for HIGH
        __delay_us(40);             // 40us sample point

        if (DHT11_PIN)
            result |= (1 << (7 - i));

        while (DHT11_PIN);          // Wait for LOW
    }
    return result;
}

uint8_t readDHT11(uint8_t* humidity, uint8_t* temperature) {
    uint8_t data[5];

    startSignal();
    if (!checkResponse())
        return 0;

    for (int i = 0; i < 5; i++)
        data[i] = readByte();// = result > 

    if ((data[0] + data[1] + data[2] + data[3]) == data[4]) {
        *humidity = data[0];
        *temperature = data[2];
        return 1;
    }
    return 0;
}

// ---------------- LCD Functions ----------------
void lcd_init() {
    __delay_ms(20);       
    lcd_send_nibble(0x03); lcd_enable(); __delay_ms(5);
    lcd_send_nibble(0x03); lcd_enable(); __delay_us(100);
    lcd_send_nibble(0x03); lcd_enable();
    lcd_send_nibble(0x02); lcd_enable(); // 4-bit mode

    lcd_command(0x28);  // 4-bit, 2-line, 5x8 font
    lcd_command(0x0C);  // Display ON, Cursor OFF
    lcd_command(0x06);  // Increment cursor
    lcd_command(0x01);  // Clear screen
    __delay_ms(2);
}

void lcd_command(unsigned char cmd) {
    RB3 = 0; // RS = 0
    RB1 = 0; // RW = 0
    lcd_send_nibble(cmd >> 4);
    lcd_enable();
    lcd_send_nibble(cmd & 0x0F);
    lcd_enable(); 
}

void lcd_data(unsigned char data) {
    RB3 = 1; // RS = 1
    RB1 = 0; // RW = 0
    lcd_send_nibble(data >> 4);
    lcd_enable();
    lcd_send_nibble(data & 0x0F);
    lcd_enable();
}

void lcd_send_nibble(unsigned char nibble) {
    PORTC &= 0x0F;
    PORTC |= (nibble << 4);
}

void lcd_enable() {
    RB2 = 1;
    __delay_us(1);
    RB2 = 0;
    __delay_us(100);
}

void lcd_print(const char *str) {
    while (*str) {
        lcd_data(*str++);
    }
}



void lcd_print_number(uint8_t num) {
    char buffer[5];
    sprintf(buffer, "%u", num);
    lcd_print(buffer);
}