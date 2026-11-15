#include <SPI.h>
#include <LoRa.h>

const int csPin = 10;       // NSS Pin
const int resetPin = 9;     // Reset Pin
const int irqPin = 2;       // IRQ Pin
const int interval =400;
String nodeid = "NODE264";

void setup() {
  Serial.begin(9600);
  Serial.println("LoRa Receiver");
  LoRa.setPins(csPin, resetPin, irqPin); 
  if (!LoRa.begin(525E6)) {
    Serial.println("LoRa initialization failed. Check connections.");
    while (true); 
  }
  LoRa.setSpreadingFactor(12);
  LoRa.setSignalBandwidth(250E3);
  LoRa.setSyncWord(0xF3); 
  Serial.println("LoRa initialization succeeded.");
}

void loop() {
  int packetSize = LoRa.parsePacket();
  if (packetSize)
  {
    Serial.print("Received packet: ");
    String receivedData = "";
    while (LoRa.available())
    {
      receivedData += (char)LoRa.read(); // Read and append as characters
    }
    Serial.println(receivedData);
    if (receivedData == "11111101") 
    { 
        Serial.println("Data recived");
        LoRa.beginPacket();
        LoRa.print(receivedData);
        LoRa.print("1234");
        int result = LoRa.endPacket();
        if (result == 1) 
        {
          Serial.println("Packet sent successfully!");
         } 
         else 
         {
           Serial.println("Packet transmission failed!");
          }
          Serial.print("RSSI: ");
          Serial.println(LoRa.packetRssi());    
      }
  }
  delay(250);
}
