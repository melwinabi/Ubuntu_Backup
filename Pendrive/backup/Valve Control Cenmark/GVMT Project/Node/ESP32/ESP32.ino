#include <LoRa.h>
#include <Wire.h>
#define ss 5
#define rst 14
#define dio0 2

String Master_ID = "3002852";
String Node_ID = "1002862";

String part1, part2, part3, part4, Percentage_rssi, MsgSnr, Message, received;

int VO_1 = 27;
int VO_2 = 26;
int VT_1 = 25;
int VT_2 = 33;

bool State = true;

void setup() {
  Serial.begin(115200);
  Serial.println("Master_ID > " + Master_ID);
  Serial.println("Node_Id > " + Node_ID);

  pinMode(VO_1, OUTPUT);
  pinMode(VO_2, OUTPUT);
  pinMode(VT_1, OUTPUT);
  pinMode(VT_2, OUTPUT);

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
  delay(1000);
}

void loop() {
  Rx_data();
  if(millis() > 4000000)
  {
    delay(50);
    esp_restart();
  }
  if (received.startsWith("@@*")) {
    Seprator(received);
    if (part1 == Master_ID && part2 == Node_ID) {
      if (part3 == "01") {
        if (part4 == "000") 
        {
          digitalWrite(VO_2, LOW);
          delay(250);
          digitalWrite(VO_1, HIGH);
          delay(500);
          digitalWrite(VO_1, LOW);
          sendResponse(part4,part3);
        } 
        else if (part4 == "010") 
        {
          digitalWrite(VO_1, LOW);
          delay(250);
          digitalWrite(VO_2, HIGH);
          delay(500);
          digitalWrite(VO_2, LOW);
          sendResponse(part4,part3);
        }
      }
      else if (part3 == "02") 
      {
        if (part4 == "000") 
        {
          digitalWrite(VT_2, LOW);
          delay(250);
          digitalWrite(VT_1, HIGH);
          delay(500);
          digitalWrite(VT_1, LOW);
          sendResponse(part4,part3);
        } 
        else if (part4 == "010") 
        {
          digitalWrite(VT_1, LOW);
          delay(250);
          digitalWrite(VT_2, HIGH);
          delay(500);
          digitalWrite(VT_2, LOW);
          sendResponse(part4,part3);
        }
      }
    } 
    else if(part1 == Master_ID && part2 == "111")
    {
      digitalWrite(VO_2, LOW);
      delay(250);
      digitalWrite(VO_1, HIGH);
      delay(500);
      digitalWrite(VO_1, LOW);
      delay(500);
      digitalWrite(VT_2, LOW);
      delay(250);
      digitalWrite(VT_1, HIGH);
      delay(500);
      digitalWrite(VT_1, LOW);
      delay(250);
      Serial.println("Total OFF");
    }
    else
    {
      Serial.println("Invalid ID");   
    }
    clearParts();
  }
}

void Rx_data() 
{
  int packetSize = LoRa.parsePacket();
  if (packetSize) 
  {
    received = "";
    while (LoRa.available())
    {
      received += (char)LoRa.read();
    }
    received.trim();
    Serial.println("Received: " + received);

    int snr = LoRa.packetSnr();
    MsgSnr = String(snr);

    int rssi = LoRa.packetRssi();
    int Rssi = (rssi < 0) ? -rssi : rssi;
    Percentage_rssi = String(Rssi);
  }
}

void sendResponse(String valveState, String valveID) 
{
  Message = "##*" + Master_ID + "*" + valveState + "*" + Node_ID + valveID + "*" + Percentage_rssi;
  LoRa_Tx(Message);
}

void LoRa_Tx(String Data) {
  Serial.println("Transmitting...");
  LoRa.beginPacket();
  LoRa.print(Data);
  LoRa.endPacket();
  Serial.println("Sent: " + Data);
  State = true;
}

void Seprator(String Incomming) {
  if (Incomming.startsWith("@@*")) {
    Incomming.trim();
    int start_index = Incomming.indexOf('*') + 1;
    int part1_end_index = Incomming.indexOf('*', start_index);
    part1 = Incomming.substring(start_index, part1_end_index);

    start_index = part1_end_index + 1;
    int part2_end_index = Incomming.indexOf('*', start_index);
    part2 = Incomming.substring(start_index, part2_end_index);

    start_index = part2_end_index + 1;
    int part3_end_index = Incomming.indexOf('*', start_index);
    part3 = Incomming.substring(start_index, part3_end_index);

    start_index = part3_end_index + 1;
    part4 = Incomming.substring(start_index);

    part1.trim();
    part2.trim();
    part3.trim();
    part4.trim();

    Serial.println("Master_ID > " + part1);
    Serial.println("Node_ID   > " + part2);
    Serial.println("Valve_ID  > " + part3);
    Serial.println("State     > " + part4);
  }
}

void clearParts() {
  part1 = "";
  part2 = "";
  part3 = "";
  part4 = "";
  received = "";
}
