#include <LoRa.h>
#include <Wire.h>

#define ss 5
#define rst 14
#define dio0 2

void setup() {
  Serial.begin(115200);
  Serial.println("MASTER TESTER > LoRa");
  LoRa.setPins(ss, rst, dio0);
  delay(100);
  if (!LoRa.begin(433E6)) 
  {
    Serial.println("MASTER TESTER > LoRa Failed");
  }
  LoRa.setSpreadingFactor(12);
  LoRa.setSignalBandwidth(62.5E3);
  LoRa.setCodingRate4(5);
  LoRa.setSyncWord(0xA5);
  LoRa.setPreambleLength(8);
  delay(500);
  Serial.println("MASTER TESTER > LoRa Success");  
}


void loop()
 {
  Rx_data();
  if (Serial.available() > 0) 
  {
    String data = Serial.readStringUntil('\n');
    data.trim();
    delay(100);
    LoRa_Tx(data);
    data="";
  }
  delay(10);
}


void LoRa_Tx(String Tx_Data) 
{
  LoRa.beginPacket();
  LoRa.print(Tx_Data);
  LoRa.endPacket();
  Serial.println("Data Transferd > " + String(Tx_Data));
}

void Rx_data() 
{
  int packetSize = LoRa.parsePacket();
  if (packetSize) 
  {
    String received = "";
    while (LoRa.available())
    {
      received += (char)LoRa.read();
    }
    received.trim();
    Serial.println("Received: " + received);
  }
}