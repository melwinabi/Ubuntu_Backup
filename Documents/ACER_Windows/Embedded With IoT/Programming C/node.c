#include <LoRa.h>
#include <Wire.h>
#define ss 4
#define rst 5
#define dio0 2

String Master_ID = "300280";
String Node_Id = "1002850";

String part1;
String part2;
String part3;
String part4;
String Percentage_rssi;
String Bt_percent = "89";
String MsgSnr;
String MESSAGE = " ";
String received = " ";

int State1;
int State2;
int State3;
int State4;

int On_Relay = 13;
int Off_Relay = 22;
int Signal = 21;

void setup()
{
  Serial.begin(9600);
  Serial.print("Node_Id > ");
  Serial.println(Node_Id);
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

void loop() {
  Rx_data();
  if (received.startsWith("Tx.")) {
    if (part1 == Node_Id) {
      if (part2 == "01") {
        if (part3 == "1") {
          State1 = 1;
          Serial.println("Valve 1 ON");
        } else if (part3 == "0") {
          State1 = 0;
          Serial.println("Valve 1 OFF");
        }
        MESSAGE = "Ack." + Node_Id + ".01." + String(State1);
        delay(200);
        LoRa_Tx(MESSAGE);
      } 
      else if (part2 == "02") {
        if (part3 == "1") {
          State2 = 1;
          Serial.println("Valve 2 ON");
        } else if (part3 == "0") {
          State2 = 0;
          Serial.println("Valve 2 OFF");
        }
        MESSAGE = "Ack." + Node_Id + ".02." + String(State2);
        delay(200);
        LoRa_Tx(MESSAGE);
      } 
      else if (part2 == "03") {
        if (part3 == "1") {
          State3 = 1;
          Serial.println("Valve 3 ON");
        } else if (part3 == "0") {
          State3 = 0;
          Serial.println("Valve 3 OFF");
        }
        MESSAGE = "Ack." + Node_Id + ".03." + String(State3);
        delay(200);
        LoRa_Tx(MESSAGE);
      } 
      else if (part2 == "04") {
        if (part3 == "1") {
          State4 = 1;
          Serial.println("Valve 4 ON");
        } else if (part3 == "0") {
          State4 = 0;
          Serial.println("Valve 4 OFF");
        }
        MESSAGE = "Ack." + Node_Id + ".04." + String(State4);
        delay(200);
        LoRa_Tx(MESSAGE);
      } 
      else if (part2 == "?") 
      {
         MESSAGE = "Ack." + Node_Id + "." + String(State1)+ "." + String(State2)+ "." + String(State3)+ "." + String(State4);;
         delay(200);
         LoRa_Tx(MESSAGE);
      }
      else if (part2 == "Sg") 
      {
        MESSAGE = "Ack." + Node_Id + String(Percentage_rssi);
        delay(200);
        LoRa_Tx(MESSAGE);  
      } 
      else if (part2 == "Bt") {
        MESSAGE = "Ack." + Node_Id + String(Bt_percent);
        delay(200);
        LoRa_Tx(MESSAGE);          
      }
    }
  }
  received = " "; 
  MESSAGE = " ";
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
