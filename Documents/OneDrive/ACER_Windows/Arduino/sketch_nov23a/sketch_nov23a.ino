#include <SPI.h>
#include <LoRa.h>

const int csPin = 10;
const int resetPin = 9;
const int irqPin = 2;
int n;
String receivedData = "";
String Data = " ";

void setup() 
{
  Serial.begin(9600);
  LoRa.setPins(csPin, resetPin, irqPin);
  if (!LoRa.begin(525E6)) 
  {
     Serial.println("LoRa initialization failed. Check connections.");
     while (true);
  }
  LoRa.setSpreadingFactor(12);
  LoRa.setSignalBandwidth(250E3);
  LoRa.setSyncWord(0xF3);
  Serial.println("LoRa Slave_2");
}

void loop() 
{
  int packetSize = LoRa.parsePacket();
  if (packetSize)
  {
    while ( LoRa.available() ) 
    {
      receivedData += (char)LoRa.read();
    }
    Serial.println(receivedData);
    receivedData=" "; 
  }
}
