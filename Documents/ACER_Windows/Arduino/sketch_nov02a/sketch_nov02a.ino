#include <Wire.h>
#include <DS3231M.h>
#define DS3231M_SQUARE_WAVE_1HZ 0

DS3231M_Class DS3231M;
const int pulsePin = 2; // Pin connected to INT/SQW of DS3231M

volatile bool pulseReceived = false;

void setup() {
  Serial.begin(9600);
  Wire.begin();
  
  if (!DS3231M.begin()) {
    Serial.println("DS3231M initialization failed!");
    while (1);
  }
  
  // Set square wave output to 1Hz on INT/SQW pin
  // Assuming 1 is for 1Hz mode; replace with the actual mode if different
  DS3231M.pinSquareWave(1); // 1 typically represents 1Hz, but check your library

  // Configure pulsePin as input
  pinMode(pulsePin, INPUT_PULLUP);
  
  // Attach an interrupt to detect the pulse
  attachInterrupt(digitalPinToInterrupt(pulsePin), pulseISR, FALLING);
}

void loop() {
  if (pulseReceived) {
    Serial.println("Pulse received from DS3231M!");
    pulseReceived = false; // Reset the flag for next pulse
  }
}

// Interrupt Service Routine to set pulseReceived flag
void pulseISR() {
  pulseReceived = true;
}
