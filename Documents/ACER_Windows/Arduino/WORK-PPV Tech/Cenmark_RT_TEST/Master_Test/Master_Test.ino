#include <SPI.h>
#include <LoRa.h>

const int csPin = 10;
const int resetPin = 9;
const int irqPin = 2;
String receivedData = "";
int flag=1;

void setup() {
  Serial.begin(9600);
  LoRa.setPins(csPin, resetPin, irqPin);
  if (!LoRa.begin(915E6)) {
    while (true);
  }
  LoRa.setSpreadingFactor(12);
  LoRa.setSignalBandwidth(125E3);
  LoRa.setCodingRate4(5);
  LoRa.setTxPower(20);
  LoRa.setSyncWord(0xF3);
  Serial.println("LoRa Master Initialized");
}       
void loop() 
{
  if (flag == 1)
  {
  Lora_Tx("Tx. 22222222.0.0");
  flag=0;
  }
  int packetSize = LoRa.parsePacket();
  if (packetSize) 
  {

    while (LoRa.available()) 
    {
      receivedData += (char)LoRa.read();
    }
    Serial.println("Rx:"+receivedData);
    receivedData.trim();

    if(receivedData == "Tx.10058991.1.0")
    {
      Serial.println("Node 1 Set");
    }
    if(receivedData == "Tx.10058992.2.0")
    {
      Serial.println("Node 2 Set");
      delay(5000);
      Lora_Tx("Tx.11111111.0.0"); 
    }
     receivedData = " ";
  }
}

void Lora_Tx(String message) {
  LoRa.beginPacket();
  LoRa.print(message);
  if (LoRa.endPacket()) {
    Serial.println(message);
  } else {
    Serial.println("Error in Master Tx.");
  }
}
