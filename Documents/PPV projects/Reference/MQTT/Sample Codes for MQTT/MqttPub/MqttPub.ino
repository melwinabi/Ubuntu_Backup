#include <WiFi.h>
#include <PubSubClient.h>


const char* ssid = "Caption_Miller";
const char* password = "12345678";
const char* mqtt_server = "broker.mqtt-dashboard.com";

WiFiClient espClient;
PubSubClient client(espClient);

const int ledPin = 2; // Built-in LED pin

void setup_wifi() {
  delay(10);
  // We start by connecting to a WiFi network
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
}

void callback(char* topic, byte* payload, unsigned int length) {
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] ");
  String message = "";
  for (int i = 0; i < length; i++) {
    Serial.print((char)payload[i]);
    message += (char)payload[i];
  }
  Serial.println();

  // Assuming payload is either "0" or "1"
  if (message == "0") {
    digitalWrite(ledPin, LOW); // Turn the LED on (LOW is the voltage level)
    Serial.println("LED turned ON");
  } else if (message == "1") {
    digitalWrite(ledPin, HIGH); // Turn the LED off (HIGH is the voltage level)
    Serial.println("LED turned OFF");
  }
}

void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Create a random client ID
    String clientId = "ESP32Client-";
    clientId += String(random(0xffff), HEX);
    // Attempt to connect
    if (client.connect(clientId.c_str())) {
      Serial.println("connected");
      // Once connected, publish an announcement...
      client.publish("LedStatus", "hello world");
      // ... and resubscribe
      client.subscribe("espMel123/#");
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}

void setup() {
  pinMode(ledPin, OUTPUT); // Initialize the built-in LED pin as an output
 // digitalWrite(ledPin, HIGH); // Turn the LED off initially (HIGH is the voltage level)
  
  Serial.begin(9600);
  setup_wifi();
  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);
}

void loop() {
  if (!client.connected()) {
    reconnect();
  }
  client.loop();
}//working
