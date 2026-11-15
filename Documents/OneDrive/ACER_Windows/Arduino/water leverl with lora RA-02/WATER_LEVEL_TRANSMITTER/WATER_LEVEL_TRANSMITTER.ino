#include <SPI.h>              // include libraries
#include <LoRa.h>
const int csPin = 10;          // LoRa radio chip select nss
const int resetPin = 9;       // LoRa radio reset
const int irqPin = 2;         // change for your board; must be a hardware interrupt pin 00

long lastSendTime = 0;        // time of last packet send

String nodeid = "NODE264"; // NODE262-sump and NODE260-tank

const int level1 = 5; // 11
const int level2 = 8; // 14
const int level3 = 7; // 13
const int level4 = 6; // 12
const int sigled = 4; // 12

int level1State = 0;
int level2State = 0;
int level3State = 0;
int level4State = 0; 

void setup() {
  Serial.begin(9600);                   // initialize serial
  pinMode(level1, INPUT);
  pinMode(level2, INPUT);
  pinMode(level3, INPUT);
  pinMode(level4, INPUT);
  pinMode(sigled, OUTPUT);
  LoRa.setPins(csPin, resetPin, irqPin);// set CS, reset, IRQ pin
  
  if (!LoRa.begin(525E6)) 
  {
    Serial.println("LoRa init failed. Check your connections.");
    while (true);
  }
  
  LoRa.setSpreadingFactor(12);
  LoRa.setSignalBandwidth(250E3);
  LoRa.setSyncWord(0xF3);           // ranges from 0-0xFF, default 0x34, see API docs
  Serial.println("LoRa init succeeded.");
}

void loop() 
{
  onReceive(LoRa.parsePacket());
  
}

void sendMessage(String outgoing) {
  LoRa.beginPacket();                   // start packet
  LoRa.print(outgoing);                 // add payload
  LoRa.endPacket();                     // finish packet and send it
}

void onReceive(int packetSize) {
  if (packetSize == 0) return;
  String incoming = "";
  while (LoRa.available()) 
  {
    incoming += (char)LoRa.read();
  }
  Serial.print("RCV : ");
  Serial.println(incoming);
 if (incoming == nodeid)
 {
    level1State = digitalRead(level1);
    level2State = digitalRead(level2);
    level3State = digitalRead(level3);
    level4State = digitalRead(level4);
    Serial.println("ID Recieved");
    if ( level1State == 1 && level2State == 1 && level3State == 1)
    {
      LoRa.beginPacket();
      LoRa.print("2000");
      LoRa.endPacket();
      Serial.println("EMPTY");
    }
    else if ( level1State == 0 && level2State == 1 && level3State == 1)
    {
      LoRa.beginPacket();
      LoRa.print("2100");
      LoRa.endPacket();
      Serial.println("LOW");
    }
    else if ( level1State == 0 && level2State == 0 && level3State == 1)
    {
      LoRa.beginPacket();
      LoRa.print("2110");
      LoRa.endPacket();
      Serial.println("MID");
    }
    else if ( level1State == 0 && level2State == 0 && level3State == 0)
    {
      LoRa.beginPacket();
      LoRa.print("2111");
      LoRa.endPacket();
      Serial.println("HIGH");
    }
    else
    {
      LoRa.beginPacket();
      LoRa.print("2000");
      LoRa.endPacket();
      Serial.println("INVD");
    }
    digitalWrite (sigled, HIGH);
    delay(200);
    digitalWrite (sigled, LOW);
    Serial.println("Data Sent");
 }
}
