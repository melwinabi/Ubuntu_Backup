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


void loop()
{
  int packetSize = LoRa.parsePacket();
  if (packetSize) {
    Serial.print("Received packet: ");
    String receivedData = "";
    while (LoRa.available()) {
      receivedData += (char)LoRa.read();
    }
    Serial.println(receivedData);
  }
} 
