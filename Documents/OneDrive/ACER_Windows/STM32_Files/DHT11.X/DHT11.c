
#pragma config FOSC = HS        // External High-Speed Crystal
#pragma config WDTE = OFF       // Watchdog Timer disabled
#pragma config PWRTE = ON       // Power-up Timer enabled
#pragma config BOREN = ON       // Brown-out Reset enabled
#pragma config LVP = OFF        // Low-Voltage Programming disabled

#include <xc.h>
#include <stdint.h>

#define _XTAL_FREQ 20000000     // 20MHz crystal
#define DHT11_PIN RB6          // DHT11 connected to RB0
#define DHT11_PIN_DIR TRISB6  
// Direction control for RB0

// Function prototypes
void initialize(void);
uint8_t readDHT11(uint8_t* humidity, uint8_t* temperature);
void startSignal(void);
uint8_t checkResponse(void);
uint8_t readByte(void);


//LCD
void lcd_enable();
void lcd_send_nibble(unsigned char nibble);
void lcd_command(unsigned char cmd);
void lcd_data(unsigned char data);
void lcd_init();
void lcd_print(const char *str);

void main(void) {
    
    {
    TRISB = 0x00; 
    TRISC = 0x00;  
    
    PORTB = 0x00;
    PORTC = 0x00;

    lcd_init();         
    lcd_print("Temp: ");  
    lcd_command(0xC0);
    lcd_print("Humidity: : "); 
    


    while(1) {}        
}

    uint8_t humidity, temperature, checksum;
    
    initialize();
    
    while(1) {
        // Read data from DHT11
        if(readDHT11(&humidity, &temperature)) {
            // Successfully read data
            // Add code to display or process the data
        } else {
            // Read failed
            // Add error handling code
        }
        
        __delay_ms(2000);  // Wait 2 seconds before next reading
    }
    
    return;
}

void initialize(void) {
    // Initialize ports and peripherals
    TRISB = 0;  // Set PORTB as output initially
    
    // Add any additional initialization code here
}

uint8_t readDHT11(uint8_t* humidity, uint8_t* temperature) {
    uint8_t data[5] = {0, 0, 0, 0, 0};
    uint8_t i, j;
    
    startSignal();
    
    if(checkResponse()) {
        // Read 40 bits (5 bytes) from DHT11
        for(i = 0; i < 5; i++) {
            data[i] = readByte();
        }
        
        // Verify checksum
        if((data[0] + data[1] + data[2] + data[3]) == data[4]) {
            *humidity = data[0];      // Integral humidity value
            *temperature = data[2];   // Integral temperature value
            return 1;  // Success
        }
    }
    
    return 0;  // Failure
}

void startSignal(void) {
    // Send start signal to DHT11
    DHT11_PIN_DIR = 0;     // Set as output
    DHT11_PIN = 0;         // Pull low
    __delay_ms(18);        // Hold low for at least 18ms
    DHT11_PIN = 1;         // Release the bus
    __delay_us(40);        // Wait for DHT11 to respond
    DHT11_PIN_DIR = 1;     // Set as input
}

uint8_t checkResponse(void) {
    // Check for DHT11 response
    // Add your code here
    
    return 1;  // Return 1 if response is detected, 0 otherwise
}

uint8_t readByte(void) {
    // Read 8 bits from DHT11
    // Add your code here
    
    return 0;  // Replace with actual read value
}



void lcd_init()
{
    __delay_ms(20);       //after power up delay   

    lcd_send_nibble(0x03);  //function calls  -wake up lcd to receive commands  
    lcd_enable();
    __delay_ms(5);
    lcd_enable();   //function call
    __delay_us(100);
    lcd_enable();

    lcd_send_nibble(0x02); //set to 4 bit   
    lcd_enable();
//function call
    lcd_command(0x28);     //4 bit 2 line 5x8     
    lcd_command(0x0C);     //display on  
    lcd_command(0x06);      //move cursor to right  
    lcd_command(0x01);      //clear and return to home  
    __delay_ms(2);
}
//as only 4 bit works
void lcd_command(unsigned char cmd)
{
    RB3 = 0;     
    RB1 = 0;     

    lcd_send_nibble(cmd >> 4); 
    lcd_enable();
    lcd_send_nibble(cmd & 0x0F);
    lcd_enable();
}

//to send data
void lcd_data(unsigned char data)
{
    RB3 = 1;   //data   
    RB1 = 0;    //write  

    lcd_send_nibble(data >> 4);  //first send upper nibble
    lcd_enable();
    lcd_send_nibble(data & 0x0F);  //second send remaning bit(lower nibble))
    lcd_enable();
}


void lcd_send_nibble(unsigned char nibble)
{
    PORTC &= 0x0F;     //unchanged upper nibble     
    PORTC |= (nibble << 4); //shifting
}

void lcd_enable()
{
    RB2 = 1;
    __delay_us(1);
    RB2 = 0;
    __delay_us(100);
}
void lcd_print(const char *str)
{
    while(*str != '\0')   // Loop until end of string
    {
        lcd_data(*str);   // Send each character
        str++;            // Move to next character
    }
}