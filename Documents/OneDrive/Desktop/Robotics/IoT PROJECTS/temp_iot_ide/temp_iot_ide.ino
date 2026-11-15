//#include <dht11.h>

#include "ThingSpeak.h"

#include <WiFi101.h>
#include <DHT.h>

char ssid[] = "MelwinAbi";    //  your network SSID (name) 
    char pass[] = "Melwin@321";   // your network password
    int status = WL_IDLE_STATUS;
    WiFiClient  client;

unsigned long myChannel =  2279201;
const char * myReadAPIKey = "READKEY";
const char * myWriteAPIKey = "B1Q2J7GA55WKK7A1";

#define DHTPIN 1   
#define DHTTYPE DHT11
DHT dht(DHTPIN,DHTTYPE);

void setup() {
  // put your setup code here, to run once:
 Serial.begin(9600);
WiFi.begin(ssid, pass);
 ThingSpeak.begin(client);
  delay(10000);

}

void loop() {
 printWifiStatus();
    Serial.println("Reading Sensor Data... ");

  float h = dht.readHumidity();
  // Read temperature as Celsius (the default)
  float t = dht.readTemperature();
  // Read temperature as Fahrenheit (isFahrenheit = true)
  float f = dht.readTemperature(true);
    float water = ThingSpeak.readFloatField(myChannel, 4);

   delay(100);

    Serial.print("Writing to ThingSpeak... ");
    ThingSpeak.setField(1, (float)h);
    ThingSpeak.setField(2, (float)t);
    ThingSpeak.setField(3, (float)f);
    Serial.print("... ");
    ThingSpeak.writeFields(myChannel, myWriteAPIKey);
     delay(5000);

  Serial.print("Current temp is: "); 
  Serial.print(f);
  Serial.print(" *F and ");  
  Serial.print(t);
  Serial.println(" *C"); 
  Serial.print("Humidity: "); 
  Serial.print(h);
  Serial.println("% "); 
  Serial.print("Water Signal: "); 
  Serial.print(water);
  Serial.println(" seconds");

  Serial.println("Function complete, time to rest.");
  delay(10000); // Hibernate [To Do]

}

void printWifiStatus() {
  Serial.print("SSID: ");
  Serial.println(WiFi.SSID());
  IPAddress ip = WiFi.localIP();
  Serial.print("IP Address: ");
  Serial.println(ip);
  long rssi = WiFi.RSSI();
  Serial.print("signal strength (RSSI):");
  Serial.print(rssi);
  Serial.println(" dBm");
}
