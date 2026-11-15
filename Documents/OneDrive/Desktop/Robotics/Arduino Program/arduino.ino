#include <LiquidCrystal.h>

// Initialize the LCD object with the pin connections
LiquidCrystal lcd(13,12,11,10,9,8);

void setup() {
  // Set up the number of columns and rows of the LCD display
  lcd.begin(16, 2);

  // Print a welcome message on the LCD display
  lcd.print("Hello, Arduino!");
}

void loop() {
  // Do nothing in the loop
}
