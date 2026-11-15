#include <SPI.h>
#include <LoRa.h>
#define ss 53
#define rst 10
#define dio0 2 

String Node_Id = "1002850"; //// change for next device.
int D8 = 44;
int D7 = 40;
int sts_v1;
int sts_v2;
int Percentage = 100;
String received = "";
String lastTransmitted = ""; 
String part1;
String part2;
String part3;
String Message = "";

void setup() 
{
  Serial.begin(9600);
  LoRa.setPins(ss, rst, dio0);  
  if (!LoRa.begin(433E6)) {
    Serial.println("LoRa initialization failed. Check connections.");
    while (true); 
  } 
  LoRa.setSpreadingFactor(12); 
  LoRa.setSignalBandwidth(62.5E3);  
  LoRa.setCodingRate4(5);     
  LoRa.setSyncWord(0xA5);         
  LoRa.setPreambleLength(8);
  Serial.print("Node_ID: ");
  Serial.println(Node_Id);
}

void loop() 
{
  Rx_data();
  if (received.startsWith("Tx.")) 
  {
    if (part1 == Node_Id)
    {
      if (part2 == "01")
      {
        if (part3 == "1")
        {
          digitalWrite(D7, HIGH);
          sts_v1 = 1;
          Message = "Ack." + part1 + ".01.1";      
          Tx_data(Message);      
        }
        else if (part3 == "0")
        {
          digitalWrite(D7, LOW);
          sts_v1 = 0;  
          Message = "Ack." + part1 + ".01.0";               
          Tx_data(Message);               
        }
        else if (part3 == "?")
        {
          if (sts_v1 == 1)
          {
            Message = "Ack." + part1 + ".01.1";                
            Tx_data(Message);          
          }
          else if (sts_v1 == 0)
          {
            Message = "Ack." + part1 + ".01.0";              
            Tx_data(Message);            
          }
        }
      }
      else if (part2 == "02")
      {
        if (part3 == "1")
        {
          digitalWrite(D8, HIGH);
          sts_v2 = 1;
          Message = "Ack." + part1 + ".02.1";      
          Tx_data(Message);      
        }
        else if (part3 == "0")
        {
          digitalWrite(D8, LOW);
          sts_v2 = 0;  
          Message = "Ack." + part1 + ".02.0";              
          Tx_data(Message);               
        }
        else if (part3 == "?")
        {
          if (sts_v2 == 1)
          {
            Message = "Ack." + part1 + ".02.1"; 
            delay(100);                
            Tx_data(Message);          
          }
          else if (sts_v2 == 0)
          {
            Message = "Ack." + part1 + ".02.0";     
            delay(100);            
            Tx_data(Message);            
          }
        }
      }
      else if (part2 == "Sg")
      {
        Message = "Ack." + Node_Id + ".Sg";   
        delay(100);             
        Tx_data(Message);
      } 
      else if (part2 == "Bt")
      {
        Message = "Ack." + Node_Id + "."  + Percentage + ".Bt";   
        delay(100);             
        Tx_data(Message);
      }            
    } 
  else if (part1 != Node_Id)
  {
    if (received != lastTransmitted) 
    { 
      Serial.println("gfgfgf");
      Tx_data(received);     
      lastTransmitted = received;   
    } 
  }
 }   
  received = "";  
  part1 = ""; 
  part2 = ""; 
  part3 = "";                
}

void Rx_data() {
  int packetSize = LoRa.parsePacket();
  if (packetSize) {
    received = "";
    while (LoRa.available()) {
      char receivedChar = (char)LoRa.read();
      received += receivedChar;
    }
    received.trim();
    Serial.print("Received: ");
    Serial.println(received); 
       
    if (received.startsWith("Tx.") || received.startsWith("Ack.")) 
    {
      received.trim();
      int start_index = received.indexOf('.') + 1;
      int part1_end_index = received.indexOf('.', start_index);
      part1 = received.substring(start_index, part1_end_index);

      start_index = part1_end_index + 1;
      int part2_end_index = received.indexOf('.', start_index);
      part2 = received.substring(start_index, part2_end_index);

      start_index = part2_end_index + 1;
      part3 = received.substring(start_index);    
    }
    Serial.println(part1);
    Serial.println(part2);
    Serial.println(part3);
  }
}

void Tx_data(String data) 
{
  LoRa.beginPacket();
  digitalWrite(D8, HIGH);   
  delay(300);
  digitalWrite(D8, LOW);  
  LoRa.print(data);
  LoRa.endPacket();
  Serial.println(data);
  Message = "";
}


//    Tx.1002850.Sg
//    Tx.1002850.Bt
//    Tx.1002850.01.1
//    Tx.8112850.01
//    Tn.8112850.12.21
//    Tf.8112850.12.21
