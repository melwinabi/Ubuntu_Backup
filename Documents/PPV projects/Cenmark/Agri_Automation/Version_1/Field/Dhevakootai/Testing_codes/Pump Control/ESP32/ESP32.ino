#include <LoRa.h>
#include <Wire.h>
#define ss 4
#define rst 5
#define dio0 2

String Master_ID = "300280";
String Pump_Id = "2002851";

String part1;
String part2;
String part3;
String part4;
String Percentage_rssi;
String Bt_percent = "89";
String MsgSnr;
String MESSAGE = " ";
String received = " ";

int State  ;
int On_Relay = 13;
int Off_Relay = 22;
int Signal = 21;

void setup()
{
  Serial.begin(9600);
  Serial.print("Pump_Id > ");
  Serial.println(Pump_Id);
  pinMode(Signal, OUTPUT);
  delay(500);
  LoRa.setPins(ss, rst, dio0);
  delay(500);
  if (!LoRa.begin(433E6)) 
  {
    while (true);
  }
  delay(500);
  LoRa.setSpreadingFactor(12);
  LoRa.setSignalBandwidth(62.5E3);  
  LoRa.setCodingRate4(5);     
  LoRa.setSyncWord(0xA5);         
  LoRa.setPreambleLength(8);
  delay(1000);
}

void loop()
{
  Rx_data();
  if (received.startsWith("Tx."))
  {
    if (part1 == Master_ID)
    {
      if (part2 == "HS")
      {
        MESSAGE = "Ack." + part2 + ".Set";
        delay(100);
        LoRa_Tx(MESSAGE);
      }
    }
    else if (part1 == Pump_Id)
    {
      if (part2 == "1")
      {
        State = 1;
        MESSAGE = "Ack." + Pump_Id + ".1";
        delay(100);
        LoRa_Tx(MESSAGE);
      }
      else if (part2 == "0")
      {
        State = 0;
        MESSAGE = "Ack." + Pump_Id + ".0";
        delay(100);
        LoRa_Tx(MESSAGE);
      }
      else if(part2 == "?")
      {
         MESSAGE = "Ack." + Pump_Id + "." + String(State);
         delay(100);
         LoRa_Tx(MESSAGE);          
       } 
      else if(part2 == "Sg")
      {
         MESSAGE = "Ack." + Pump_Id + "." + "Sg." + String(Percentage_rssi);
         delay(100);
         LoRa_Tx(MESSAGE);          
       }
      else if(part2 == "Bt")
      {
         MESSAGE = "Ack." + Pump_Id + "." + "Bt." +String(Bt_percent);
         delay(100);
         LoRa_Tx(MESSAGE);          
       }                 
      }
    }
  received = " "; 
}

void Rx_data()
{
  int packetSize = LoRa.parsePacket();
  if (packetSize)
  {
    received = ""; 
    while (LoRa.available())
    {
      digitalWrite(Signal, HIGH);
      delay(100);  
      digitalWrite(Signal, LOW);
      delay(100);  
      char receivedChar = (char)LoRa.read();
      received += receivedChar;  
    }
    received.trim();
    Serial.println(received);

    int snr = LoRa.packetSnr();
    MsgSnr = String(snr);
    int rssi = LoRa.packetRssi();
    int Rssi = (rssi < 0) ? -rssi : rssi;
    Percentage_rssi = String(Rssi);
    
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
      int part3_end_index = received.indexOf('.', start_index);
      part3 = received.substring(start_index, part3_end_index);

      start_index = part3_end_index + 1;
      part4 = received.substring(start_index);
    }
  }
}

void LoRa_Tx(String Data)
{
  Serial.println("in");
  LoRa.beginPacket();
  LoRa.print(Data);
  LoRa.endPacket();
  Serial.println(Data);
  MESSAGE = " ";
  digitalWrite(Signal, HIGH);
  delay(100);
  digitalWrite(Signal, LOW);
  delay(100);
  digitalWrite(Signal, HIGH);
  delay(100);
  digitalWrite(Signal, LOW);
  delay(100);
  Serial.println("out");
}
