#include <EEPROM.h>
#include "BluetoothSerial.h"
#include <WiFi.h>
#include <HTTPClient.h>

#define RXD2 16
#define TXD2 17

BluetoothSerial bt;

const char* System_ID = "100";
String Blt_Name = "";
String stored_data = "";
String ssid     = "";
String password = "";
int root = 0;
int k = 0;

void setup() 
{
  Serial.begin(115200); 
  Serial2.begin(115200, SERIAL_8N1, RXD2, TXD2); 
  EEPROM.begin(512);
  Blt_Name = "Scalex_";
  Blt_Name += System_ID;  
  bt.begin(Blt_Name);  
  int Data_Len = EEPROM.read(0);  
  Serial.print("Stored Length: ");
  Serial.println(Data_Len);
  for (int i = 0; i < Data_Len; i++)
  {
    stored_data += char(EEPROM.read(i + 3));  
  }
  Serial.println("Stored Data: " + stored_data);
  delay(1000);
  int index    = stored_data.indexOf('*');
  String SSID = stored_data.substring(0, index);
  String PASS = stored_data.substring(index + 1);
  Serial.println("Part 1: " + SSID); 
  Serial.println("Part 2: " + PASS); 
  WiFi.begin(SSID, PASS);
  while (WiFi.status() != WL_CONNECTED) 
  {
    delay(1000);
    Serial.print(".");
    k ++;
    if ( k > 10)
    {
      root = 2;
      break;
    }
  }
  Serial.println();
  Serial.println("WiFi connected.");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
}  

void loop() 
{
  if (Serial.available() > 0) 
  {
    String Read = Serial.readStringUntil('\n'); 
    Read.trim(); 
    Serial.println("Serial Received: " + Read);

    if (Read.startsWith("http:"))  // Simple check if it's a URL
    {
      if (WiFi.status() == WL_CONNECTED) 
      {
        HTTPClient http;
        http.begin(Read);  // Pass the full URL
        int httpCode = http.GET();  // Send the request

        if (httpCode > 0) 
        {
          String payload = http.getString();
          //Serial.println("HTTP Response code: " + String(httpCode));
          //Serial.println("Response: " + payload);
        } 
        else 
        {
          //Serial.println("HTTP GET failed, error: " + http.errorToString(httpCode));
        }

        http.end();  // Close connection
      } 
      else 
      {
        Serial.println("Serial2");
        Serial2.println(Read);
      }
    }
    Read = "";
  }
  if (bt.available() > 0) 
  {
    String Bt_data = bt.readStringUntil('\n');
    Bt_data.trim();
    if (Bt_data.startsWith("&&*"))
    {
      String S_PS = Bt_data.substring(3);
      Serial.println("BT Received: " + S_PS); 
      S_PS.trim();
      int len = S_PS.length();  
      EEPROM.write(0, len);  
      for (int j = 0; j < len; j++) 
      {
        EEPROM.write(j + 3, S_PS[j]);  
      }
      EEPROM.commit();    
      Serial.println("Data saved to EEPROM."); 
      bt.println("SYSTEM RESTART");
      delay(1000);
      ESP.restart();
    }
  }
}
