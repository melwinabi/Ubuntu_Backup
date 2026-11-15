#include <SPI.h>
#include <LoRa.h>

const int csPin = 10;       // NSS Pin
const int resetPin = 9;     // Reset Pin
const int irqPin = 2;       // IRQ Pin

String nodeid = "NODE264";  // Node Identifier
long lastSendTime = 0;      // Timestamp for the last packet sent
int interval = 2000;        // Interval between packets (milliseconds)
int counter = 0;            // Packet counter

void setup() {
  Serial.begin(9600);
  Serial.println("LoRa Transmitter");

  // Initialize LoRa
  LoRa.setPins(csPin, resetPin, irqPin); // Set CS, Reset, IRQ pins
  if (!LoRa.begin(525E6)) 
  {
    Serial.println("LoRa initialization failed. Check connections.");
    while (true); 
  }

  // Set LoRa Parameters
  LoRa.setSpreadingFactor(12);
  LoRa.setSignalBandwidth(250.E3);
  LoRa.setSyncWord(0xF3);

  Serial.println("LoRa initialization succeeded.");
}

void loop() {
    int iny =1011;
    Serial.print("Sending packet: ");
    Serial.println(nodeid);
    LoRa.beginPacket();
   // LoRa.print(nodeid); // Transmit node ID
    LoRa.print(iny); // Add additional data
    int result = LoRa.endPacket(); // Complete packet transmission

    if (result == 1) {
      Serial.println("Packet sent successfully!");
    } else {
      Serial.println("Packet transmission failed!");
    }
    delay(250);
}
