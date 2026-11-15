#include "ThingSpeak.h"

#include<ESP8266WiFi.h>
  


char ssid[] = "MelwinAbi";    //  your network SSID (name) 
char pass[] = "Melwin@321";   // your network password
   
WiFiClient  client;

unsigned long myChannel = 2285197;
const char * myReadAPIKey = "WAMAXS9197V4IU4C";
int statusCode = 0;






void setup() {
  // put your setup code here, to run once:
 Serial.begin(9600);
 WiFi.begin(ssid, pass);
 ThingSpeak.begin(client);

  pinMode(D4,OUTPUT);
    delay(10000);

}

void loop() {
 printWifiStatus();
    Serial.println("loading........ ");
    statusCode = ThingSpeak.readMultipleFields(myChannel, myReadAPIKey) ;
    
    if(statusCode == 200)
    {
      // Fetch the stored data

      int LED = ThingSpeak.getFieldAsInt(1); // Field 1
      //int LED2 = ThingSpeak.getFieldAsInt(2); // Field 2
      //int LED3 = ThingSpeak.getFieldAsInt(3); // Field 3......8
      Serial.println(LED);  
      if(LED==1)
      {
        digitalWrite(D4,HIGH);
      }
     if(LED==0)
      {
        digitalWrite(D4,LOW);
      }
    }
    else{
      Serial.println("Problem reading channel. HTTP error code " + String(statusCode)); 
    }
    
 Serial.println();
    delay(1000);
 
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
