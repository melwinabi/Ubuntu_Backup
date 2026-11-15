#include <WiFi.h>
#include <PubSubClient.h>
#define RXD2 16  // GPIO16 -> Serial2 RX
#define TXD2 17  // GPIO17 -> Serial2 TX

const char* ssid = "PPV";
const char* password = "ppv@12345678";
const char* mqtt_server = "147.182.245.143";

const int led = 2;
const int Master_ID = 3002852;
unsigned long startTime = 0;
String Publish = String(Master_ID) + "_PUB";
String Subscribe = String(Master_ID) + "_SUC";
String serialInput = "";
String serial2Input = "";

String message = "";
String Lastmsg = "";
int id_in = 0;

WiFiClient espClient;
PubSubClient client(espClient);

void setup() {
  Serial.begin(115200);
  while (!Serial); 
  Serial.println("ESP32 Serial Communication Example");
  Serial2.begin(9600, SERIAL_8N1, RXD2, TXD2);
  Serial.println("Serial2 started at 9600 baud");
  pinMode(led, OUTPUT);
  WiFi.begin(ssid, password);
  Serial.print("Connecting to WiFi");
  while (WiFi.status() != WL_CONNECTED) 
  {
    digitalWrite(led, HIGH);
    delay(300);
    digitalWrite(led, LOW);
    delay(300);
    Serial.print(".");
  }
  Serial.println("\nWiFi connected.");

  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);
}

void loop() 
{ 
  if (Serial2.available() > 0) {
    String input = Serial2.readStringUntil('\n');
    Serial.println("Received from Serial2: ");
    input.trim();
    if (input.startsWith("##*")) 
    {
      Serial.println(input);
      client.publish(Publish.c_str(),input.c_str());
    }
  }
  if (!client.connected()) {
    reconnect();
  }
  if (millis() - startTime >= 20000)
  {
   startTime=millis();
   String avl = "##*AVL*3002852*010";
   client.publish(Publish.c_str(),avl.c_str());
   delay(10);
   client.publish(Publish.c_str(), buildMfO().c_str());
  }  
  client.loop();
}

void callback(char* topic, byte* payload, unsigned int length) {
  String msg = "";
  for (unsigned int i = 0; i < length; i++) {
    msg += (char)payload[i];
  }
  msg.trim();
  Separator(msg);
}

void reconnect() {
  while (WiFi.status() != WL_CONNECTED) {
    Serial.println("Reconnecting WiFi...");
    WiFi.begin(ssid, password);
    delay(1000);
  }

  while (!client.connected()) {
    String clientId = "ESP32Client-" + String(random(0xffff), HEX);
    Serial.println("Attempting MQTT connection...");
    if (client.connect(clientId.c_str())) {
      Serial.println("MQTT connected.");
      client.publish(Publish.c_str(), ("Master>" + String(Master_ID) + "*Online").c_str());
      client.subscribe(Subscribe.c_str());
      client.publish(Publish.c_str(), buildMfO().c_str());
    } else {
      Serial.print("MQTT failed, rc=");
      Serial.print(client.state());
      delay(5000);
    }
  }
}

String buildMfO() {
  String header     = "WMfO";
  String masterID   = "3002852";
  String imsi       = "404940968606519";
  String iccid      = "8991000920984748682F";
  String network    = "IND airtel";
  String mcc        = "404";
  String mnc        = "40";
  String netType    = "5";
  String tac        = "42139";
  String cellID     = "155244594";
  String signal     = "7";
  String latitude   = "00.0000000";
  String longitude  = "11.1111111";
  String ldr        = "1";

  String dataToSend = "##*" + header + "*" + masterID + "*" + imsi + "*" + iccid + "*" +
                      network + "*" + mcc + "*" + mnc + "*" + netType + "*" + tac + "*" +
                      cellID + "*" + signal + "*" + latitude + "*" + longitude + "*" +
                      ldr + "*$$";
  delay(100);                    
  return dataToSend;            
  
}

void Separator(String value) {
  if (value.startsWith("@@*") || value.startsWith("&&*")) {
    message = "##*" + String(Master_ID) + "*C@K";
    client.publish(Publish.c_str(), value.c_str());
    Serial2.println(value);
    Serial.println("Received MQTT: " + value);
    delay(100);
  }
  else
  {
    
  }
}
