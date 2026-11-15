/*New Blynk app with Home Automation
   Home Page
*/
#define BLYNK_TEMPLATE_ID "TMPL3jtlsqhcB"
#define BLYNK_TEMPLATE_NAME "agri bot"
#define BLYNK_AUTH_TOKEN "Qrksmu-2xXaKSqMJ_rKA7z0GnhyZ1uxp"
//Include the library files
#define BLYNK_PRINT Serial
#include <ESP8266WiFi.h>
#include <BlynkSimpleEsp8266.h>

#include <DHT.h>
#define DHTPIN 4
#define DHTTYPE DHT11
String x;
void data();
void camera();
String removeWhitespace(String str);
DHT dht(DHTPIN, DHTTYPE);

#define sensorpin A0

BlynkTimer timer;

char auth[] = BLYNK_AUTH_TOKEN;
char ssid[] = "AGRI_BOT";//Enter your WIFI name
char pass[] = "12345678";//Enter your WIFI password

//Get the button values
BLYNK_WRITE(V1) {
  bool value1 = param.asInt();
  // Check these values and turn the relay1 ON and OFF
  if (value1 == 1) {
    Serial.println("A");
  } else {
    Serial.println("B");
  }
}


void setup() {
  Serial.begin(115200);
  dht.begin();
  pinMode(sensorpin, INPUT);
  pinMode(DHTPIN, INPUT);

  //Initialize the Blynk library
  Blynk.begin(auth, ssid, pass, "blynk.cloud", 80);
  timer.setInterval(1000L, data);
}

void loop() {
  //Run the Blynk library
  Blynk.run();
  timer.run();
  data();
  camera();

}
void camera()
{
  if (Serial.available() > 0) {
    String receivedString = Serial.readString();
    receivedString.trim(); // Remove leading and trailing whitespace
    receivedString = removeWhitespace(receivedString); // Remove all whitespace characters
      Serial.println(receivedString);

    if (receivedString.equals("APPLE"))
    {

      x = "APPLE";
       Serial.println(x);
      delay(10000);

    }
  }
    else 
    {
        x = "NO_DATA";
         Serial.println(x);
    }
  
}
String removeWhitespace(String str) {
  str.replace(" ", ""); // Remove spaces
  str.replace("\t", ""); // Remove tabs
  str.replace("\r", ""); // Remove carriage returns
  //str.replace("\n", ""); // Remove newlines
  return str;
}
void data() {

  int sensor = analogRead(A0);
  float h = dht.readHumidity();
  float t = dht.readTemperature();
 
  Blynk.virtualWrite(V2, x);
  Blynk.virtualWrite(V3, sensor);
  Blynk.virtualWrite(V4, h);
  Blynk.virtualWrite(V5, t);

}
