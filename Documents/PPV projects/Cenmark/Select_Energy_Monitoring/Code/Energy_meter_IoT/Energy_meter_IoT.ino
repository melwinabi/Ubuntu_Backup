#include <EEPROM.h>
#include "BluetoothSerial.h"
#include <WiFi.h>
#include <HTTPClient.h>

BluetoothSerial bt;
// Fixed WiFi credentials
const char* ssid ="";// "PPV TECH";
const char* password ="";//"ppv@12345678";

const char* System_ID = "10003";

int Update_Status = 2;
int Wifi_Status = 0;
int Flag_1 = 0;
int Flag_2 = 0;
int f = 0;
String Blt_Name = "";
String stored_data = "";
unsigned long previousMillis_TWO = 0;
const unsigned long interval_TWO = 60000; // 2 seconds

String urlBase = "https://api.smartpruner.in/api/DataInsert/SaveDataInTable?SerialNo=";
float AV = 0, AI = 0, AP = 0, KW = 0, HW = 0;

void setup() {
  Serial.begin(115200); 
  Blt_Name = "Cenmark_";
  Blt_Name += System_ID; 
  delay(500);  
  bt.begin(Blt_Name);
  delay(1000);  
  EEPROM.begin(512);
  int Data_Len = EEPROM.read(10);  
  for (int i = 0; i < Data_Len; i++)
    stored_data += char(EEPROM.read(i + 3));  
  delay(1000);
  int index = stored_data.indexOf('*');
  String SSID = stored_data.substring(0, index);
  String PASS = stored_data.substring(index + 1); 
  WiFi.begin(SSID, PASS);
  WiFi.begin(ssid, password);
  Serial.println("Connecting to WiFi...");
  int rst = 0;
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
    btc_func();
  }
    bt.end(); 
  //Serial.println("\nWiFi Connected ✅");
  Serial.println(WiFi.localIP());
  Wifi_Status = 1;

}

void loop() {
  // read serial lines like "AV: 0.00"
  if (Serial.available()) {
    String Read = Serial.readStringUntil('\n');
    Read.trim();

    if (Read.startsWith("AV:")) {
      AV = Read.substring(3).toFloat();
      //Serial.print("IN_AV = "); Serial.println(AV);
    } else if (Read.startsWith("AI:")) {
      AI = Read.substring(3).toFloat();
      //Serial.print("IN_AI = "); Serial.println(AI);
    } else if (Read.startsWith("AP:")) {
      AP = Read.substring(3).toFloat();
      //Serial.print("IN_AP = "); Serial.println(AP);
    } else if (Read.startsWith("KW:")) {
      KW = Read.substring(3).toFloat();
      //Serial.print("IN_KW = "); Serial.println(KW);
    } else if (Read.startsWith("HW:")) {
         = Read.substring(3).toFloat();
      //Serial.print("IN_HW = "); Serial.println(HW);
    }
  }

  unsigned long currentMillis_TWO = millis();
  if (currentMillis_TWO - previousMillis_TWO >= interval_TWO) {
    previousMillis_TWO = currentMillis_TWO;

    // Combine data for upload
    String values =  String(AV, 2) + "*v2=" + String(AI, 2) + "*v3=" + String(AP, 2) + "*v4=" + String(KW, 2) + "*v5=" + String(HW, 2);
    String fullURL = urlBase + System_ID + "&values=v1=" + values;

    Serial.println("Sending to server:");
    Serial.println(fullURL);

    if (WiFi.status() == WL_CONNECTED) {
      HTTPClient http;
      http.begin(fullURL.c_str());
      int httpCode = http.GET();

      if (httpCode > 0) {
        String payload = http.getString();
        Serial.println("Server Response: " + payload);

        if (payload.indexOf("\"success\":true") != -1) {
          Update_Status = 1;
          Flag_1 = 0;
          Wifi_Status = 1;
          //Serial.println("Upload Success ✅");
        } else {
          Update_Status = 0;
          Flag_1++;
          Serial.println("Upload Failed ⚠️");
        }
      } else {
         Serial.print("HTTP GET failed, code: ");
         Serial.println(httpCode);
        Update_Status = 0;
        Flag_1++;
      }

      http.end();
    } else {
      Serial.println("WiFi Disconnected ⚠️");
      Wifi_Status = 0;
      f++;
      if (f > 5) ESP.restart();
    }
  }

  if (Flag_2 > 5) {
     Serial.println("Restarting...");
    delay(1000);
    ESP.restart();
  }
}

void btc_func()
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
}
//https://beta.plastocore.in/api/Weigh/Data?deviceid=10001&v1=21.67&v2=11.23&v3=12.37&v4=11.26&v5=98.23
//https://beta.plastocore.in/api/Weigh/ReadTestValue?SerialNo=10001&values=10.23~10.24~10.65~19.18