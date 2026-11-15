#include <WiFi.h>
#include <PubSubClient.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>

int led = 2; 
const char* ssid = "PPV TECH";
const char* password = "ppv@12345678";
const char* mqtt_server = "198.38.81.119";
int id_in = 0;
String message = " ";
String Lastmsg = " ";
WiFiClient espClient;
PubSubClient client(espClient);

void setup() {
    Serial.begin(115200);
    Serial2.begin(9600);
    pinMode(led, OUTPUT);
    WiFi.begin(ssid, password);
    while (WiFi.status() != WL_CONNECTED) {
        digitalWrite(led, HIGH); 
        delay(500);
        digitalWrite(led, LOW);
        delay(500);
    }
    
    for (int i = 0; i < 7; i++) {
        digitalWrite(led, HIGH);
        delay(100);
        digitalWrite(led, LOW);
        delay(100);
    }

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
            Serial2.print("*");
            for (JsonPair kv : root) {
                const char* key = kv.key().c_str();
                if (String(key).startsWith("id_")) {
                    const char* sensor = kv.value();
                    id_in++;
                    Serial2.print(sensor);       
                }      
            }
            Serial2.print("*");
            Serial2.print("$");
            Serial2.print(id_in);
            Serial2.println("#");
        } else {
            Serial.println("JSON parse error.");
        }
    } else {
        Serial.println("HTTP GET failed.");
    }
    http.end(); 
}

void callback(char* topic, byte* payload, unsigned int length) {
    String message = "";

    for (unsigned int i = 0; i < length; i++) {
        message += (char)payload[i];
    }
    
    Serial.println(message);

    if (message.length() > 0) {
        digitalWrite(led, HIGH);
        delay(500);
        digitalWrite(led, LOW);
    }
}

void reconnect() {
    while (WiFi.status() != WL_CONNECTED) {
        WiFi.begin(ssid, password);
        delay(500);
    }

    while (!client.connected()) {
        String clientId = "ESP32Client-";
        Serial.println("Client Disconnect");
        clientId += String(random(0xffff), HEX);
        if (client.connect(clientId.c_str())) {
            client.publish("RX_Valve_test", "SV Notification");
            client.subscribe("Cenmark_PPV");
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

    if (Serial2.available() > 0) {
        char incomingBuffer[100];
        size_t len = Serial2.readBytesUntil('\n', incomingBuffer, sizeof(incomingBuffer) - 1); 
        incomingBuffer[len] = '\0'; 
        if (String(incomingBuffer) != Lastmsg) {
            if (strncmp(incomingBuffer, "Ack.", 4) == 0 || strncmp(incomingBuffer, "Bt.", 3) == 0 || strncmp(incomingBuffer, "Sg.", 3) == 0) {
                client.publish("RX_Valve_test", incomingBuffer); 
            }
            Lastmsg = incomingBuffer;
        }
    }
}
