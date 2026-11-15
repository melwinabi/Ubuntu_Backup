#include <Arduino.h>
#include "ESP32SAM.h"

// The speaker is on GPIO 26 for the CYD
const int speakerPin = 26;

ESP32SAM sam;

void setup() {
  Serial.begin(115200);
  
  // No special DAC init needed for SAM library, 
  // but we want to ensure GPIO 26 is ready.
  pinMode(speakerPin, OUTPUT);
  
  Serial.println("Starting Speech...");
}

void loop() {
  // .say() parameters: (Pin, "Text to speak")
  // SAM uses the DAC on pin 26 naturally
  sam.say(speakerPin, "Hello. Welcome.");
  
  delay(5000); // Wait 5 seconds before saying it again
}