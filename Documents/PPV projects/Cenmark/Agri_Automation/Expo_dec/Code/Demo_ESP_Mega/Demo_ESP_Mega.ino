#include <WiFi.h>
#include <HTTPClient.h>
#include <PubSubClient.h>
#include <ArduinoJson.h>

#define RXD2 17
#define TXD2 16   // required by Serial2.begin

// ---------------- CONFIG ----------------
const char* WIFI_SSID = "Cen_Agri";
const char* WIFI_PASS = "Cen@12345678";

int Master_ID = 3002853;
String Publish_Master;
String Subscribe_Master;

String mqtt_ip = "mqtt.ppvgroups.in";
int mqtt_port = 1883;

WiFiClient espClient;
PubSubClient mqttClient(espClient);

// ---------------- VARIABLES ----------------
String rxData = "";
bool Scale = false;

unsigned long lastHTTP = 0;
unsigned long lastMasterInfo = 0;

// ---------------- MQTT CALLBACK ----------------
void mqttCallback(char* topic, byte* payload, unsigned int length) {
  String msg = "";
  for (uint8_t i = 0; i < length; i++)
    msg += (char)payload[i];

  Serial.println("MQTT RX: " + msg);

  if (msg.startsWith("@@*") || msg.startsWith("&&*")) {
    Serial2.print(msg);   // forward to UART
  }
}

// ---------------- MQTT CONNECT ----------------
void mqttConnect() {
  while (!mqttClient.connected()) {
    String clientID = "ESP32_" + String(random(1000, 9999));
    if (mqttClient.connect(clientID.c_str())) {
      mqttClient.subscribe(Subscribe_Master.c_str(), 1);
      mqttClient.publish(
        Publish_Master.c_str(),
        ("##*Master>" + String(Master_ID) + "*Online").c_str(),
        false
      );
    } else {
      delay(2000);
    }
  }
}

// ---------------- SERIAL2 RECEIVE (YOUR CODE) ----------------
void serial2Receive() {
  while (Serial2.available()) {
    char c = Serial2.read();

    if (c == '\n') {   // END OF FRAME
      rxData.trim();

      Serial.print("Received: ");
      Serial.println(rxData);

      // ---- PROCESS DATA ----
      if (rxData == "##INO") {
        Scale = true;
      }
      else if (rxData.startsWith("##*")) {
        mqttClient.publish(Publish_Master.c_str(), rxData.c_str(), false);
      }

      rxData = "";   // clear buffer
    } 
    else {
      rxData += c;
    }
  }
}

// ---------------- MASTER INFO ----------------
void sendMasterInfo() {
  String msg =
    "##*WMfO*" + String(Master_ID) +
    "*404040968066519*8991009020984748682F*IND airtel*404*40*5*42139*180844554*7*00.0000000*11.1111111*0";
  mqttClient.publish(Publish_Master.c_str(), msg.c_str(), false);
}
// ---------------- SETUP ----------------
void setup() {
  Serial.begin(115200);
  Serial2.begin(9600, SERIAL_8N1, RXD2, TXD2);

  Publish_Master = String(Master_ID) + "_PUB";
  Subscribe_Master = String(Master_ID) + "_SUC";

  WiFi.begin(WIFI_SSID, WIFI_PASS);
  while (WiFi.status() != WL_CONNECTED) delay(500);

  mqttClient.setServer(mqtt_ip.c_str(), mqtt_port);
  mqttClient.setCallback(mqttCallback);

  Serial.println("ESP32 Ready");
  Serial2.println("ESP32 Serial2 RX Ready");
}

// ---------------- LOOP ----------------
void loop() {
  if (!mqttClient.connected())
    mqttConnect();
  mqttClient.loop();

  serial2Receive();   // <<< YOUR RX LOGIC

  // ---------- HTTP API ----------
  if (millis() - lastHTTP > 5000) {
    lastHTTP = millis();
    HTTPClient http;
    http.begin("https://api.cenpilot.in/api/CenPilot/ValveListONCount?Id=3002853");
    if (http.GET() == 200 && Scale) {
      Serial2.print("cout " + http.getString());
      Scale = false;
    }
    http.end();
  }

  // ---------- MASTER INFO ----------
  if (millis() - lastMasterInfo > 30000) {
    lastMasterInfo = millis();
    sendMasterInfo();
  }
}
