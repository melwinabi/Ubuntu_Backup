#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include "RTClib.h"

// Initialize the RTC
RTC_DS3231 rtc;

// Initialize the I2C LCD (20x4, I2C address is 0x27)
LiquidCrystal_I2C lcd(0x27, 20, 4);

void setup() {
  // Start the serial communication
  Serial.begin(9600);
  // Initialize the I2C communication for the RTC module
  Wire.begin();
  
  // Initialize the RTC
  if (!rtc.begin()) {
    Serial.println("Couldn't find RTC");
    while (1);
  }

  // Check if the RTC lost power and only set the time once if necessary
  if (rtc.lostPower()) {
    Serial.println("RTC lost power, setting the time!");
    rtc.adjust(DateTime(2024, 10, 21, 11, 56, 05));   // Set to the time when the sketch was compiled
  }

  // Initialize the LCD
  lcd.init();
  lcd.backlight();  // Turn on the backlight

  // Display a startup message on the LCD
  lcd.setCursor(0, 0);
  lcd.print("RTC Time/Date:");
}

void loop() {
  // Get the current date and time from the RTC
  DateTime now = rtc.now();

  // Print the current date and time to the Serial Monitor
  Serial.print(now.year(), DEC);
  Serial.print('/');
  Serial.print(now.month(), DEC);
  Serial.print('/');
  Serial.print(now.day(), DEC);
  Serial.print(" ");
  Serial.print(now.hour(), DEC);
  Serial.print(':');
  Serial.print(now.minute(), DEC);
  Serial.print(':');
  Serial.print(now.second(), DEC);
  Serial.println();

  // Display the current date on the first line of the LCD
  lcd.setCursor(0, 1);
  lcd.print("Date: ");
  lcd.print(now.year());
  lcd.print('/');
  lcd.print(now.month());
  lcd.print('/');
  lcd.print(now.day());

  // Display the current time on the second line of the LCD
  lcd.setCursor(0, 2);
  lcd.print("Time: ");
  lcd.print(now.hour());
  lcd.print(':');
  lcd.print(now.minute());
  lcd.print(':');
  lcd.print(now.second());

  // Delay 1 second before updating again
  delay(1000);
}
