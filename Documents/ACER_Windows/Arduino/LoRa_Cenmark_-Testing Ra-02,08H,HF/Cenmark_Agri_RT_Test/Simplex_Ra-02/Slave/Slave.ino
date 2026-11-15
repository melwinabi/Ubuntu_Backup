#include <SPI.h>              // include libraries
#include <LoRa.h>
const int csPin = 10;          // LoRa radio chip select nss
const int resetPin = 9;       // LoRa radio reset
const int irqPin = 2;
String received = "";
void setup() {
  Serial.begin(9600);
  pinMode(resetPin,OUTPUT);
  LoRa.setPins(csPin, resetPin, irqPin);  
  if (!LoRa.begin(433E6)) {
    Serial.println("Starting LoRa failed!");
    while (1);
  }

  digitalWrite(resetPin,HIGH);
  LoRa.setSyncWord(0xF3);  
  LoRa.setSpreadingFactor(12); 
  LoRa.setSignalBandwidth(125E3);  
  LoRa.setCodingRate4(5);  
  LoRa.setTxPower(20); 
  Serial.println("LoRa Receiver");
}        

void loop() 
{
  int packetSize = LoRa.parsePacket();
  if (packetSize) {
    received = "";
    while (LoRa.available()) {
      char receivedChar = (char)LoRa.read();
      received += receivedChar;
    }
    Serial.print("Message: ");
    received.trim();
    Serial.println(received);
  }
}
