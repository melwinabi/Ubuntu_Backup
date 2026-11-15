#include<ESP8266WiFi.h> // calling esp8266 wifi library no need any other wifi library while using node mcu
#define DHTPIN 2 

char ssid[] = "MelwinAbi";    //  your network SSID (name) 
char pass[] = "Melwin@321";   // your network password

WiFiClient  client; 

unsigned long myChannel =  000000;
const char * myReadAPIKey = "xyxyxyxy";
const char * myWriteAPIKey = "hegfuh4683hdgu3k";


           setup


           
  WiFi.begin(ssid, pass);  
    ThingSpeak.begin(client);


           loop


           
   printWifiStatus();


      Serial.print("Writing to ThingSpeak... ");
   // ThingSpeak.setField(1, (float)stored varriable names example : h);
   //ThingSpeak.setField(2, (float)"""""""""""""""""""""""""""""""""t);
   // ThingSpeak.setField(3, (float)"""""""""""""""""""""""""""""""""f);
    Serial.print("... ");
    ThingSpeak.writeFields(myChannel, myWriteAPIKey);
    delay(5000);






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
