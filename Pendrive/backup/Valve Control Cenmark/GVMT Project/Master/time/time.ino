#include <stdio.h>
#include <stdlib.h>
#include <HardwareSerial.h>
#include <DS3231.h>
#include <Wire.h>
#include <TimeLib.h>

#define CLINT 15
#define Led 2

HardwareSerial mySerial(2);
DS3231 rtc;

String part1;
String part2;
String part3;
String part4;
unsigned long previousMillis = 0;
const long interval = 1000;  
bool CenturyBit = false;
bool h12 = false;        
bool hPM = false; 

void setup() {
    Serial.begin(115200); 
    Wire.begin();
    mySerial.begin(115200, SERIAL_8N1, 16, 17); 
    pinMode(CLINT, INPUT);
    pinMode(Led, OUTPUT);
    Serial.println("ESP32 Ready to Communicate...");
}

void loop() {
    if (mySerial.available()) {
        String receivedMsg = mySerial.readStringUntil('\n'); 
        Serial.print("Received: ");
        Serial.println(receivedMsg); 
        Network_Data(receivedMsg);
    }

    unsigned long currentMillis = millis();
    if (currentMillis - previousMillis >= interval) {
        previousMillis = currentMillis;
        printTime();
    }
}

void Network_Data(String incomingByte) {
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

        if (part2 == "tme") {
            time(part1);
        }
    }
}

void time(String data) {
    unsigned long num = strtoul(data.c_str(), NULL, 10);
    setTime(num);
    int x = hour();
    int y = minute();
    int z = second();
    int a = day();
    int b = month();
    int c = 2000 + year();

    Serial.print("Hour: ");
    Serial.print(x);
    Serial.print(" Minute: ");
    Serial.print(y);
    Serial.print(" Second: ");
    Serial.print(z);
    Serial.print(" Date: ");
    Serial.print(a);
    Serial.print(" Month: ");
    Serial.print(b);
    Serial.print(" Year: ");
    Serial.println(c); 

    delay(100);
    rtc.setSecond(z);
    rtc.setMinute(y);
    rtc.setHour(x);
    rtc.setDate(a);
    rtc.setMonth(b);
    rtc.setYear(c);
    delay(100);
    
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
