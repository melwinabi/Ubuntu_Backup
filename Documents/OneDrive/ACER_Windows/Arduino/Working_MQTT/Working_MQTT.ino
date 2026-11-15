#include <WiFi.h>
#include <PubSubClient.h>
#include<EEPROM.h>
#define LED_PIN 2
#define BUTTON_PIN 4
#define EEPROM_SIZE 64
int EEPROM_add = 0;
int lastButtonState = LOW;
int ledState;
int state1;
int buttonState;
int CurrentState;
int CheckFunction = 0;
unsigned long debounceDuration = 10000; //milli
unsigned long lastTimeButtonStateChanged = 0;
const char* ssid = "Caption_Miller";
const char* password = "12345678";
const char* mqtt_server = "broker.hivemq.com";
WiFiClient espClient;
PubSubClient client(espClient);

void setup(){
  Serial.begin(115200);
  Serial.print("Begin");
  EEPROM.begin(EEPROM_SIZE);
  Serial.print("EEPROM Begin ");
  pinMode(LED_PIN, OUTPUT);
  pinMode(BUTTON_PIN, INPUT_PULLUP);
  Serial.print("Pin Dcl ");
  CheckFunction = 1;
  setup_wifi();
  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);
}

void setup_wifi(){
  delay(10);
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);
  delay(1000);
  Serial.println("");
  Serial.println("WiFi connected");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
}

void callback(char* topic, byte* payload, unsigned int length){
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] ");
  String message = "";
  for (int i = 0; i < length; i++) {
    Serial.print((char)payload[i]);
    message += (char)payload[i];
  }
  Serial.println();
  if (message == "0") {
    ledState=0;
    EEPROM.write(EEPROM_add,ledState);
    EEPROM.commit();
    CheckFunction = 1;
    delay(10000);
    client.publish("LED01_RXD", "OFF");
    Serial.println("MQTT OFF");
    Serial.println("LED turned OFF");
  } else if (message == "1") { 
    ledState=1; 
    EEPROM.write(EEPROM_add,ledState);
    EEPROM.commit();
    CheckFunction = 1;
    delay(10000);
    client.publish("LED01_RXD", "ON");
    Serial.println("MQTT ON");
    Serial.println("LED turned ON");
  }

   
}

void reconnect(){
  if(WiFi.status() != WL_CONNECTED)
  {
    WiFi.begin(ssid, password);
    delay(500);
  }

  else
  {
    while (!client.connected()) 
    {
      Serial.print("Attempting MQTT connection...");
      String clientId = "ESP32Client-";
      clientId += String(random(0xffff), HEX);
  
      if (client.connect(clientId.c_str())) 
      {
        Serial.println("connected");
        client.publish("MelEsp321", "Led Notification");
        client.subscribe("LED01_TXD/#");
      } 
      else 
      {
        Serial.print("failed, rc=");
        Serial.print(client.state());
        Serial.println(" try again in 5 seconds");
        delay(5000);
      }
    }
  }
}

void loop() {
  if(CheckFunction == 1)
  {
    checkState();
  }
  if (millis() - lastTimeButtonStateChanged > debounceDuration) 
  {
     lastTimeButtonStateChanged = millis();
     if (!client.connected()) 
     {
        reconnect();
     }
  }
  ledState=state1;
  buttonState = digitalRead(BUTTON_PIN);
  if(buttonState == LOW)
  {
    CurrentState = EEPROM.read(EEPROM_add);
    Serial.print("Bf State : ");
    Serial.println(CurrentState);
    if(CurrentState == 1)
    {
      CurrentState = 0;
      client.publish("LED01_RXD", "OFF");
    }
    else
    {
      CurrentState = 1;
      client.publish("LED01_RXD", "ON");
    }
    Serial.print("Af State : ");
    Serial.println(CurrentState);
    EEPROM.write(EEPROM_add,CurrentState);
    EEPROM.commit();
    CheckFunction = 1;
    Serial.println("Button");
    delay(500);
  }
  client.loop();
}

void checkState(){
  ledState = EEPROM.read(EEPROM_add);
  Serial.print("ledState : ");
  Serial.println(ledState);
  digitalWrite(LED_PIN,ledState);
  CheckFunction = 0;
}//working pub
