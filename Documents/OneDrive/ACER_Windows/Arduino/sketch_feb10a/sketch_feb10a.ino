#include <LoRa.h>
#include <Wire.h>

#define ss 4 
#define rst 5
#define dio0 2 

String Master_ID = "8112850";   

void setup() 
{
  Serial.begin(115200);
  Serial.println("Setup Starts");
  Serial.print("Master_ID: ");
  Serial.println(Master_ID);

  // Initialize LoRa module at 433 MHz (adjust frequency if needed)
  if (!LoRa.begin(433E6)) {
    Serial.println("LoRa initialization failed!");
    while (1); // Stop execution if LoRa initialization fails
  }

  LoRa.setPins(ss, rst, dio0); 
  LoRa.setSpreadingFactor(12);
  LoRa.setSignalBandwidth(62.5E3);
  LoRa.setCodingRate4(5);
  LoRa.setSyncWord(0xA5);
  LoRa.setPreambleLength(8);  
  delay(500);
  Serial.println("Setup Ends"); 
}

void loop()
{
  LoRa_Tx("hai 1 2 3 4 5 6 7 8 9 0");
  delay(1000); // Add a delay to avoid continuous transmission
}

void LoRa_Tx(String Dxta) 
{
  Serial.println("LoRa IN"); 
  LoRa.beginPacket();
  LoRa.print(Dxta);
  LoRa.endPacket();
  Serial.println("LoRa OUT");
}
