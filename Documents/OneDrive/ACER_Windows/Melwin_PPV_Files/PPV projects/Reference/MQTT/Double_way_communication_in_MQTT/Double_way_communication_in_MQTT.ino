#include <WiFi.h>
#include <PubSubClient.h>
#include <EEPROM.h>

const char* ssid = "Caption_Miller";
const char* password = "12345678";
const char* mqtt_server = "broker.mqtt-dashboard.com";

const int ledPin = 2;
const int switchPin = 4;
const int EEPROM_LED_ADDR = 0; 

WiFiClient espClient;
PubSubClient client(espClient);

bool ledState = LOW;
bool lastSwitchState = LOW;
unsigned long lastDebounceTime = 0;
unsigned long debounceDelay = 50;

void setup_wifi() {
  delay(10);
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

  // Convert payload to string
  String message = "";
  for (int i = 0; i < length; i++) {
    message += (char)payload[i];
  }

  Serial.println(message);

 
  if (strcmp(topic, "espMel123/led") == 0) {
    if (message.equals("0")) {
      ledState = LOW;
    } else if (message.equals("1")) {
      ledState = HIGH;
    }

    digitalWrite(ledPin, ledState);
    EEPROM.write(EEPROM_LED_ADDR, ledState ? 1 : 0); 
    EEPROM.commit(); 
    Serial.println(ledState ? "LED turned ON by MQTT" : "LED turned OFF by MQTT");
  }
}

void reconnect() {
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    String clientId = "ESP32Client-";
    clientId += String(random(0xffff), HEX);

    if (client.connect(clientId.c_str())) {
      Serial.println("connected");
      
      client.subscribe("espMel123/led");

      
      ledState = EEPROM.read(EEPROM_LED_ADDR);
      digitalWrite(ledPin, ledState);
      Serial.println(ledState ? "Last known LED state: ON" : "Last known LED state: OFF");
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      delay(5000);
    }
  }
}

void setup() {
  pinMode(ledPin, OUTPUT);
  pinMode(switchPin, INPUT_PULLUP);
  digitalWrite(ledPin, ledState);

  Serial.begin(9600);
  setup_wifi();
  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);

  EEPROM.begin(512); 
}

void loop() {
  if (!client.connected()) {
    reconnect();
  }
  client.loop();

  // Read switch state and debounce
  bool reading = digitalRead(switchPin);
  if (reading != lastSwitchState) {
    lastDebounceTime = millis();
  }

  if ((millis() - lastDebounceTime) > debounceDelay) {
    if (reading != ledState) {
      ledState = reading;
      digitalWrite(ledPin, ledState ? HIGH : LOW);

      client.publish("espMel123/led", ledState ? "1" : "0");
      Serial.println(ledState ? "LED turned ON by switch" : "LED turned OFF by switch");

  
      EEPROM.write(EEPROM_LED_ADDR, ledState ? 1 : 0); 
      EEPROM.commit(); 
    }
  }

  lastSwitchState = reading;
  int eep = EEPROM.read(0);
  Serial.print(eep);
}//working 80%
