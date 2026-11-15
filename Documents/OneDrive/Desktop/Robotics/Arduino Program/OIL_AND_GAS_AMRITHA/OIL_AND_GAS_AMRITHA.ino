#include <LCD_I2C.h>
#include<WiFi.h>
#include <Wire.h>
#define GAS_SENSOR_PIN 32
#define f_pin 34
int threshold=40;
const int thresholdValue = 5000;
String gasValue1,H2s_Gas1,flame1;
String apiKey = "95CA4CJPM1BZBJPX";
const char* ssid = "OILGAS";     // replace with your WiFi SSID
const char* pass = "12345678";      // replace with your WiFi password
const char* server = "api.thingspeak.com";
WiFiClient client;
LCD_I2C lcd(0x27, 16, 2);

void setup() {
  Serial.begin(115200);
  lcd.begin();
  lcd.backlight();
  lcd.setCursor(0, 0);
  lcd.print("OIL AND GAS");
  lcd.setCursor(0, 1);
  lcd.print("DETECTION");
  delay(5000);
  lcd.clear();

  pinMode(GAS_SENSOR_PIN, INPUT);
  
  pinMode(f_pin, INPUT);
  pinMode(33,OUTPUT);

  connectToWiFi();
}

void loop() {
  int gasValue = digitalRead(GAS_SENSOR_PIN);
  int flame = digitalRead(f_pin);
  Serial.println(gasValue);
  Serial.println("\t");
  Serial.println(flame);
  if (gasValue==HIGH) {
    gasValue1 = "ABNORMAL";
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("GAS LEAK DETECTED");
    digitalWrite(27, HIGH);
      }    
  
  if (digitalRead(f_pin)==LOW) {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("FIRE DETECTED ");
    Buzzer();
    flame1 = "FLAME_DETECTED";
  }
  else
  {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("GAS LEVEL ");
    lcd.setCursor(0, 1);
    lcd.print("NORMAL ");
    gasValue1 = "NORMAL";
    flame1 = "FLAME_NOT_DETECTED";
    delay(1000);
  }



  sendToThingSpeak(gasValue1, flame1);

  delay(10000); // Wait for 15 seconds before next update
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

void sendToThingSpeak(String gasValue1, String flame1) {
  if (client.connect(server, 80)) {
    String postStr = "api_key=" + apiKey;
    postStr += "&field1=" + gasValue1;
    postStr += "&field2=" + flame1;
    postStr += "\r\n\r\n";

    client.print("POST /update HTTP/1.1\n");
    client.print("Host: api.thingspeak.com\n");
    client.print("Connection: close\n");
    client.print("Content-Type: application/x-www-form-urlencoded\n");
    client.print("Content-Length: ");
    client.print(postStr.length());
    client.print("\n\n");
    client.print(postStr);

    delay(1000); // Allow time for the server to process the request

    if (client.connected()) {
      client.stop();
      Serial.println("Data sent to ThingSpeak");
    } else {
      Serial.println("Connection to ThingSpeak closed prematurely");
    }
  } else {
    Serial.println("Failed to connect to ThingSpeak server");
  }
}

void Buzzer() {
  digitalWrite(33, HIGH);
  delay(500);
  digitalWrite(33, LOW);
  delay(500);
}
