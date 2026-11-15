#include <WiFi.h>
#include <HTTPClient.h>

#define RXD2 16
#define TXD2 17
#define LED 19

const char* ssid = "Cenmark jio123";
const char* password = "Irrigation@260924";

String serverName = "https://beta.plastocore.in/api/Weigh/ReadWeight?rfID=0&branchID=100&machineID=";

String serverPath;

String readString;

int StringLocation_weight;
int StringLocation_id;

String weight;
String id;

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
  pinMode(LED, OUTPUT);

  for(int m = 0; m > 10; m++)
  {
    digitalWrite(LED, HIGH);
    delay(100);
    digitalWrite(LED, LOW);
    delay(100);
  }

  WiFi.begin(ssid, password);
  Serial.println("Connecting");
  while(WiFi.status() != WL_CONNECTED) 
  {
    delay(500);
    Serial.print(".");
    rst++;
    digitalWrite(LED, HIGH);
    delay(100);
    digitalWrite(LED, LOW);
    delay(100);
    if(rst > 10)
    {
      ESP.restart();
    }
  }
  Serial.println("");
  Serial.print("Connected to WiFi network with IP Address: ");
  Serial.println(WiFi.localIP());
  digitalWrite(LED, HIGH);
}

void loop() 
{
  
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
