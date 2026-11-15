#include <WiFi.h>
#include <PubSubClient.h>

#define RXD2 16
#define TXD2 17
#define LED 19

int ledState;
int state1;
int buttonState;
int CurrentState;
int CheckFunction = 0;
String message = "";
String Ack = "";
int check = 0;
int call = 0;
char PassValue[10];
unsigned long debounceDuration = 10000; //milli
unsigned long lastTimeButtonStateChanged = 0;
const char* ssid = "Caption_miller_03";
const char* password = "Melwin@123";
const char* mqtt_server = "broker.hivemq.com";
WiFiClient espClient;
PubSubClient client(espClient);

void setup()
{
  Serial.begin(9600);
  //Serial2.begin(9600, SERIAL_8N1, RXD2, TXD2);
  pinMode(LED, OUTPUT);
  digitalWrite(LED, LOW);
  Serial.print("Begin");
  setup_wifi();
  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);
}

void setup_wifi()
{
  delay(10);
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);
  delay(1000);
  Serial.println("");
  Serial.println("WiFi connected");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
}

void callback(char* topic, byte* payload, unsigned int length)
{
  for (int i = 0; i < length; i++) 
  {
    message += (char)payload[i];
  }  
  if 
   (message == "010" || message == "011" || message == "020" || 
    message == "021" || message == "030" || message == "031" || 
    message == "040" || message == "041" || message == "050" ||
    message == "051" || message == "000" || message == "901" ||
    message == "902") 
  {
    Serial.println("Send Msg : "+message);
    //Serial2.print(message);
    message = "";
    delay(500);
  }  
}

void reconnect(){
  if(WiFi.status() != WL_CONNECTED)
  {
    WiFi.begin(ssid, password);
    delay(500);
  }

  if(WiFi.status() == WL_CONNECTED)
  {
    while (!client.connected()) 
    {
      Serial.print("Attempting MQTT connection...");
      String clientId = "ESP32Client-";
      clientId += String(random(0xffff), HEX);
      if (client.connect(clientId.c_str())) 
      {
        Serial.println("connected");
        digitalWrite(LED, HIGH);
        client.publish("SV_RXD", "SV Notification");
        client.subscribe("SV_TXD/#");
      } 
      else 
      {
        Serial.print("failed, rc=");
        Serial.print(client.state());
        Serial.println(" try again in 5 seconds");
        check++;
        delay(1000);
      }
    }
  }
}

void loop() 
{ 
  if (!client.connected()) 
  {
    check++;
    Serial.println("Retry : "+check);
    delay(500);
    reconnect();
  }
  client.loop();
  if (Serial.available()>0) 
  {
    char c = Serial.read();
    if (c == '*')
    {
      call = 1;
    }
    else if(c == '\n')
    {
    }
    else
    {
      Ack += c;
    } 
  }
  if(call == 1)
  {
    Ack.trim();
    Serial.println("Ack :"+Ack);
    Ack.toCharArray(PassValue, 4);                                                       
    client.publish("SV_RXD", PassValue);
    
    Ack = "";
    call = 0;  
  }
  if(check > 5)
  {
    Serial.println("ESP Reseting");
    ESP.restart();
  }
}
//working
