#include <Wire.h>
#include <DS3231M.h>
DS3231M_Class DS3231M;

void setup() {
  // Start serial communication
  Serial.begin(9600);
  
  // Initialize the DS3231M module
  Wire.begin();
  if (!DS3231M.begin()) {
    Serial.println("Couldn't find DS3231M.");
    while (1);
  }
    DS3231M.adjust(DateTime(2024, 10, 22, 14, 30, 0));
  }


void loop() {
  // Get the current date and time
  DateTime now = DS3231M.now();

  // Print the current date and time to the serial monitor
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

  // Wait for a second before updating the time again
  delay(1000);
}
