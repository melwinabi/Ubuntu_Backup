#include<LoRa.h>
#include<Wire.h>
#include<WiFi.h>
#include<PubSubClient.h>  

#define ss 4 
#define rst 5
#define dio0 2 

WiFiClient espClient;
PubSubClient client(espClient);

String Master_ID  = "3002850";
String Node_IDs[] = { "1002850", "1002851", "1002852", "1002853", "1002854", "1002855", "1002856" };
String Pump_ID = "2002851";

String part1;
String part2;
String part3;
String part4;
String Percentage_rssi;
String MsgSnr;
String MESSAGE = " ";
String received = " ";
String msg = "";
String MQTT_SEND;

unsigned long previousMillis = 0;
unsigned long currentMillis;

const char* ssid = "PPV TECH";
const char* password = "ppv@12345678";
const char* mqtt_server = "198.38.81.119";
const long interval = 60000;

int Hold = 13; //red
int Start = 22; //green
int Signal = 21; //yellow
int Switch = 1;
int x = 0;
int Timer;

bool ID;
bool mqttSent = false; // Added flag to prevent continuous MQTT sending

void setup() {
  Serial.begin(9600);
  pinMode(Hold, OUTPUT);
  pinMode(Start, OUTPUT);
  pinMode(Signal, OUTPUT);
  digitalWrite(Hold, HIGH);
  digitalWrite(Start, HIGH);
  digitalWrite(Signal, HIGH);
  delay(500);
  digitalWrite(Start, LOW);
  digitalWrite(Signal, LOW);
  Serial.print("Master ID > ");
  Serial.println(Master_ID);
  LoRa.setPins(ss, rst, dio0);
  if (!LoRa.begin(433E6)) {
    Serial.println("LoRa initialization failed!");
    while (true);
  }  
  LoRa.setSpreadingFactor(12); 
  LoRa.setSignalBandwidth(62.5E3);  
  LoRa.setCodingRate4(5);     
  LoRa.setSyncWord(0xA5);         
  LoRa.setPreambleLength(8);  
  delay(1000);
  WiFi.begin(ssid, password);
  delay(500);
  while (WiFi.status() != WL_CONNECTED) {
    digitalWrite(Hold, HIGH);
    delay(500);
    digitalWrite(Hold, LOW);
    delay(500);

  }
 
  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);
  digitalWrite(Hold, LOW);
  digitalWrite(Start, HIGH);
}

void callback(char* topic, byte* payload, unsigned int length) {
  msg = "";
  for (unsigned int i = 0; i < length; i++) {
    msg += (char)payload[i];
  }
  Serial.println(msg);
  Network_Data(msg);
  msg = "";

  if (msg.length() > 0) {
    digitalWrite(Hold, HIGH);
    delay(500);
    digitalWrite(Hold, LOW);
  }
}

void reconnect() {
  while (WiFi.status() != WL_CONNECTED) {
    WiFi.begin(ssid, password);
    delay(500);
  }
  while (!client.connected()) {
    String clientId = "ESP32Client-";
    clientId += String(random(0xffff), HEX);
    if (client.connect(clientId.c_str())) {
      client.subscribe("Cenmark_PPV");
      MESSAGE = "";
      MESSAGE = "SYSTEM ON > ID : " + Master_ID;
      client.publish("RX_Valve_test", MESSAGE.c_str());
    } 
    else 
    {
      delay(5000);
    }
  }
}

void loop() 
{
  COUNT_t();
  if (Switch == 1) {
    if (!client.connected()) {
      reconnect();  
    } 
    client.loop();
    delay(200);
    if (!mqttSent) {
      client.publish("RX_Valve_test", MQTT_SEND.c_str());
      mqttSent = true;
    }
  } else if (Switch == 0) {
    Rx_data(); 
  }
}

void LoRa_Tx(String Data) {
  Serial.println("in");
  LoRa.beginPacket();
  LoRa.print(Data);
  LoRa.endPacket();
  Serial.println(Data);
  digitalWrite(Signal, HIGH);
  delay(100);
  digitalWrite(Signal, LOW);
  delay(100);
  Serial.println("out");
}

void Network_Data(String incomingByte) {
  if (incomingByte.startsWith("Tx.") || incomingByte.startsWith("Ack.")) {
    incomingByte.trim();
    int start_index = incomingByte.indexOf('.') + 1;
    int part1_end_index = incomingByte.indexOf('.', start_index);
    part1 = incomingByte.substring(start_index, part1_end_index);
    
    start_index = part1_end_index + 1;
    int part2_end_index = incomingByte.indexOf('.', start_index);
    part2 = incomingByte.substring(start_index, part2_end_index);
    
    start_index = part2_end_index + 1;
    int part3_end_index = incomingByte.indexOf('.', start_index);
    part3 = incomingByte.substring(start_index, part3_end_index);
    
    start_index = part3_end_index + 1;
    part4 = incomingByte.substring(start_index);  
    isValidNode(part1);
    
    if ((part1 == Pump_ID) && ID) {
        if (incomingByte.startsWith("Tx.")) {
            Serial.println("SENDING LoRa......");
            MESSAGE = "Ack." + Master_ID + ".00.0";
            client.publish("RX_Valve_test", MESSAGE.c_str());

            Switch = 0;
            LoRa_Tx(incomingByte);

            // Start Timer correctly
            previousMillis = millis();
            Timer = 1;
            part1 = "";
        } 
        else if (incomingByte.startsWith("Ack.")) {
            Timer = 0;
            Switch = 1;
            Serial.println("SENDING MQTT......");
            MESSAGE = "Ack." + Master_ID + ".11.1";
            client.publish("RX_Valve_test", MESSAGE.c_str());

            MQTT_SEND = incomingByte;
            mqttSent = false; // Reset flag after acknowledgment
        }
        ID = false;
    } 
    else if (part1 == Master_ID) {
        if (part2 == "Rst") {
            MESSAGE = "Ack." + Master_ID + ".Reset";
            client.publish("RX_Valve_test", MESSAGE.c_str());
            Serial.println("Restarting");
            ESP.restart();
        }+-----------------
    } 
    else {
        MESSAGE = "INVALID ID FOUND!";
        client.publish("RX_Valve_test", MESSAGE.c_str());
    }
  }
}

void COUNT_t() {
    if (Timer == 1) {
        currentMillis = millis();
        
        if (currentMillis - previousMillis >= interval) {
            Serial.println("Timer expired, sending ACK...");
            MESSAGE = "";
            MESSAGE = "Ack." + Master_ID + ".11.1";
            client.publish("RX_Valve_test", MESSAGE.c_str());
            delay(500);

            // Reset values properly
            Timer = 0; 
            Switch = 1;
        }
    }
}


void Rx_data() {
  int packetSize = LoRa.parsePacket();
  if (packetSize) {
    received = " ";
    while (LoRa.available()) {
      char receivedChar = (char)LoRa.read();
      received += receivedChar;  
    }
    received.trim();
    Serial.println("LoRa received: [" + received + "]");
    Network_Data(received); 
  }  
}

void isValidNode(String incomingData) {
  int totalNodes = sizeof(Node_IDs) / sizeof(Node_IDs[0]);
  for (int i = 0; i < totalNodes; i++) {
    if (incomingData.equals(Node_IDs[i])) 
    {
      ID = true;
      break;
    }
    else
    {
      ID = false;
    }  
  }
}




