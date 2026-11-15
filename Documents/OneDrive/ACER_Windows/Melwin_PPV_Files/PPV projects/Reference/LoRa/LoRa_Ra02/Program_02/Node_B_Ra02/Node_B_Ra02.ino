#include <LoRa.h>
#include <Wire.h>
#include "BluetoothSerial.h"

#define ss    5
#define rst   14
#define dio0  2
#define BUTTON_PIN 13

BluetoothSerial SerialBT;

unsigned long First_Millis;
unsigned long Second_Millis;
String Node_Tester_ID = "1002";
String expected = "AEFJKOPTUXY@1918880914526?ejoty";
int Flag = 1;

void setup() {
  Serial.begin(115200);
  SerialBT.begin("Node_Tester_ID >> " + Node_Tester_ID);
  //Serial.println("Node_Tester_ID >> " + Node_Tester_ID);

  LoRa.setPins(ss, rst, dio0);
  delay(500);

  if (!LoRa.begin(433E6)) {
    Serial.println("LoRa Failed!!!");
    while (true);
  }
  LoRa.setSpreadingFactor(12);
  LoRa.setSignalBandwidth(62.5E3);
  LoRa.setCodingRate4(5);
  LoRa.setSyncWord(0xA5);
  LoRa.setPreambleLength(8);
  pinMode(BUTTON_PIN, OUTPUT);
  digitalWrite(BUTTON_PIN, HIGH);
  delay(1000);
  //Serial.println("Device Installed");
  SerialBT.println("Analyzing ");
}

void loop() 
{
  if (Flag == 133) {
    SerialBT.println("Analyzing.....");
    LoRa_Tx("AEFJKOPTUXY@1918880914526?ejoty");
    Flag = 0;
  }
  Rx_data();
}

void LoRa_Tx(String Data) {
  //Serial.println("Transmitting...");
  LoRa.beginPacket();
  LoRa.print(Data);
  LoRa.endPacket();
  //Serial.println("Sent: " + Data );
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
      //Serial.println("Received: " + received);
      int snr = LoRa.packetSnr();
      int rssi = LoRa.packetRssi();
      int Rssi = (rssi < 0) ? -rssi : rssi;
      int quality = calculateSignalQuality(expected, received);
      BT_data(String(snr), String(Rssi), String(quality));
      String Retransmit = "Ack." + received;
      LoRa_Tx(Retransmit);
    }
}

int calculateSignalQuality(String expected, String Rec) {
  int totalLength = expected.length();
  int matchCount = 0;

  if (Rec.length() != totalLength) 
  {
    SerialBT.println("RECEIVED >> " + Rec);
    return 0;
  }
  else
  {
    for (int i = 0; i < totalLength; i++) {
      if (expected[i] == Rec[i]) {
        matchCount++;
      }
    }
    int signalQuality = (matchCount * 100) / totalLength;
    return signalQuality;
  }
}

void BT_data(String BT_SNR, String BT_RSSI, String SIGNAL_QUALITY) {
  String BT_Transfer = "Node_Tester_ID >> " + Node_Tester_ID+ " " + "| SNR = " + BT_SNR + " | RSSI = " + BT_RSSI + " | SIGNAL_QUALITY = " + SIGNAL_QUALITY + "%";
  Serial.println(BT_Transfer);
  SerialBT.println(BT_Transfer);
}
