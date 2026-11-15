#include <WiFi.h>
#include <PubSubClient.h>

WiFiClient espClient;
PubSubClient client(espClient);

const char* ssid = "PPV TECH";
const char* password = "ppv@12345678";
const char* mqtt_server = "198.38.81.119";

String Master_ID = "3002850";
String MESSAGE = " ";
String msg = "";
String MQTT_SEND = "";

unsigned long previousMillis_15s = 0;
unsigned long previousMillis_10s = 0;
const long interval_15s = 15000; // 15 seconds
const long interval_10s = 10000; // 10 seconds

void setup() {
  Serial.begin(9600);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
  }

  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);
}

void callback(char* topic, byte* payload, unsigned int length) {
  msg = "";
  for (unsigned int i = 0; i < length; i++) {
    msg += (char)payload[i];
  }
  Serial.println(msg);
}

void reconnect() {
  while (WiFi.status() != WL_CONNECTED) {
    WiFi.begin(ssid, password);
    delay(500);
  }
  while (!client.connected()) {
    String clientId = "Cenmark_Valve_Control-";
    clientId += String(random(0xffff), HEX);
    if (client.connect(clientId.c_str())) {
      client.subscribe("send");
      MESSAGE = "SYSTEM ON > ID : " + Master_ID;
      client.publish("receive", MESSAGE.c_str());
    } else {
      delay(5000);
    }
  }
}

void loop() {
  if (!client.connected()) {
    reconnect();
  }
  client.loop();

  unsigned long currentMillis = millis();

  // Send first MQTT message every 15 seconds
  if (currentMillis - previousMillis_15s >= interval_15s) {
    previousMillis_15s = currentMillis;
    MQTT_SEND = "$11*" + Master_ID + "*9345896818*" + "868651062914487*" + "1*" + "IND*" + "Air*" + "1*" + "Cenmark_PPV*" + "RX_Valve_test#";
    client.publish("receive", MQTT_SEND.c_str());
  }

  // Send second MQTT message every 10 seconds
  if (currentMillis - previousMillis_10s >= interval_10s) {
    previousMillis_10s = currentMillis;
    MQTT_SEND = "$11*" + Master_ID + "*89*" + "1*" + "9*" + "0*" + "433MHZ*";
    client.publish("receive", MQTT_SEND.c_str());
  }
}
