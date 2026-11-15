#include <EEPROM.h>
#include "BluetoothSerial.h"
#include <WiFi.h>
#include <HTTPClient.h>


#define RXD2 16
#define TXD2 17
#define LED 19

BluetoothSerial bt;

//...........................................................................................................................................................
const char* System_ID = "10002";
//...........................................................................................................................................................

String serverName = "https://beta.plastocore.in/api/Weigh/ReadWeight?rfID=0&branchID=100&machineID=1002";

String serverPath;
String PASS = "";
String SSID = "";
String readString;

int StringLocation_weight;
int StringLocation_id;

String weight;
String id;
String Blt_Name = "";
String stored_data = "";

unsigned long lastTime = 0;
unsigned long timerDelay = 5000;

int d = 0;
int f = 0;
int rst = 0;
void SendData();

void setup() 
{
  Serial.begin(115200); 
  Serial2.begin(115200, SERIAL_8N1, RXD2, TXD2);
  Blt_Name = "Cenmark_";
  Blt_Name += System_ID; 
  delay(1000);  
  EEPROM.begin(512);   
  bt.begin(Blt_Name);
  int Data_Len = EEPROM.read(0);  
  for (int i = 0; i < Data_Len; i++)
  stored_data += char(EEPROM.read(i + 3));  
  delay(1000);
  int index = stored_data.indexOf('*');
  SSID = stored_data.substring(0, index);
  PASS = stored_data.substring(index + 1); 
  Serial.println(SSID);
  Serial.println(PASS);
  WiFi.begin(SSID,PASS);  
  pinMode(LED, OUTPUT);
  for(int m = 0; m > 10; m++)
  {
    digitalWrite(LED, HIGH);
    delay(100);
    digitalWrite(LED, LOW);
    delay(100);
  }

  WiFi.begin(SSID, PASS);
  delay(3000);
  if (WiFi.status() == WL_CONNECTED) 
  {
    delay(1000);
    bt.println("System Connected to WiFi Sucessfully");
    Serial.println("System Connected to WiFi Sucessfully");
  } 
  else
  {
     bt.println("System Failed to Connected");
     Serial.println("System Failed to Connected");
  }
}

void loop() 
{
  if (bt.available()) 
  {
    String Bt_data = bt.readStringUntil('\n');
    Bt_data.trim();

    if (Bt_data.startsWith("&&*"))
    {
      String S_PS = Bt_data.substring(3);
      S_PS.trim();
      int len = S_PS.length();  
      EEPROM.write(0, len);  
      for (int j = 0; j < len; j++) 
      {
        EEPROM.write(j + 3, S_PS[j]);  
      }
      EEPROM.commit();    
      bt.println("SYSTEM RESTART");
      delay(1000);
      ESP.restart();
    }
  }
  if (Serial2.available() >0) 
  {
    char c = Serial2.read();
    if (c == '*') 
    {
      StringLocation_weight = readString.indexOf(',');
      weight = readString.substring(0, StringLocation_weight);
      StringLocation_id = readString.indexOf(',',StringLocation_weight+1);
      id = readString.substring(StringLocation_weight+1, StringLocation_id);
      d = 1;
      weight.trim();
      id.trim();
      serverPath = serverName + id + "&weight1=0&weight2=" + weight;
    }
    else
    {
      readString += c;
    } 
  }
  if(d == 1)
  {
    SendData();
  }
}
void SendData()
{
  if(WiFi.status() == WL_CONNECTED)
  {
    HTTPClient http;
    Serial.println(serverPath);
    digitalWrite(LED, LOW);
    http.begin(serverPath.c_str());
    int httpResponseCode = http.GET();
    readString="";
    if (httpResponseCode>0) 
    {
      Serial.print("HTTP Response code: ");
      Serial.println(httpResponseCode);
      String payload = http.getString();
      int location_1 = payload.indexOf('}');
      String value = payload.substring(12, location_1-1);
      Serial.println(payload);
      if(value == "0")
      {
        Serial2.print("A");
      }
      else if(value == "1")
      {

      }
      digitalWrite(LED, HIGH);
    }
    else 
    {
      Serial.print("Error code: ");
      Serial.println(httpResponseCode);
    }
    if (httpResponseCode == 200)
    {
      d = 0;
      serverPath = "";
      digitalWrite(LED, HIGH);
    }
    else
    {
      f++;
      if( f > 5)
      {
        d = 0;
        ESP.restart();
        digitalWrite(LED, LOW);
      }
      digitalWrite(LED, LOW);
    }
    http.end();
  }
  else 
  {
    Serial.println("WiFi Disconnected");
  }
}
