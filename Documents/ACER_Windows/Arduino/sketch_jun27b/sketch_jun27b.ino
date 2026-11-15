

#include <WiFi.h>
#include <PubSubClient.h>

// Update these with your network credentials and MQTT broker information
const char* ssid = "Caption_Miller";
const char* password = "12345678";
const char* mqtt_server = "broker.mqtt-dashboard.com";

WiFiClient espClient;
PubSubClient client(espClient);

const int ledPin = 2;

void setup_wifi() {
  delay(10);
  // Connecting to the WiFi network
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
  for (int i = 0; i < length; i++) {
    Serial.print((char)payload[i]);
  }
  Serial.println();

  // Assuming payload is either "0" or "1"
  if (length == 1 && payload[0] == '1') {
    digitalWrite(ledPin, LOW); // Turn the LED on (active LOW for built-in LED)
    Serial.println("LED turned ON");
  } else if (length == 1 && payload[0] == '0') {
    digitalWrite(ledPin, HIGH); // Turn the LED off
    Serial.println("LED turned OFF");
  }
}

void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Attempt to connect
    if (client.connect("ESP32Client")) {
      Serial.println("connected");
      // Once connected, publish an announcement...
      client.publish("LedStstus", "hello world");
      // ... and resubscribe
      client.subscribe("Melwin");
      Serial.println("Subscribed to inTopic");
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
  digitalWrite(ledPin, HIGH); // Turn the LED off (active LOW for built-in LED)
  
  Serial.begin(115200);
  setup_wifi();
  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);
}

void loop() {
  if (!client.connected()) {
    reconnect();
  }
  client.loop();
}
