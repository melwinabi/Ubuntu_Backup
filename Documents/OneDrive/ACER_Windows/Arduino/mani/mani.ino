//.............................................OIL AND GAS DETECTION.....................................................
#include<LiquidCrystal_I2C.h>
#include<WiFi.h>
#include <Wire.h>
#include <TinyGPSPlus.h>
TinyGPSPlus gps;
#define GAS_SENSOR_PIN 32
#define H2S_SENSOR_PIN 33
#define f_pin 34
int threshold=40;
const int thresholdValue = 5000;
int count=0;
HardwareSerial SIM900(2);
String gasValue1,H2s_Gas1,flame1;
String apiKey = "95CA4CJPM1BZBJPX";
const char *ssid = "MelwinAbi";     // replace with your WiFi SSID
const char *pass = "Melwin@321";      // replace with your WiFi password
const char *server = "api.thingspeak.com";
WiFiClient client;
LiquidCrystal_I2C lcd(0x3F, 16, 2);
//..............................................................................................
void setup() {
  Serial.begin(115200);
  SIM900.begin(9600);
  lcd.init();
  lcd.backlight();
  lcd.setCursor(0, 0);
  lcd.print("OIL AND GAS");
  lcd.setCursor(0, 1);
  lcd.print("DETECTION");
  delay(5000);
  lcd.clear();
  pinMode(GAS_SENSOR_PIN, INPUT);
  pinMode(H2S_SENSOR_PIN, INPUT);
  pinMode(f_pin, INPUT);
  pinMode(27,OUTPUT);
  connectToWiFi();
  sendSMS();
}
//.................................................................................................
void loop() {
  int gasValue = digitalRead(GAS_SENSOR_PIN);
  int H2s_Gas = digitalRead(H2S_SENSOR_PIN);
  int flame = digitalRead(f_pin);
  Serial.println(gasValue);
  Serial.println(H2s_Gas);
  if (gps.location.isValid()) {
    Serial.print("Latitude: ");
    Serial.print(gps.location.lat(), 6);
    Serial.print(", Longitude: ");
    Serial.println(gps.location.lng(), 6);
  } else {
    Serial.println("GPS data not available");
  }
  if (gasValue ==LOW) {
    gasValue1 = "ABNORMAL";
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("METHANE DETECTED");
    delay(1000);
    digitalWrite(27, HIGH);
    sendSMS();
  } 
   if(gasValue==HIGH)
  {
    lcd.setCursor(0, 0);
    lcd.print("METHANE NORMAL     ");
    delay(1000);
     Serial.println("sensorR :");
    gasValue1 = "NORMAL";
    digitalWrite(27, LOW);
  }
   if (H2s_Gas==LOW) {
    lcd.clear();
    lcd.setCursor(0, 1);
    lcd.print("H2S DETECTED   ");
    digitalWrite(27, HIGH);
    delay(600);
    digitalWrite(27, LOW);
    delay(600);
    digitalWrite(27, HIGH);
    delay(600);
    digitalWrite(27, LOW);
    delay(600);
    digitalWrite(27, HIGH);
    delay(1000);
    digitalWrite(27, LOW);
    sendSMS();
    delay(1000);
    H2s_Gas1 = "ABNORMAL";
      Serial.println("sensorL :");
    
  }
  else if(H2s_Gas==HIGH) 
  {
     lcd.clear();
    lcd.setCursor(0, 1);
    lcd.print("H2S NORMAL");
    delay(1000);
    H2s_Gas1 = "NORMAL";
    Serial.println("sensorL :");
    digitalWrite(27, LOW);
  }
   if (digitalRead(f_pin)==LOW) {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("FIRE DETECTED ");
    Buzzer();
    sendSMS();
    delay(1000);
    flame1 = "FLAME_DETECTED";
  }
  else
  {
    digitalWrite(27,LOW);
    flame1 = "FLAME_NOT_DETECTED";
  }
  
  count++;
  if(count>10)
  {
    sendToThingSpeak(gasValue1, H2s_Gas1, flame1);
    count = 0;
  }
}
//..................................................................................................................
void connectToWiFi() {
  WiFi.begin(ssid, pass);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi connected");
}

void sendToThingSpeak(String gasValue1, String H2s_Gas1, String flame1) {
  if (client.connect(server, 80)) {
    String postStr = "api_key=" + apiKey;
    postStr += "&field1=" + gasValue1;
    postStr += "&field2=" + H2s_Gas1;
    postStr += "&field3=" + flame1;
    postStr += "&field4=" + constructGPSMessage(); // GPS data to field 4
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
  digitalWrite(27, HIGH);
  delay(500);
  digitalWrite(27, LOW);
  delay(500);
}

void sendSMS() {
  // Add code to send SMS via SIM900 module
  SIM900.println("AT+CMGF=1"); // Set SMS mode to text mode
  delay(1000);
  SIM900.print("AT+CMGS=\"+919345896818\"\r"); // Start SMS message transmission
  delay(1000);
  SIM900.println("LEAKAGE DETECTED"); // SMS message content
  delay(1000);
  SIM900.println((char)26); // End of message character (Ctrl+Z)
  delay(1000); // Wait for the module to send the SMS
  Serial.print ("DONE");
}

String constructGPSMessage() {
  // Add code to construct GPS message from TinyGPS++ data
  if (gps.location.isValid()) {
    // Construct the message
    return "Latitude: " + String(gps.location.lat(), 6) + ", Longitude: " + String(gps.location.lng(), 6);
  }
  return ""; // Return empty string if GPS data is invalid
}
