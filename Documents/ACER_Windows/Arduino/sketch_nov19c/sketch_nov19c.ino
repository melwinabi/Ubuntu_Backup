#include <SPI.h>
#include <LoRa.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
const int csPin = 10;       // NSS Pin
const int resetPin = 9;     // Reset Pin
const int irqPin = 2;       // IRQ Pin
String nodeid = "NODE264";  // Node Identifier
long lastSendTime = 0;      // Timestamp for the last packet sent
int interval = 2000;        // Interval between packets (milliseconds)
int counter = 0;            // Packet counter
String string;
int Length;
int x = 1,y = 8;;
String incomingByte;
String receivedData;

void setup() 
{
  Serial.begin(115200);  
  LoRa.setPins(csPin, resetPin, irqPin);
  if (!LoRa.begin(525E6)) 
  {
    Serial.println("LoRa initialization failed. Check connections.");
    while (true); 
  } 
  LoRa.setSpreadingFactor(12);
  LoRa.setSignalBandwidth(250.E3);
  LoRa.setSyncWord(0xF3); 
  Serial.println("LoRa initialization succeeded.");
  delay(100);
  Serial.println("Master Active");
}

void loop() 
{
  if (Serial.available() > 0) 
  {
     incomingByte = Serial.readString();
     Serial.println(incomingByte);
    if (incomingByte.startsWith("*")) {
      int data1 = incomingByte.indexOf("$");
      if (data1 != -1) {
        string = incomingByte.substring(1, data1);
        Length = string.length() / 7;
        Serial.print("Length: ");
        Serial.println(Length);
        for (int n = 1; n <= Length; n++) {
          int Start = (n - 1) * 7;
          int End = Start + 7;
          if (End <= string.length()) {
            String ID = string.substring(Start, End);
            Serial.print("Channel_Id[");
            Serial.print(n);
            Serial.print("]: ");
            Serial.println(ID);
            delay(50);
          } else {
            Serial.println("Error: Invalid substring range.");
            break;
          }
        }
        Serial.println("ALL set");
      } else {
        Serial.println("Error: Missing '$' in input.");
      }
    }
  }
  int packetSize = LoRa.parsePacket();
  if (packetSize)
  {
    Serial.print("Received packet: ");
    receivedData = "";
    while (LoRa.available())
    {
      receivedData += (char)LoRa.read(); // Read and append as characters
    }
    Serial.println(receivedData);
  }
    if(incomingByte == "11111101")
    {
      long int iny =11111101;
      Serial.print("Sending packet: ");
      //Serial.println("09090909090909090909090909");
      LoRa.beginPacket();
      LoRa.print(iny);
      int result = LoRa.endPacket();
      if (result == 1) 
      {
        Serial.println("PUMB 1 Signal");
      } 
      else 
      {
        Serial.println("Packet transmission failed!");
      }
    }

    else if(incomingByte == "11111111")
    {
      long int iny =11111111;
      Serial.print("Sending packet: ");
      //Serial.println("09090909090909090909090909");
      LoRa.beginPacket();
      LoRa.print(iny);
      int result = LoRa.endPacket();
      if (result == 1) 
      {
        Serial.println("PUMB 2 Signal");
      } 
      else 
      {
        Serial.println("Packet transmission failed!");
      }
    }
     if(incomingByte == "12301")
     {
       Serial.println("PUMP ONE ON") ;
             Serial.println("PUMP ONE OFF") ;
         LoRa.beginPacket();
        LoRa.print("12301");
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
     else if(incomingByte == "12300")
     {
      Serial.println("PUMP ONE OFF") ;
            Serial.println("PUMP ONE OFF") ;
         LoRa.beginPacket();
        LoRa.print("12300");
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
     else if(incomingByte == "12311")
     {
      Serial.println("PUMP TWO ON") ;
            Serial.println("PUMP ONE OFF") ;
         LoRa.beginPacket();
        LoRa.print("12311");
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
     else if(incomingByte == "12310")
     {
      Serial.println("PUMP ONE OFF") ;
         LoRa.beginPacket();
        LoRa.print("12310");
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
 delay(250);
}
