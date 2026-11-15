#include <Wire.h> 
#include <LiquidCrystal_I2C.h>

// Set the LCD address to 0x27 or 0x3F according to your module
#define I2C_ADDR    0x3F

// Define the LCD size (number of columns and rows)
#define LCD_COLS 16
#define LCD_ROWS 2

// Create an LCD object. Provide the I2C address, number of columns and rows.
LiquidCrystal_I2C lcd(I2C_ADDR, LCD_COLS, LCD_ROWS);

void setup() {
  // Initialize the LCD
  lcd.init();
  
  // Turn on the backlight
  lcd.backlight();
  Serial.print("eihufrioufh:");

  // Print a message to the LCD
  lcd.setCursor(0, 0); // Set cursor to first column, first row
  lcd.print("Hello,");
  lcd.setCursor(0, 1); // Set cursor to first column, second row
  lcd.print("ESP32!");
}

void loop() {
  // Nothing to do here for this simple example
}
