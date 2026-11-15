#include <Wire.h>
#include <RTClib.h>
#include <WiFi.h>
#include <PubSubClient.h>

WiFiClient espClient;
PubSubClient client(espClient);

const char* ssid = "PPV TECH";
const char* password = "ppv@12345678";
const char* mqtt_server = "198.38.81.119";

String Master_ID = "3002850";
String MESSAGE = " ";
String msg = "";
String MQTT_SEND = "";

unsigned long previousMillis_15s = 0;
unsigned long previousMillis_10s = 0;
const long interval_15s = 15000; 
const long interval_10s = 10000; 



RTC_DS3231 rtc;
const int ALARM_PIN = 23; 

void IRAM_ATTR alarmISR() {
    Serial.println("Alarm Triggered!");
}

void setup() {
    Serial.begin(115200);
    WiFi.begin(ssid, password);
    while (WiFi.status() != WL_CONNECTED)
     {
    delay(500);
    }    
     client.setServer(mqtt_server, 1883);
     client.setCallback(callback);
    Wire.begin(21, 22); 
    pinMode(ALARM_PIN, INPUT_PULLUP);
    attachInterrupt(digitalPinToInterrupt(ALARM_PIN), alarmISR, FALLING);

    if (!rtc.begin()) {
        Serial.println("Couldn't find RTC");
        while (1);
    }

    if (rtc.lostPower()) {
        Serial.println("RTC lost power, setting time.");
        rtc.adjust(DateTime(F(__DATE__), F(__TIME__))); 
    }

    rtc.clearAlarm(1);
    rtc.disableAlarm(2);
    rtc.writeSqwPinMode(DS3231_OFF);

    setNextAlarm();
}
void callback(char* topic, byte* payload, unsigned int length) {
  msg = "";
  for (unsigned int i = 0; i < length; i++) {
    msg += (char)payload[i];
  }
  Serial.println(msg);
}

void reconnect() {
  while (WiFi.status() != WL_CONNECTED) {
    WiFi.begin(ssid, password);
    delay(500);
  }
  while (!client.connected()) {
    String clientId = "Cenmark_Valve_Control";
    clientId += String(random(0xffff), HEX);
    if (client.connect(clientId.c_str())) {
      client.subscribe("Cenmark_PPV");
      MESSAGE = "SYSTEM ON > ID : " + Master_ID;
      client.publish("RX_Valve_test", MESSAGE.c_str());
    } else {
      delay(5000);
    }
  }
}
void loop() {
    if (!client.connected()) {
    reconnect();
  }
  client.loop();
    DateTime now = rtc.now();

    Serial.print("Time: ");
    Serial.print(now.hour());
    Serial.print(':');
    Serial.print(now.minute()+1);
    Serial.print(':');
    Serial.println(now.second());

    if (rtc.alarmFired(1)) {
        Serial.println("ALARM 1 FIRED!");
        MQTT_SEND = "Alarm Fired";
        client.publish("RX_Valve_test", MQTT_SEND.c_str());
        rtc.clearAlarm(1);
        setNextAlarm(); 
    }

    delay(1000);
}

void setNextAlarm() {
    DateTime now = rtc.now();
    int nextHour = now.hour();
    int nextMinute = ((now.minute() / 3) + 1) * 3; 

    if (nextMinute >= 60) {
        nextMinute = 0;
        nextHour++;
        if (nextHour >= 24) nextHour = 0; 
    }

    DateTime nextAlarm(now.year(), now.month(), now.day(), nextHour, nextMinute, 0);
    rtc.setAlarm1(nextAlarm, DS3231_A1_Minute); 
    Serial.print("Next alarm set for: ");
    Serial.print(nextHour);
    Serial.print(":");
    Serial.println(nextMinute);
}
