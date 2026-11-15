#include <SPI.h>
#include <LoRa.h>
const int csPin = 10;
const int resetPin = 9;
const int irqPin = 2;
String previousData = "";

void setup() {
  Serial.begin(9600);
  Serial.println("LoRa Slave");
  LoRa.setPins(csPin, resetPin, irqPin);
  if (!LoRa.begin(525E6)) 
  {
     Serial.println("LoRa initialization failed. Check connections.");
     while (true);
  }
  LoRa.setSpreadingFactor(12);
  LoRa.setSignalBandwidth(250E3);
  LoRa.setSyncWord(0xF3);
}

void loop() 
{
   int packetSize = LoRa.parsePacket();
   if (packetSize > 0)
   {
      String receivedData = "";
      while ( LoRa.available() ) 
      {
         receivedData += (char)LoRa.read();
      }
       Serial.println(receivedData);
      }
   }
