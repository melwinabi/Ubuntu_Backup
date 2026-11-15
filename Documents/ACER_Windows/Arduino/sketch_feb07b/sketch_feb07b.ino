#include <SoftwareSerial.h>
SoftwareSerial mySerial(3, 4);  // RX, TX

void setup() {
  Serial.begin(115200);      // Debug monitor
  mySerial.begin(115200);      // Communication with ESP32
}

void loop() {
  if (mySerial.available()) {
    String data = mySerial.readStringUntil('\n');
    Serial.println("Received: " + data);
  }
}
