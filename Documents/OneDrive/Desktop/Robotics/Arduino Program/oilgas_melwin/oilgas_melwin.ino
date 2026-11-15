#include <LCD_I2C.h>
#include <WiFi.h>
#include <Wire.h>
#define GAS_SENSOR_PIN 32
#define f_pin 34
#define BUZZER_PIN 33
#define flowSensorPin 27
volatile int flowPulseCount = 0;
unsigned long lastFlowTime;
float flowRate;
float flowVolume;
int gasValue, flame; 
String gasValue1, flame1,flowRate1;
String apiKey = "RN0RGLAKD1P8WR3F";
const char* ssid = "OILGAS";     // replace with your WiFi SSID
const char* pass = "12345678";    // replace with your WiFi password
const char* server = "api.thingspeak.com";
int count = 0;
void  flowSensorISR();
void connectToWiFi(); 
WiFiClient client;
LCD_I2C lcd(0x27, 16, 2);

void setup() {
  Serial.begin(9600);
  lcd.begin();
  lcd.backlight();
  lcd.setCursor(0, 0);
  lcd.print("OIL AND GAS");
  lcd.setCursor(0, 1);
  lcd.print("DETECTION");
  delay(5000);
  lcd.clear();
  pinMode(flowSensorPin, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(flowSensorPin), flowSensorISR, FALLING); 
  lastFlowTime = millis();
  pinMode(GAS_SENSOR_PIN, INPUT);
  pinMode(f_pin, INPUT);
  pinMode(BUZZER_PIN, OUTPUT);
  connectToWiFi();
}

void loop() {
  unsigned long currentTime = millis();
  if (currentTime - lastFlowTime > 1000) { 
    detachInterrupt(digitalPinToInterrupt(flowSensorPin));
    flowRate = ((1000.0 / (currentTime - lastFlowTime)) * flowPulseCount) / 7.5; 
    flowVolume += (flowRate / 60.0) * (currentTime - lastFlowTime);
//    Serial.print("Flow rate: ");
//    Serial.print(flowRate);
//    Serial.print(" L/min\tTotal volume: ");
//    Serial.print(flowVolume);
//    Serial.println(" Liters");
    flowPulseCount = 0;
    lastFlowTime = currentTime;
    attachInterrupt(digitalPinToInterrupt(flowSensorPin), flowSensorISR, FALLING);
  }

  if(flowRate>=10){
    Serial.print("detc");
     lcd.clear();
     lcd.setCursor(0,1);
     lcd.print("LEAKAGE DETECTED");
     delay(2000);
     flowRate1="LEAKAGE DETECTED";
  }
  else 
  {
     flowRate1="LEAKAGE-NORMAL";
  }
 
  if (digitalRead(GAS_SENSOR_PIN) == LOW) {
    gasValue1 = "ABNORMAL";
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("GAS LEAK DETECTED");
    Buzzer();
    digitalWrite(27, HIGH);
  }    
  else
  {
     gasValue1 = "GAS-NORMAL";
  }
  if (digitalRead(f_pin) == LOW) {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("FIRE DETECTED ");
    Buzzer();
    flame1 = "FLAME_DETECTED";
  } else {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("EVERYTHING NORMAL ");
    flame1 = "FIRE-NORMAL";  
  }

  count++;
  if (count == 7) {   
    sendToThingSpeak(gasValue1,flame1,flowRate1);
    count = 0;
  }
  delay(1000);
}

void connectToWiFi() {
  WiFi.begin(ssid, pass);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi connected");
}

void sendToThingSpeak(String gasValue1, String flame1,String flowRate1) {
  if (client.connect(server, 80)) {
    String postStr = "api_key=" + apiKey;
    postStr += "&field1=" + gasValue1;
    postStr += "&field2=" + flame1;
    postStr += "&field3=" + flowRate1;
    postStr += "\r\n\r\n";

    client.print("POST /update HTTP/1.1\n");
    client.print("Host: api.thingspeak.com\n");
    client.print("Connection: close\n");
    client.print("Content-Type: application/x-www-form-urlencoded\n");
    client.print("Content-Length: ");
    client.print(postStr.length());
    client.print("\n\n");
    client.print(postStr);    

    Serial.println("Data sent to ThingSpeak");
  } else {
    Serial.println("Failed to connect to ThingSpeak server");
  }
}

void Buzzer() {
  digitalWrite(BUZZER_PIN, HIGH);
  delay(100);
  digitalWrite(BUZZER_PIN, LOW);
  delay(100);
  digitalWrite(BUZZER_PIN, HIGH);
  delay(100);
  digitalWrite(BUZZER_PIN, LOW);
  delay(100);
  digitalWrite(BUZZER_PIN, HIGH);
  delay(100);
  digitalWrite(BUZZER_PIN, LOW);
  delay(100);
  digitalWrite(BUZZER_PIN, HIGH);
  delay(100);
  digitalWrite(BUZZER_PIN, LOW);
  delay(100);
}
void flowSensorISR() {
  flowPulseCount++;
}
