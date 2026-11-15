#include <SoftwareSerial.h>

// Define SoftwareSerial RX and TX pins
SoftwareSerial mySerial(4, 3); // RX = 3, TX = 4

void setup() {
  Serial.begin(9600);    // Monitor serial
  mySerial.begin(9600);  // SoftwareSerial communication with ESP32
}

void loop() {
  if (mySerial.available()) {
    char data = mySerial.read();
    Serial.print("Received from ESP32: ");
    Serial.println(data);
  }
}
