#include <ESP8266WiFi.h>
  
const char* ssid     = "TRASH01";
const char* password = "12345678";

const char* host = "api.thingspeak.com";
const char* streamId   = "....................";
const char* privateKey = "....................";

void serialEvent();
String inputString = "";         // a string to hold incoming data
boolean stringComplete = false;  // whether the string is complete

void setup() {
  Serial.begin(9600);
  delay(10);
  Serial.println();
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

  
  WiFi.begin(ssid, password);
  
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi connected");  
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}

int value = 0;

void loop() {
      serialEvent();
      Serial.print("connecting to ");
      Serial.println(host);

      
      WiFiClient client;
      const int httpPort = 80;
      if (!client.connect(host, httpPort))
      {
        Serial.println("connection failed");
        return;
      }
      
      Serial.print("Requesting URL: ");
      Serial.println(inputString);
      
      client.print(String("GET ") + inputString + " HTTP/1.1\r\n" +
                   "Host: " + host + "\r\n" + 
                   "Connection: close\r\n\r\n");
                   
      unsigned long timeout = millis();
      while (client.available() == 0) 
      {
        if (millis() - timeout > 5000) 
        {
          Serial.println(">>> Client Timeout !");
          client.stop();
          return;
        }
      }
      while(client.available())
      {
        String line = client.readStringUntil('\r');
        Serial.print(line);
      }
      Serial.println();
      Serial.println("closing connection");
      inputString="";

    
}


void serialEvent()
{   
  while(Serial.available()) 
  {
      inputString = Serial.readString();;
     // delay(1000);
  }
}
