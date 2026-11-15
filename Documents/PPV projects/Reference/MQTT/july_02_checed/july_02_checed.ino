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

int ledState;
int LEDMem;
int lastSwitchState;
int switchState;
unsigned long lastDebounceTime = 0;
unsigned long debounceDelay = 500;

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
  lastSwitchState = 0;
  ledState = 0;
  switchState = 0;
  Serial.begin(9600);
  setup_wifi();
  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);
  EEPROM.begin(512); 
}

void loop() 
{
  if (!client.connected()) 
  {
    reconnect();
  }
  client.loop();

  // Hardware
  switchState = digitalRead(switchPin);
  if(switchState == LOW)
  {
    if (LEDMem == 1)
    {
      LEDMem = 0;
      Serial.println("LED turned OFF by switch");
    }
    else
    {
      LEDMem = 1;
      Serial.println("LED turned ON by switch");
    }
    delay(500);
  }
  // MQTT
  if ((millis() - lastDebounceTime) > debounceDelay) 
  {
    lastDebounceTime = millis();
    client.publish("espMel123/led", ledState ? "1" : "0");
    if(ledState == HIGH)
    {
      LEDMem = 1;
      Serial.println("LED turned ON by MQTT");
    }
    else
    {
      LEDMem = 0;
      Serial.println("LED turned OFF by MQTT");
    }
  }
  if(LEDMem == 1)
  {
    digitalWrite(ledPin, HIGH);
  }
  else
  {
    digitalWrite(ledPin, LOW);
  }
}
