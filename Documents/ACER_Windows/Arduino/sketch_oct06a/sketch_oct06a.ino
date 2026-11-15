#include "BluetoothSerial.h"
#include <EEPROM.h>

BluetoothSerial bt;
String receivedData = " ";
int i, j;
byte val1;
int addr1 = 0;
byte val2;
byte va1;
byte va2;
int addr2 = 30;
String text1, text2;
String pass = " ";
String ssid = " ";
unsigned long startTime;
boolean UD_WiFi();

void setup() {
    Serial.begin(9600);
    bt.begin("Cenmaark_bt");
    startTime = millis();
    EEPROM.begin(512);
    while (true) {
        if (millis() - startTime > 15000) {
            Serial.println("Checking for WiFi connection...");
            if (UD_WiFi()) {
               break;
            } else {
                Serial.println("No data received yet.");
                break;
            }
        }
    }

    Serial.println("MAKING WIFI CONNECTION");
    for (int i = 0; i < 30; i++) {
        va1 = EEPROM.read(i);
        ssid += (char)va1;
        ssid.trim();
    }
     for (int j = 30; j < 50; j++) {
        va2 = EEPROM.read(j);
        pass += (char)va2;
        pass.trim();
    }
     Serial.println("SSID:"+ssid);
     Serial.println("PASS:"+pass);
}

void loop() {}

boolean UD_WiFi() {
    if (bt.available()) {
        receivedData = bt.readStringUntil('\n');
        if (receivedData.startsWith("SSID:")) {
            text1 = receivedData.substring(5, receivedData.indexOf("PASS:"));
            text2 = receivedData.substring(receivedData.indexOf("PASS:") + 5);
            text1.trim();
            text2.trim();
            Serial.println(text1);
            Serial.println(text2);
            writessid();
            writepass();
            receivedData = " ";
            return true;
        }
    }
    return false;
}

void writessid() {
    addr1 = 0;
    for (i = 0; i < text1.length(); i++) {
        val1 = (byte)(text1.charAt(i));
        EEPROM.write(addr1, val1);
        addr1++;
    }
    for (i = addr1; i < 30; i++) {
        EEPROM.write(i, 0);
    }
    EEPROM.commit();
}

void writepass(){
   addr2 = 30;
    for (j = 0; j < text2.length(); j++) {
        val2 = (byte)(text2.charAt(j));
        EEPROM.write(addr2, val2);
        addr2++;
    }
    for (j = addr2; j < 50; j++) {
        EEPROM.write(j, 0);
    }
    EEPROM.commit();
}
