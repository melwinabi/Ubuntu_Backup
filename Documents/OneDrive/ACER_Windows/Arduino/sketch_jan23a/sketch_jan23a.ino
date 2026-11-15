#include <stdio.h>
#include <stdlib.h>
#include <LoRa.h>
#include <Wire.h>
#include "RTClib.h"

#define ss 10
#define rst 9
#define dio0 2

RTC_DS3231 rtc;

String Master_ID = "8112850";
int D8 = 30;
int D4 = 48;
int Tx_S = 44;
int Pumb_Led = 42;
int sec2;
int Float = 25;
int Aut_Man = 27;
int n, s;
int Flt_s;
int Prs_s;
int pup_s;
int AM_s;
int Hr = 2;
int Mn = 3;
int minm = -122;
int maxm = -23;
int Time;
String Serial_data = " ";
String part1;
String part2;
String part3;
int Flag;
String Node[10];
int Length;
String string = " ";
String lastTransmitted = " ";
String Message = " ";
String received = "";
String Percentage;
String MsgSnr;
float pressure;

void setup()
{
    Serial.begin(115200);
    Serial1.begin(115200);
      pinMode(Tx_S, OUTPUT);
  pinMode(D8, OUTPUT);
  pinMode(D4, OUTPUT);
  pinMode(Pumb_Led, OUTPUT);
  pinMode(Float, INPUT);
  pinMode(Aut_Man, INPUT);
  digitalWrite(Float, HIGH);
  digitalWrite(Aut_Man, HIGH);
  pinMode(A0, INPUT);
  delay(500);
  if (!rtc.begin()) {
    Serial.println("Couldn't find RTC. Check connections!");
    while (1);
  } 
  if (!LoRa.begin(433E6))
  {
    while (true);
  }
  LoRa.setSpreadingFactor(12); 
  LoRa.setSignalBandwidth(62.5E3);  
  LoRa.setCodingRate4(5);     
  LoRa.setSyncWord(0xA5);         
  LoRa.setPreambleLength(8);
  Serial.print("Master_ID: ");
  Serial.println(Master_ID);
}

void loop()
{
 
    if (Serial1.available())
    {
        Serial_data = Serial1.readString();
        Serial_data.trim();
        Serial.println(Serial_data);
        MQTT(Serial_data);
    }
}

void MQTT(String incomingByte)
{
    if (incomingByte.startsWith("*"))
    {
        int data1 = Serial_data.indexOf("$");
        if (data1 != -1)
        {
            string = Serial_data.substring(1, data1);
            Serial_data = " ";
            Length = string.length() / 7;
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
                        Message = "Bt." + part1 + "." + part2;
                        Serial1.println(Message);
                        Message = " " ;
                    }
                    if (part2 == "Sg")
                    {
                        Message = "Sg." + part1 + ":" + MsgSnr + ":" + Percentage;
                        Serial1.println(Message);
                        Message = " " ;
                    }
                    if (part3 != "Bt" || part2 != "Sg")
                    {
                        Serial.println(incomingByte);
                    }
                }
            }
        }
        if (incomingByte.startsWith("Tn."))
        {
        }
        if (part1 == Master_ID)
        {
            if (part2 == "00")
            {
                LOW_Ack();
            }
            else if (part2 == "01")
            {
                pup_s = 1;
                delay(250);
                digitalWrite(Pumb_Led, HIGH);
                Message = "Ack." + Master_ID + ".01";
                Serial1.println(Message);
                Message = " " ;
            }
            else if (part2 == "?")
            {
                if (pup_s == 1)
                {
                    Message = "Ack." + Master_ID + ".01";
                    Serial1.println(Message);
                    delay(200);
                    Message = " ";
                }
                else if (pup_s == 0)
                {
                    Message = "Ack." + Master_ID + ".00";
                    Serial1.println(Message);
                    Message = " " ;
                    delay(200);
                    Message = String("Ack.") + pup_s + ":" + pressure + ":" + Flt_s + ":" + AM_s;
                    Serial1.println(Message);
                    Message = " " ;
                }
            }
        }
    }
    incomingByte = " ";
    received = " ";
    MsgSnr = " ";
    Percentage = " ";
}

void LoRa_Tx(String data)
{
    LoRa.beginPacket();
    digitalWrite(Tx_S, HIGH);
    delay(200);
    digitalWrite(Tx_S, LOW);
    delay(200);
    LoRa.print(data);
    LoRa.endPacket();
    Serial.println("Done");
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
        received.trim();
        int snr = LoRa.packetSnr();
        MsgSnr = String(snr);
        float rssi = LoRa.packetRssi();
        float perc = calculatePercentage(rssi, minm, maxm);
        Percentage = String(perc);
        MQTT(received);
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

void LOW_Ack()
{
    delay(250);
    digitalWrite(Pumb_Led, LOW);
    pup_s = 0;
}

void Pressure_Monitoring()
{
    int High_Lps = 9;
    int Low_Lps = 2;
    int potValue = analogRead(A0);
    float voltage = potValue * (4.0 / 1023.0) + 0.5;
    pressure = (voltage - 0.5) / 4.0 * 10.0;
    if ((pressure >= 9) || (pressure <= 2))
    {
        if (pressure <= 2)
        {
            LOW_Ack();
            Prs_s = 1;
            String pressureStr = String(pressure, 2);
            String Bar = String("Ack.") + Master_ID + ".L_P:" + pressureStr + "." + Master_ID + ".00";
            Serial1.println(Bar);
        }
        else if (pressure >= 9)
        {
            LOW_Ack();
            Prs_s = 5;
            String pressureStr = String(pressure, 2);
            String Bar = String("Ack.") + Master_ID + ".H_P:" + pressureStr + "." + Master_ID + ".00";
            Serial1.println(Bar);
        }
    }
    else
    {
        Prs_s = 0;
    }
}

void Float_Signal()
{
    int Float_Value = digitalRead(Float);
    if (Float_Value == 1)
    {
        LOW_Ack();
        Flt_s = 1;
        Message = " ";
        Message = String("Ack.") + Master_ID + ".L_F" + "." + Master_ID + ".00";
        Serial1.println(Message);
        Message = " " ;
    }
    else
    {
        Flt_s = 0;
    }
}
