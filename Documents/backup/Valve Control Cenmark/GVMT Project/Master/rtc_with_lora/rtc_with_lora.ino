#include<LoRa.h>
#include <DS3231.h>
#include <Wire.h>
#include <HardwareSerial.h>
#include <TimeLib.h>
#include <stdio.h>
#include <stdlib.h>

#define ss 4 
#define rst 5
#define dio0 2 

#define CLINT 15

HardwareSerial mySerial(2);
DS3231 rtc;

String Master_ID  = "3002850";
String Node_IDs[] = { "1002850", "1002851", "1002852", "1002853", "1002854", "1002855", "1002856" };
String Pump_ID = "2002851";

String part1;
String part2;
String part3;
String part4;
String Percentage_rssi;
String MsgSnr;
String MESSAGE = " ";
String received = " ";

unsigned long previousMillis = 0;
unsigned long currentMillis;
unsigned long currentMillis_2;
unsigned long previousMillis_2 = 0;

const long interval = 60000;
const long interval_2 = 1000;  
bool CenturyBit = false;
bool h12 = false;        
bool hPM = false; 

int x = 0;
int y = 0;
int z = 0;
int a = 0;
int b = 0;
int c = 0;

int Hold = 13; //red
int Start = 22; //green
int Signal = 21; //yellow
int Switch = 1;
int Timer;

bool ID;

void setup() 
{
    Serial.begin(115200); 
    Wire.begin();
    mySerial.begin(115200, SERIAL_8N1, 16, 17); 
    pinMode(CLINT, INPUT);
    pinMode(Hold, OUTPUT);
    pinMode(Start, OUTPUT);
    pinMode(Signal, OUTPUT);
    delay(200);
    digitalWrite(Hold, HIGH);
    digitalWrite(Start, HIGH);
    digitalWrite(Signal, HIGH);
    Serial.print("Master ID > ");
    Serial.println(Master_ID);
    LoRa.setPins(ss, rst, dio0);    
    if (!LoRa.begin(433E6)) 
    {
      Serial.println("LoRa initialization failed!");
      while (true);
    }  
    LoRa.setSpreadingFactor(12); 
    LoRa.setSignalBandwidth(62.5E3);  
    LoRa.setCodingRate4(5);     
    LoRa.setSyncWord(0xA5);         
    LoRa.setPreambleLength(8);  
    delay(800);
    digitalWrite(Hold, LOW);
    delay(200);
    digitalWrite(Start, HIGH);
}

void loop()
{
  currentMillis_2 = millis();
  if (mySerial.available()) 
  {
    String receivedMsg = mySerial.readStringUntil('\n'); 
    Serial.print("Received: ");
    Serial.println(receivedMsg); 
    Network_Data(receivedMsg); 
  } 
  COUNT_t();
  if (Switch == 0) 
  {
    Rx_data(); 
  }
  if (currentMillis_2 - previousMillis_2 >= interval_2) 
  {
    previousMillis_2 = currentMillis_2;
    printTime();
  }  
}

void Network_Data(String incomingByte) 
{
  if (incomingByte.startsWith("Tx.") || incomingByte.startsWith("Ack.")) {
    incomingByte.trim();
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
    part4 = incomingByte.substring(start_index);  
    isValidNode(part1);
    
    if ((part1 == Pump_ID) && ID) {
        if (incomingByte.startsWith("Tx.")) {
            Serial.println("SENDING LoRa......");
            Switch = 0;
            LoRa_Tx(incomingByte);
            previousMillis = millis();
            Timer = 1;
            part1 = "";
        } 
        else if (incomingByte.startsWith("Ack.")) {
            Timer = 0;
            Switch = 1;
            Serial.println("ACK RECEIVED");
        }
        ID = false;
    } 
    else if (part1 == Master_ID) {
        if (part2 == "Rst") {
            Serial.println("Restarting");
            ESP.restart();
        }
    } 
    if(part2 == "tme")
    {
       time(part1);
    }
    else {
        Serial.println("INVALID ID FOUND!");
    }
  }
}

void time(String data) {
    unsigned long num = strtoul(data.c_str(), NULL, 10);
    setTime(num);
    
    x = hour();
    y = minute();
    z = second();
    a = day();
    b = month();
    c = 2000 + year();

    Serial.print("Setting RTC Time to: ");
    Serial.print(x);
    Serial.print(":");
    Serial.print(y);
    Serial.print(":");
    Serial.print(z);
    Serial.print(" Date: ");
    Serial.print(a);
    Serial.print("/");
    Serial.print(b);
    Serial.print("/");
    Serial.println(c);

    delay(100);
    rtc.setSecond(z);
    rtc.setMinute(y);
    rtc.setHour(x);
    rtc.setDate(a);
    rtc.setMonth(b);
    rtc.setYear(c);

    delay(100);

    // Verify RTC time
    Serial.println("Verifying RTC Time...");
    printTime(); 
}


void printTime() {
    byte day = rtc.getDate();
    byte month = rtc.getMonth(CenturyBit);
    byte year = rtc.getYear();
    byte weekday = rtc.getDoW();
    byte hour = rtc.getHour(h12, hPM);
    byte minute = rtc.getMinute();
    byte second = rtc.getSecond();

    Serial.print("Date: ");
    Serial.print(day);
    Serial.print("/");
    Serial.print(month);
    Serial.print("/");
    Serial.print(year);
    Serial.print(" (Day ");
    Serial.print(weekday);
    Serial.println(")");

    Serial.print("Time: ");
    Serial.print(hour);
    Serial.print(":");
    Serial.print(minute);
    Serial.print(":");
    Serial.println(second);
}

void COUNT_t() {
    if (Timer == 1) {
        currentMillis = millis();
        
        if (currentMillis - previousMillis >= interval) {
            Serial.println("Timer expired, sending ACK...");
            delay(500);
            Timer = 0; 
            Switch = 1;
        }
    }
}

void isValidNode(String incomingData) {
  int totalNodes = sizeof(Node_IDs) / sizeof(Node_IDs[0]);
  for (int i = 0; i < totalNodes; i++) {
    if (incomingData.equals(Node_IDs[i])) 
    {
      ID = true;
      return;
    }
  }
  ID = false;
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
    Serial.println("LoRa received: [" + received + "]");
    Network_Data(received); 
  }  
}

void LoRa_Tx(String Data) {
  Serial.println("in");
  LoRa.beginPacket();
  LoRa.print(Data);
  LoRa.endPacket();
  Serial.println(Data);
  digitalWrite(Signal, HIGH);
  delay(100);
  digitalWrite(Signal, LOW);
  delay(100);
  Serial.println("out");
}
