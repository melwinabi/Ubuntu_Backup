#include <EEPROM.h>
#include "BluetoothSerial.h"
#include <WiFi.h>
#include <HTTPClient.h>

#define RXD2 16
#define TXD2 17

BluetoothSerial bt;

int root = 0;
int k = 0;
int Update_Status = 2;
int Wifi_Status = 0;
int GSM_Status = 0;
int Flag_1 = 0;
int Flag_2 = 0;
int Buffer_gsm, Buffer_wifi, Buffer_update;
const char* System_ID = "0";
unsigned long lastSIMCheckTime = 0;
const unsigned long SIMCheckInterval = 10000;  // 10 seconds
unsigned long  Current_millis;
unsigned long Previous_millis;

String Blt_Name = "";
String stored_data = "";
String ssid = "";
String password = "";
String ciicrResponse;
String Str_gsm;
String Str_wifi;
String Str_update;

void setup() 
{
  Serial.begin(115200); 
  Serial2.begin(115200, SERIAL_8N1, RXD2, TXD2); 
  Blt_Name = "Scalex_";
  Blt_Name += System_ID;  
  bt.begin(Blt_Name);
  delay(1000);  
  EEPROM.begin(512);
  int Data_Len = EEPROM.read(0);  
  for (int i = 0; i < Data_Len; i++)
  stored_data += char(EEPROM.read(i + 3));  
  delay(1000);
  int index = stored_data.indexOf('*');
  String SSID = stored_data.substring(0, index);
  String PASS = stored_data.substring(index + 1); 
  WiFi.begin(SSID,PASS);
  while (WiFi.status() != WL_CONNECTED) 
  {
    delay(1000);
    k++;
    if (k > 10)
    {
      root = 2;
      break;
    }
  }

  if (WiFi.status() == WL_CONNECTED) 
  {
    Wifi_Status = 1; 
    delay(1000);
  } 
  else
  {
    Wifi_Status = 0;
    sendCommand("AT", 100);
    sendCommand("ATE1", 100);
    sendCommand("AT+CPIN?", 100);
    sendCommand("AT+CIPSHUT", 100);
    sendCommand("AT+CGATT=1", 100);
    sendCommand("AT+CSTT=\"airtelgprs.com\",\"\",\"\"", 200);
    ciicrResponse = sendCommand("AT+CIICR", 300);
    if (ciicrResponse.indexOf("OK") == -1) 
    {
      GSM_Status = 0;
    } else {
      GSM_Status = 1;
    }
    sendCommand("AT+CIFSR", 200);
    sendCommand("AT+SAPBR=3,1,\"CONTYPE\",\"GPRS\"", 100);
    sendCommand("AT+SAPBR=1,1", 300);
    sendCommand("AT+SAPBR=2,1", 100);
    sendCommand("AT+HTTPINIT", 200);
    sendCommand("AT+HTTPPARA=\"CID\",1", 100);    
  }
}  

void loop() 
{
  Current_millis = millis();
  if(Current_millis - Previous_millis >= 3000)
  {
    Previous_millis = Current_millis;
    if(GSM_Status == 0)
    {
      Str_gsm = "Disconnected";
      Buffer_gsm = 0;
    }
    else
    {
      Str_gsm = "Connected";
      Buffer_gsm = 1;
    }
    if (Wifi_Status == 0 )
    {
      Str_wifi = "Disconnected";
      Buffer_wifi = 0;
    } 
    else
    {
      Str_wifi = "Connected";
      Buffer_wifi = 1;
    }    
    if (Update_Status == 0 )
    {
      Str_update = "Failed";
      Buffer_update = 0;
    }
    else if(Update_Status == 2)
    {
      Str_update = "Processing_Data";
      Buffer_update = 0;
    }
    else
    {
      Str_update = "Success";
      Buffer_update = 1;
    }  
    String Bluetooth = " :: GSM_Status >> " + String(Str_gsm) + " :: WiFi_Status >> " + String(Str_wifi) +  " :: UPDATE_Status >> "+ String(Str_update); 
    //Serial.println(Buffer); 
    bt.println(Bluetooth);
    delay(10);
    if ((Buffer_wifi == 0 && Buffer_gsm == 0) || Buffer_update == 0)
    {
      Serial.println("Slow_Blynk");
    }
    else
    {
      Serial.println("Fast_Blynk");
    }
  }
  if(Flag_1 > 5 || Flag_2 > 5)
  {
    bt.println("Restarting...");
    delay(1000);
    ESP.restart();
  }  
  if (Serial.available()) 
  {
    String Read = Serial.readStringUntil('\n');
    Read.trim();

    if (Read.length() > 0)
    {
      // bt.println(Read);
      // Serial.print("DATA : ");
      // Serial.println(Read);

      if (Read.startsWith("http:")) 
      {
      //bt.println("Read Ok");
      if (WiFi.status() == WL_CONNECTED) {
          HTTPClient http;
          http.begin(Read);  
          Read="";
          int httpCode = http.GET();  

          if (httpCode > 0) {
             // bt.print("HTTP Response Code: ");
             // bt.println(httpCode);

              String payload = http.getString();
             // bt.println("Payload: " + payload);

              if(payload.indexOf("\"success\": true") != -1) {
                  Update_Status = 1;
                  Flag_1 = 0;
                  Wifi_Status = 1;
              }
              else {
                  Update_Status = 0;
                  Flag_1++;
              }
          }
          else {
              Serial.print("HTTP GET failed, error: ");
              Serial.println(http.errorToString(httpCode).c_str());
              Update_Status = 0;
              Flag_1++;
          }
          http.end();
      }

        else 
        {
          Wifi_Status = 0;
          sendURL(Read);  
          Read="";
          if (millis() - lastSIMCheckTime >= SIMCheckInterval) {
            lastSIMCheckTime = millis();

            String simStatus = sendCommand("AT+CREG?", 200);
            // CREG? returns +CREG: <n>,<stat> — we check if <stat> is 1 (home) or 5 (roaming)
            if (simStatus.indexOf("+CREG:") != -1) {
              int index = simStatus.indexOf(",");
              if (index != -1 && index + 1 < simStatus.length()) {
                char stat = simStatus.charAt(index + 1);
                if (stat == '1' || stat == '5') 
                {
                  Flag_2 = 0;
                  GSM_Status = 1; 
                } 
                else 
                {
                  Flag_2++;
                  GSM_Status = 0; 
                }
              } 
              else 
              {
                Flag_2++;
                GSM_Status = 0;
              }
            } 
            else 
            {
              Flag_2++;
              GSM_Status = 0;  
            }
          }          
        }
      }
    }
  }
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


void sendURL(String url) {
  String httpCmd = "AT+HTTPPARA=\"URL\",\"" + url + "\"";
  sendCommand(httpCmd.c_str(), 3000);
  sendCommand("AT+HTTPACTION=0", 3000); 
   Serial2.println("AT+HTTPREAD");
  String response = "";
  long timeout = millis() + 3000;
  while (millis() < timeout) {
    while (Serial2.available()) {
      response += Serial2.readString();
    }
  }
  if (response.indexOf("\"success\": true") != -1) {
    Update_Status = 1;
    Flag_1 = 0;
  } 
  else 
  {
    Flag_1 ++;
    Update_Status = 0;
  }
}

String sendCommand(const char* cmd, int delayMs) 
{
  Serial2.println(cmd);
  delay(delayMs);
  String res = "";
  while (Serial2.available()) 
  {
    res += Serial2.readString();
  }
  //Serial.print("response >> ");
  //Serial.println(res);
  return res;
}
