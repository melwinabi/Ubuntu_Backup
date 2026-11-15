#include <SoftwareSerial.h>

// Define SoftwareSerial pins for E22
SoftwareSerial LoRaSerial(2, 3); // RX (to TXD of E22), TX (to RXD of E22)

// Define mode control pins
#define M0 4
#define M1 5
#define AUX 6

void setup() {
  // Set pin modes
  pinMode(M0, OUTPUT);
  pinMode(M1, OUTPUT);
  pinMode(AUX, INPUT);
  digitalWrite(M0, LOW);
  digitalWrite(M1, LOW);

  // Start serial communication
  Serial.begin(9600);       // Debugging
  LoRaSerial.begin(9600);   // LoRa module communication
  Serial.println("Receiver Ready...");
}

void loop() {
  // Check if data is available to read
  if (LoRaSerial.available()) {
    String receivedMessage = "";

    // Read the incoming data
    while (LoRaSerial.available()) {
      char c = LoRaSerial.read();
      receivedMessage += c;
    }

    // Print received message to the Serial Monitor
    Serial.println("Received: " + receivedMessage);

    // Send acknowledgment back to the sender
    String acknowledgment = "ACK" + receivedMessage;
    LoRaSerial.print(acknowledgment); // Send "ACK"
    Serial.println("Acknowledgment sent: " + acknowledgment);
  }

  delay(100); // Short delay to prevent serial buffer overflow
}
