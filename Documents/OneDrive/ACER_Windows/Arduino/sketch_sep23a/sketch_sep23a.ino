#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <ESP8266HTTPUpdateServer.h>
#include <AsyncElegantOTA.h>

// Replace with your network credentials
const char* ssid = "Captain_miller_03";
const char* password = "Melwin@123";

// Web server running on port 80
ESP8266WebServer server(80);

// OTA Update server
ESP8266HTTPUpdateServer httpUpdater;

void setup() {
  // Start Serial
  Serial.begin(115200);
  Serial.println();
  Serial.println("Booting");

  // Connect to WiFi
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);

  // Wait for connection
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.print("Connected to WiFi, IP address: ");
  Serial.println(WiFi.localIP());

  // Setup OTA update server
  httpUpdater.setup(&server);

  // Start web server
  server.begin();
  Serial.println("HTTP server started");
}

void loop() {
  // Handle web server
  server.handleClient();
}
