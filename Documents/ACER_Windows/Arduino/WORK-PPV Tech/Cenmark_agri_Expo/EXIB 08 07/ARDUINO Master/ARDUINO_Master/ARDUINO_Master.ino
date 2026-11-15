#include <ArduinoJson.h>
#include <Ethernet.h>
#include <SPI.h>

EthernetClient client;

const char* server = "api.openweathermap.org";
const char* resource = "REPLACE_WITH_YOUR_URL_RESOURCE";
const unsigned long HTTP_TIMEOUT = 10000;
const size_t MAX_CONTENT_SIZE = 512;

byte mac[] = {0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED};

struct clientData {
  char temp[8];
  char humidity[8];
  char ids[6][8]; // To hold multiple IDs (6 IDs for this example)
};

void setup() {
  Serial.begin(9600);
  while (!Serial) {
    ;
  }
  Serial.println("Serial ready");
  if(!Ethernet.begin(mac)) {
    Serial.println("Failed to configure Ethernet");
    return;
  }
  Serial.println("Ethernet ready");
  delay(1000);
}

void loop() {
  if(connect(server)) {
    if(sendRequest(server, resource) && skipResponseHeaders()) {
      clientData clientData;
      if(readReponseContent(&clientData)) {
        printclientData(&clientData);
      }
    }
  }
  disconnect();
  wait();
}

bool connect(const char* hostName) {
  Serial.print("Connect to ");
  Serial.println(hostName);
  bool ok = client.connect(hostName, 80);
  Serial.println(ok ? "Connected" : "Connection Failed!");
  return ok;
}

bool sendRequest(const char* host, const char* resource) {
  Serial.print("GET ");
  Serial.println(resource);
  client.print("GET ");
  client.print(resource);
  client.println(" HTTP/1.1");
  client.print("Host: ");
  client.println(host);
  client.println("Connection: close");
  client.println();
  return true;
}

bool skipResponseHeaders() {
  char endOfHeaders[] = "\r\n\r\n";
  client.setTimeout(HTTP_TIMEOUT);
  bool ok = client.find(endOfHeaders);
  if (!ok) {
    Serial.println("No response or invalid response!");
  }
  return ok;
}

bool readReponseContent(struct clientData* clientData) {
  const size_t bufferSize = JSON_ARRAY_SIZE(1) + JSON_OBJECT_SIZE(1) + 
      2*JSON_OBJECT_SIZE(2) + JSON_OBJECT_SIZE(4) + JSON_OBJECT_SIZE(5) + 
      JSON_OBJECT_SIZE(6) + JSON_OBJECT_SIZE(12) + 390;
  DynamicJsonBuffer jsonBuffer(bufferSize);
  JsonObject& root = jsonBuffer.parseObject(client);
  if (!root.success()) {
    Serial.println("JSON parsing failed!");
    return false;
  }

  for (int i = 0; i < 6; i++) {
    // Parse each id_n field dynamically
    String key = "id_" + String(i);
    if (root.containsKey(key.c_str())) {
      strcpy(clientData->ids[i], root[key.c_str()]);
    }
  }

  return true;
}

void printclientData(const struct clientData* clientData) {
  for (int i = 0; i < 6; i++) {
    Serial.print("id_");
    Serial.print(i);
    Serial.print(" = ");
    Serial.println(clientData->ids[i]);
  }
}

void disconnect() {
  Serial.println("Disconnect");
  client.stop();
}

void wait() {
  Serial.println("Wait 60 seconds");
  delay(60000);
}
