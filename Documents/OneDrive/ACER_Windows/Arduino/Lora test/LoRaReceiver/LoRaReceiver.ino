

#include <SPI.h>
#include <LoRa.h>

const int csPin = 10;       // NSS Pin
const int resetPin = 9;     // Reset Pin
const int irqPin = 2;       // IRQ Pin
const int pin = 4;      // LED Pin
const int interval =400;
void setup() {
  Serial.begin(9600);
  Serial.println("LoRa Receiver");

  // Initialize LoRa
  LoRa.setPins(csPin, resetPin, irqPin); // Set CS, Reset, IRQ pins
  if (!LoRa.begin(525E6)) {
    Serial.println("LoRa initialization failed. Check connections.");
    while (true); // Stay here if initialization fails
  }

  // Set LoRa Parameters
  LoRa.setSpreadingFactor(12);
  LoRa.setSignalBandwidth(250E3);
  LoRa.setSyncWord(0xF3);

  pinMode(pin, OUTPUT); // Configure LED pin as output

  Serial.println("LoRa initialization succeeded.");
}

void loop() {
  // Check for incoming packets
  int packetSize = LoRa.parsePacket();
  if (packetSize) {
    Serial.print("Received packet: ");

    // Read packet content
    String receivedData = "";
    while (LoRa.available()) {
      receivedData += (char)LoRa.read(); // Read and append as characters
    }

    Serial.println(receivedData);

    // Compare received data
    if (receivedData == "1011") { // Compare as a string
      Serial.println("Matched: 1011");
      digitalWrite(pin, HIGH);   // Turn LED on
      delay(300);       // Wait for half the interval
      digitalWrite(pin, LOW);    // Turn LED off
      delay(300);
    }

    // Optional: Display RSSI (signal strength)
    Serial.print("RSSI: ");
    Serial.println(LoRa.packetRssi());
  }
  delay(250);
}
