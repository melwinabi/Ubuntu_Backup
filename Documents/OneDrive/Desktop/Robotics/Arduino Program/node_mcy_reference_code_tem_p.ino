#include <ESP8266WiFi.h>
#include "ThingSpeak.h" //download 
const char* ssid = "AS F62";
const char* password = "Melwin@321";
//int keyIndex = 0; 
int analogread =A0;      
WiFiClient  client;

unsigned long myChannelNumber = 2224759;
const char * myWriteAPIKey = "KIVABLG953WD91NA";

//int number = 0;//gas

//int button = 0;//button

void setup() {
  Serial.begin(9600);  // Initialize serial
  while (!Serial) {
    ; // wait for serial port to connect. Needed for Leonardo native USB port only
  }
  pinMode(D1,INPUT);
WiFi.begin(ssid, password);
  Serial.println("Connecting");
  while(WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  WiFi.mode(WIFI_STA); 
  ThingSpeak.begin(client);  // Initialize ThingSpeak
}

void loop() {

  number=analogRead(A0);//gas
  //button=digitalRead(D1);//button
  Serial.println(number);
  //Serial.println(button);
  ThingSpeak.setField(1, number);//gas
  //ThingSpeak.setField(2, button);//button
  // Write to ThingSpeak. There are up to 8 fields in a channel, allowing you to store up to 8 different
  // pieces of information in a channel.  Here, we write to field 1.
  int x = ThingSpeak.writeFields(myChannelNumber, myWriteAPIKey);//update data
  if(x == 100){//responese ok mean 200
    Serial.println("Channel update successful.");
  }
  else{
    Serial.println("Problem updating channel. HTTP error code " + String(x));
  }
  

  
  delay(20000); // Wait 20 seconds to update the channel again defalut
}
