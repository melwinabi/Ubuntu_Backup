#include <WiFi.h>
#include <PubSubClient.h>
#define RXD2 16
#define TXD2 17

const char* ssid = "Caption_miller_03";
const char* password = "Melwin@123";
const char* mqtt_server = "broker.hivemq.com";
String message = "";
int check = 0;
WiFiClient espClient;
PubSubClient client(espClient);

void setup()
{
  Serial.begin(9600);
  Serial2.begin(9600, SERIAL_8N1, RXD2, TXD2);
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
    message="";
   Serial.println(message);   
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
      //  Serial.println("connected");
        client.publish("RX_Valve_78787878", "SV Notification");
        client.subscribe("Cen_MqTt@9992019"); //topic in mobile
      } 
      else 
      {
       // Serial.print("failed, rc=");
       // Serial.print(client.state());
      //  Serial.println(" try again in 5 seconds");
        check++;
        delay(1000);
      }
    }
  }
}

void loop(){
   if (!client.connected()) 
  {
    reconnect();
  }
  client.loop();
}

//mqtt to ard thrg esp
