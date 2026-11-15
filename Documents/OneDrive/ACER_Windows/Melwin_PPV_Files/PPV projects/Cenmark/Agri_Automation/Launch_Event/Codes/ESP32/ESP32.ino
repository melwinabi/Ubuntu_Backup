#include <WiFi.h>
#include <PubSubClient.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>

const char* ssid = "PPV TECH";
const char* password = "ppv@12345678";
const char* mqtt_server = "198.38.81.119";

const int led = 2;
const int Master_ID = 3002852;

String message = "";
String Lastmsg = "";
int id_in = 0;

WiFiClient espClient;
PubSubClient client(espClient);

void Separator(String msg) {
  if (msg.startsWith("@@*")) {
    message = "##*" + String(Master_ID) + "*C@K";
    client.publish("RX_Valve_test", message.c_str());
    Serial.println(message);
  } else if (msg.startsWith("##*")) {
    message = msg;
    client.publish("RX_Valve_test", message.c_str());
    Serial.println(message);
  }
}

void callback(char* topic, byte* payload, unsigned int length) {
  String msg = "";
  for (unsigned int i = 0; i < length; i++) {
    msg += (char)payload[i];
  }

  Serial.println("Received MQTT: " + msg);
  digitalWrite(led, HIGH);
  delay(500);
  digitalWrite(led, LOW);
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
      client.publish("RX_Valve_test", ("Master > " + String(Master_ID) + " Online").c_str());
      client.subscribe("Cenmark_PPV");
      client.publish("RX_Valve_test", buildMfO().c_str());
    } else {
      Serial.print("MQTT failed, rc=");
      Serial.print(client.state());
      delay(5000);
    }
  }
}

String buildMfO() {
  // Dummy values for operator/network, can replace with real ESP32 network info if needed
  String op1 = "JIO";
  String op2 = "4G";
  String op3 = "IND";
  String net1 = "OK";
  String net2 = "Signal";
  String net3 = "Good";
  String net4 = "ON";

  return "##*MfO*" + String(Master_ID) + "*" + "NA_IMSI" + "*" + "NA_ICCID" + "*" +
         op1 + "*" + op2 + "*" + op3 + "*" + net1 + "*" + net2 + "*" + net3 + "*" + net4 + "*$$";
}

void setup() {
  Serial.begin(115200);
  Serial2.begin(115200);
  pinMode(led, OUTPUT);
  WiFi.begin(ssid, password);
  Serial.print("Connecting to WiFi");
  while (WiFi.status() != WL_CONNECTED) {
    digitalWrite(led, HIGH);
    delay(300);
    digitalWrite(led, LOW);
    delay(300);
    Serial.print(".");
  }
  Serial.println("\nWiFi connected.");

  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);

  HTTPClient http;
  http.begin("https://ppvgroups.com/node.php?count=10");
  int httpCode = http.GET();

  if (httpCode > 0) {
    String payload = http.getString();
    StaticJsonDocument<1024> jsonBuffer;
    DeserializationError error = deserializeJson(jsonBuffer, payload);
    if (!error) {
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
      Serial.println("#");
    } else {
      Serial.println("JSON parse error.");
    }
  } else {
    Serial.println("HTTP GET failed.");
  }
  http.end();
}

void loop() {
  if (!client.connected()) {
    reconnect();
  }
  client.loop();

  if (Serial2.available() > 0) {
    String incomingMsg = Serial.readStringUntil('\n');
    incomingMsg.trim();
    if (incomingMsg != Lastmsg) {
      if (incomingMsg.startsWith("Ack.") || incomingMsg.startsWith("Bt.") || incomingMsg.startsWith("Sg.")) {
        client.publish("RX_Valve_test", incomingMsg.c_str());
      } else if (incomingMsg.startsWith("@@*") || incomingMsg.startsWith("##*")) {
        Separator(incomingMsg);
      }
      Lastmsg = incomingMsg;
    }
  }
}
