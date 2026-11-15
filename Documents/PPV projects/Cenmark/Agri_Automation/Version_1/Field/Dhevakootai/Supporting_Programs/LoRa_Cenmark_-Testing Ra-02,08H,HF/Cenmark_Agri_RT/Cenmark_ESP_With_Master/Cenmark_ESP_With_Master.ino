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
    pinMode(led, OUTPUT);
    WiFi.begin(ssid, password);
    while (WiFi.status() != WL_CONNECTED) {
        digitalWrite(led, HIGH); 
        delay(500);
        digitalWrite(led, LOW);
        delay(500);
    }

    digitalWrite(led, HIGH);
    delay(500);
    digitalWrite(led, LOW);
    delay(500);

    client.setServer(mqtt_server, 1883);
    client.setCallback(callback);

    HTTPClient http;
    http.begin("https://ppvgroups.com/node.php?count=10");
    int httpCode = http.GET();
    
    if (httpCode > 0) { // Successful HTTP response
        String payload = http.getString();
        StaticJsonDocument<1024> jsonBuffer; // Adjust size if needed
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

void callback(char* topic, byte* payload, unsigned int length) {
//    Serial.print("Topic: ");
//    Serial.println(topic);
    String message = "";

    for (unsigned int i = 0; i < length; i++) {
        message += (char)payload[i];
    }
    
//    Serial.print("Message: ");
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

    // Handle incoming Serial messages
    if (Serial.available() > 0) {
        char incomingBuffer[100]; // Buffer to store incoming data
        size_t len = Serial.readBytesUntil('\n', incomingBuffer, sizeof(incomingBuffer) - 1); 
        incomingBuffer[len] = '\0'; // Null-terminate the strin
        // Check if the message starts with "Ack."
        if(incomingBuffer != Lastmsg)
        {
         if (strncmp(incomingBuffer, "Ack.", 4) == 0 || strncmp(incomingBuffer, "Bt.", 3) == 0  || strncmp(incomingBuffer, "Sg.", 3) == 0 ) 
         {
            client.publish("RX_Valve_test", incomingBuffer); 
         }
         Lastmsg = incomingBuffer;
      }  
   }
}
