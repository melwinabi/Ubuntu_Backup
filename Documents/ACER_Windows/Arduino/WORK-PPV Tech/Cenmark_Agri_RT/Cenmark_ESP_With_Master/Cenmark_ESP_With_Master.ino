#include <WiFi.h>
#include <PubSubClient.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>
int led = 2 ;
const char* ssid = "Caption_miller_03";
const char* password = "Melwin@123";
const char* mqtt_server = "broker.hivemq.com";
int  id_in=0;
String message = "";
int check = 0;
WiFiClient espClient;
PubSubClient client(espClient);

void setup() {
  Serial.begin(115200);
  pinMode(led,OUTPUT);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    digitalWrite(led,HIGH); 
    delay(1000);
  }
  digitalWrite(led,HIGH);
  delay(500);
  digitalWrite(led,LOW);
  delay(500);
  digitalWrite(led,HIGH);
  delay(500);
  digitalWrite(led,LOW);
  delay(500);  
  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);
  HTTPClient http;
  http.begin("https://ppvgroups.com/node.php");
  int httpCode = http.GET();

  if (httpCode > 0) { // Check if the GET request was successful
    String payload = http.getString();
    StaticJsonDocument<1024> jsonBuffer; // Adjust size as needed
    DeserializationError error = deserializeJson(jsonBuffer, payload);
    if (error) {
      //Serial.print(F("deserializeJson() failed: "));
      //Serial.println(error.f_str());
      return;
    }
    JsonObject root = jsonBuffer.as<JsonObject>();
    Serial.print("*");
    for (JsonPair kv : root) {
      const char* key = kv.key().c_str();
      
      if (String(key).startsWith("id_")) {
        const char* sensor = kv.value();
        id_in++;
        Serial.print(sensor);
        
      }
      
    }
      Serial.print("*");
      Serial.print("$");
      Serial.print(id_in);
      Serial.print("#");
  } else {
  }
  http.end(); 
}

void callback(char* topic, byte* payload, unsigned int length)
{
  for (int i = 0; i < length; i++) 
  {
    message += (char)payload[i];
  }
    Serial.println(message);
    message="";
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
        client.publish("RX_Valve_test", "SV Notification");
        client.subscribe("Cenmark_PPV");
      } 
      else 
      {
        check++;
        delay(1000);
      }
    }
  }
}

void loop() {

}
