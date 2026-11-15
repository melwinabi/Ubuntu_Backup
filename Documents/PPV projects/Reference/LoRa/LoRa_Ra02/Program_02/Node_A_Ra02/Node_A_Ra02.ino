#include <LoRa.h>
#include <Wire.h>
#include "BluetoothSerial.h"

#define ss    5
#define rst   14
#define dio0  2
#define BUTTON_PIN 13

BluetoothSerial SerialBT;

unsigned long previousMillis = 0;
String Node_Tester_ID = "1001";
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
  unsigned long currentMillis = millis();
  if (currentMillis - previousMillis >= 5000) 
  {
   previousMillis = currentMillis;
   Serial.println("2 seconds passed");
   Flag =  1;
  }  
  if (Flag == 1) {
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
  if (packetSize) {
    String received = "";
    while (LoRa.available()) {
      received += (char)LoRa.read();
    }
    received.trim();
    //Serial.println("Received: " + received);

    int snr = LoRa.packetSnr();
    int rssi = LoRa.packetRssi();
    int Rssi = (rssi < 0) ? -rssi : rssi;

    if (received == "AEFJKOPTUXY@1918880914526?ejoty") {
      BT_data(String(snr), String(Rssi));
      String Retransmit = "Ack." + received;
      LoRa_Tx(Retransmit);
    }
    else if (received.startsWith("Ack.")) {
      BT_data(String(snr), String(Rssi));
      SerialBT.println(received);
    }
  }
}

void BT_data(String BT_SNR, String BT_RSSI) {
  String BT_Transfer = "Node_Tester_ID >> " + Node_Tester_ID+ " " + "| SNR = " + BT_SNR + " | RSSI = " + BT_RSSI;
  Serial.println(BT_Transfer);
  SerialBT.println(BT_Transfer);
}
