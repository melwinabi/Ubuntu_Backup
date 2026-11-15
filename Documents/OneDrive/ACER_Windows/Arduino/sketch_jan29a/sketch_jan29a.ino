#include "BluetoothSerial.h"
#include <stdio.h>
#include <LoRa.h>
#include <Wire.h>

#define ss 2
#define rst 21
#define dio0 5

BluetoothSerial SerialBT; 

//*****************************************Enter The Master ID*******************************//
String Master_ID = "8112850";
//*******************************************************************************************//
String Serial_data = " ";
String string = " ";
String Node[10];
String part1 = " ";
String part2 = " ";
String part3 = " ";
String Percentage;
String MsgSnr;
String message = " ";
String received = " ";

int Length;
int n;
int D8 = 40;
int D4 = 38;
int Tx_S = 44;
int Pumb_Led = 42;
int sec2;
int Float = 25;
int Aut_Man = 27;
int minm = -122; // change during development
int maxm = -23;


void setup()
{
  Serial.begin(115200);
  SerialBT.begin("ESP32_BT");
  Serial.print("Master_ID: ");
  Serial.println(Master_ID);

  pinMode(Tx_S, OUTPUT);
  pinMode(D8, OUTPUT);
  digitalWrite(D8, LOW);

  LoRa.setPins(ss, rst, dio0);
  if (LoRa.begin(433E6))
  {
    LoRa.setSpreadingFactor(12);
    LoRa.setSignalBandwidth(62.5E3);
    LoRa.setCodingRate4(5);
    LoRa.setSyncWord(0xA5);
    LoRa.setPreambleLength(8);
    Serial.print("Master_ID: ");
    Serial.println(Master_ID);
  }

  delay(1000);
}

void loop()
{
  LoRa_Rx();
  if (SerialBT.available())
  {
    Serial_data = SerialBT.readString();
    Serial_data.trim();
    Serial.println(Serial_data);
    Network_Data(Serial_data);
    Serial_data = " ";
  }
  received = " ";
}

void Network_Data(String incomingByte)
{
  if (incomingByte.startsWith("*"))
  {
    int data1 = Serial_data.indexOf("$");
    if (data1 != -1)
    {
      string = Serial_data.substring(1, data1);
      Length = string.length() / 7;
      SerialBT.println(Length);
      for (n = 1; n <= Length; n++)
      {
        int Start = (n - 1) * 7;
        int End = Start + 7;
        if (End <= string.length())
        {
          Node[n] = string.substring(Start, End);
          delay(100);
        }
      }
      digitalWrite(Tx_S, HIGH);
      delay(200);
      digitalWrite(Tx_S, LOW);
      delay(200);
    }
  }
  else if (incomingByte.startsWith("Tx.") || incomingByte.startsWith("Ack."))
  {
    incomingByte.trim();
    delay(100);
    int start_index = incomingByte.indexOf('.') + 1;
    int part1_end_index = incomingByte.indexOf('.', start_index);
    part1 = incomingByte.substring(start_index, part1_end_index);

    start_index = part1_end_index + 1;
    int part2_end_index = incomingByte.indexOf('.', start_index);
    part2 = incomingByte.substring(start_index, part2_end_index);

    start_index = part2_end_index + 1;
    int part3_end_index = incomingByte.indexOf('.', start_index);
    part3 = incomingByte.substring(start_index, part3_end_index);

    start_index = part3_end_index + 1;
    String part4 = incomingByte.substring(start_index);

    for (int s = 1; s <= Length; s++)
    {
      if (part1 == Node[s])
      {
        if (incomingByte.startsWith("Tx."))
        {
          delay(100);
          LoRa_Tx(incomingByte);
        }
        else if (incomingByte.startsWith("Ack."))
        {
          if (part3 == "Bt")
          {
            message = "Bt." + part1 + "." + part2;
            Serial.println(message);
            message = " ";
          }
          if (part2 == "Sg")
          {
            message = "Sg." + part1 + ":" + MsgSnr + ":" + Percentage;
            Serial.println(message);
            message = " ";
          }
          else if (part2 == "01") // Handle the specific command for pin D8
          {
            if (part3 == "1") // Turn the pin HIGH if the value is "1"
            {
              digitalWrite(D8, HIGH);
              Serial.println("D8 HIGH");
            }
            else if (part3 == "0") // Turn the pin LOW if the value is "0"
            {
              digitalWrite(D8, LOW);
              Serial.println("D8 LOW");
            }
          }
          if (part3 != "Bt" || part2 != "Sg")
          {
            Serial.println(incomingByte);
          }
        }
      }
    }
  }
}

void LoRa_Tx(String data)
{
  SerialBT.println("Done1");
  LoRa.beginPacket();
  digitalWrite(Tx_S, HIGH);
  delay(200);
  digitalWrite(Tx_S, LOW);
  delay(200);
  LoRa.print(data);
  LoRa.endPacket();
  SerialBT.println("Done2");
}

void LoRa_Rx()
{
  int packetSize = LoRa.parsePacket();
  if (packetSize)
  {
    while (LoRa.available())
    {
      char receivedChar = (char)LoRa.read();
      received += receivedChar;
    }
    String lastTransmitted = "";
    received.trim();
    SerialBT.println(received );
    int snr = LoRa.packetSnr();
    MsgSnr = String(snr);
    float rssi = LoRa.packetRssi();
    SerialBT.print("RSSI: ");
    SerialBT.println(rssi);   
    float perc = calculatePercentage(rssi, minm, maxm);
    Percentage = String(perc);
   // Serial.print("Signal Strength: ");
   // Serial.println(Percentage);
    Network_Data(received);
  }
}

float calculatePercentage(int input, int minm, int maxm)
{
  if (input < minm)
    input = minm;
  if (input > maxm)
    input = maxm;
  return ((float)(input - minm) / (maxm - minm)) * 100;
}

//

//Tx.1002850.01.1
