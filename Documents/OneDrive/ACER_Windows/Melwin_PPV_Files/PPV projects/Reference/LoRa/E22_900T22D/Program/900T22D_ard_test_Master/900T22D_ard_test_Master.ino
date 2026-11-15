#include <SoftwareSerial.h>

const byte PIN_RX = 2;     // Arduino listens here (module TXD)
const byte PIN_TX = 3;     // Arduino talks here   (module RXD)
const byte PIN_AUX = 4;    // optional

SoftwareSerial lora(PIN_RX, PIN_TX); // RX, TX

unsigned long counter = 0;

void waitForReady() {
  if (digitalRead(PIN_AUX) == HIGH) return;          // already ready
  while (digitalRead(PIN_AUX) == LOW) ;              // wait
}

void setup() {
  pinMode(PIN_AUX, INPUT_PULLUP);   // AUX floats high when ready
  Serial.begin(9600);
  lora.begin(9600);                 // default factory baud
  delay(200);
  Serial.println("E22 transmitter started");
}

void loop() {
  char buffer[300];
  sprintf(buffer, "Hello E22  #%lu\n", millis());
  waitForReady();                   // optional—but polite
  lora.print(buffer);               // send out over the air  
  Serial.print("Sent → "); Serial.print(buffer);
  delay(1000);
}
