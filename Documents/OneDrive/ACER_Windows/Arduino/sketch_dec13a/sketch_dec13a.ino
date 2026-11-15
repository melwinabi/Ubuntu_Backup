#include <HardwareSerial.h>

// Define custom RX and TX pins
#define RX_PIN 4  // GPIO4 (D2)
#define TX_PIN 2  // GPIO2 (D4)

// Create a HardwareSerial object for UART2
HardwareSerial mySerial(2);

void setup() {
  // Initialize Serial for debugging
  Serial.begin(115200);
  Serial.println("HardwareSerial communication on RX (D2) and TX (D4) started");

  // Initialize UART2 with custom RX and TX pins
  mySerial.begin(115200, SERIAL_8N1, RX_PIN, TX_PIN); // RX -> GPIO4, TX -> GPIO2
}

void loop() {
  // Send data through UART2
  mySerial.println("Hello from TX (D4) and RX (D2)");

  // Check if data is received on UART2
  if (mySerial.available()) {
    char receivedData = mySerial.read();
    Serial.print("Received via UART2: ");
    Serial.println(receivedData);
  }

  delay(1000); // Wait for 1 second
}
