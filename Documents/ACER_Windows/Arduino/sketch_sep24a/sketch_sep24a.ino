#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <ESP8266HTTPUpdateServer.h>

// Replace these with your network credentials
const char* ssid = "Captain_miller_03";
const char* password = "Melwin@123";

// Create web server object on port 80
ESP8266WebServer server(80);
ESP8266HTTPUpdateServer httpUpdater;

void setup() {
  Serial.begin(115200);
  Serial.println();
  Serial.println("Booting");

  // Connect to WiFi
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi...");
  }

  Serial.println("Connected to WiFi");

  // Print the IP address
  Serial.println(WiFi.localIP());

  // Initialize the HTTP update server
  httpUpdater.setup(&server);

  // Start the server
  server.begin();
  Serial.println("HTTP server started");
}

void loop() {
  // Handle any client requests
  server.handleClient();
}
