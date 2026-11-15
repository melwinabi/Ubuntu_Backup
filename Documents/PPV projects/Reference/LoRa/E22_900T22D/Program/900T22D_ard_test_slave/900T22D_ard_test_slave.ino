#include <SoftwareSerial.h>

const byte PIN_RX = 2;     // Arduino listens here (module TXD)
const byte PIN_TX = 3;     // Arduino talks here   (module RXD)

SoftwareSerial lora(PIN_RX, PIN_TX); // RX, TX

void setup() {
  Serial.begin(9600);
  lora.begin(9600);
  Serial.println("E22 receiver ready");
}

void loop() {
  if (lora.available() > 0 ) {
    String msg = lora.readStringUntil('\n'); // read one line
    msg.trim();
    delay(10);
    Serial.print("Received ← ");
    Serial.println(msg);
  }
}
