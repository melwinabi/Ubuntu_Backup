#include <SoftwareSerial.h>

// Define SoftwareSerial pins
SoftwareSerial softSerial(18, 19); // RX (pin 18) and TX (pin 19)

void setup() {
  // Initialize the software serial port
  softSerial.begin(115200);   // Communication with external device
  Serial.begin(115200);       // Debugging with Serial Monitor

  Serial.println("Software Serial Initialized");
}

void loop() {
  // Check if data is available on SoftwareSerial
  if (softSerial.available()) {
    String receivedData = "";

    // Read the incoming data
    while (softSerial.available()) {
      char c = softSerial.read(); // Read a character
      receivedData += c;          // Append it to the string
    }

    // Print received data to Serial Monitor
    Serial.println("Received: " + receivedData);

    // Optionally, send an acknowledgment back to the sender
    softSerial.println("Data received: " + receivedData);
  }

  delay(100); // Small delay to avoid flooding the Serial Monitor
}
