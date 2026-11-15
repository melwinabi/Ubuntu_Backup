#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <NewPing.h>
#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>

#define SCREEN_WIDTH  128
#define SCREEN_HEIGHT 32
#define OLED_RESET    -1
#define sts 14

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

#define TRIGGER_PIN  0 //d3
#define ECHO_PIN     2 //d4
#define MAX_DISTANCE 500
#define SIGNAL_PIN   14 // Choose a digital pin for signal

NewPing sonar(TRIGGER_PIN, ECHO_PIN, MAX_DISTANCE);

const char *ssid = "ROBOT_J";
const char *password = "12345678";

ESP8266WebServer server(80);

#define EMPTY_LEVEL_CM 33
#define FULL_LEVEL_CM 7
#define SIGNAL_THRESHOLD 90 // Adjust this threshold according to your needs

int percentage;       // Global variable
int distance;         // Global variable
int halfMaxDistance;  // Global variable

void setup() {
  Serial.begin(9600);

  // Connect to Wi-Fi
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi...");
  }
  Serial.println("Connected to WiFi");

  // Initialize display
  if (!display.begin()) {
    Serial.println(F("SSD1306 allocation failed"));
    for (;;);
  }

  display.display();
  delay(20);
  display.clearDisplay();

  // Set up web server routes
  server.on("/", HTTP_GET, handleRoot);

  // Start server
  server.begin();
  Serial.println("Connected to WiFi");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());

  // Initialize signal pin as output
  pinMode(SIGNAL_PIN, OUTPUT);
}

void loop() {
  server.handleClient();  // Handle web server clients
  delay(50);

  distance = sonar.ping_cm();
  halfMaxDistance = MAX_DISTANCE / 2;
  percentage = map(distance, FULL_LEVEL_CM, EMPTY_LEVEL_CM, 100, 0); // Map the distance to a percentage range

  // Check if the dustbin is full and send signal
  if (percentage >= SIGNAL_THRESHOLD) {
    digitalWrite(SIGNAL_PIN, HIGH); // Send signal
  } else {
    digitalWrite(SIGNAL_PIN, LOW); // Ensure signal pin is low if not sending signal
  }

  updateDisplay();

  // Handle other tasks or animations here
}

void updateDisplay() {
  display.clearDisplay();
  display.setTextSize(4);
  display.setTextColor(SSD1306_WHITE);

  static int xPos = SCREEN_WIDTH;
  xPos -= 2;
  if (xPos < -SCREEN_WIDTH) {
    xPos = SCREEN_WIDTH;
  }

  display.setCursor(xPos, 0);

  if (distance == 0) {
    display.print(F("Error"));
  } else {
    display.print(F("Waste: "));
    display.print(percentage);
    display.print(F("%"));

    if (distance >= EMPTY_LEVEL_CM) {
      display.print(F(" (Empty)"));
      digitalWrite(sts,LOW);
       Serial.println("nsd");
    } else if (distance <= FULL_LEVEL_CM) {
      display.print(F(" (Full)"));
      digitalWrite(sts,HIGH);
      Serial.println("nsd");
    } else {
      display.print(F(" (Not Empty)"));
      digitalWrite(sts,LOW);
      Serial.println("nsd");
    }
  }

  display.display();
}

void handleRoot() {
  String html = "<html><head>";
  html += "<style>";
  html += "body { font-family: Arial, sans-serif; text-align: center; margin: 50px; background-color: #111; color: #fff; }";
  html += "h1 { color: #3498db; }";
  html += "p { font-size: 24px; color: #666; }";
  html += ".empty { color: #00cc00; font-weight: bold; }";
  html += ".full { color: #cc0000; font-weight: bold; }";
  html += ".navbar { background-color: #333; overflow: hidden; }";
  html += ".navbar a { float: left; display: block; color: #f2f2f2; text-align: center; padding: 14px 16px; text-decoration: none; }";
  html += ".navbar a:hover { background-color: #ddd; color: black; }";
  html += ".garbage-container { display: flex; justify-content: center; align-items: center; height: 300px; }";
  html += ".garbage-bin { position: relative; width: 150px; height: 200px; background-color: #808080; border-radius: 10px; overflow: hidden; }";
  html += ".water { position: absolute; bottom: 0; left: 0; width: 100%; height: " + String(percentage) + "%; background-color: #3498db; transition: height 2s ease-in-out; }";
  html += ".garbage-text { position: absolute; top: 50%; left: 50%; transform: translate(-50%, -50%); color: #fff; font-size: 16px; }";
  html += ".garbage-bin:hover .water { height: 100%; }";
  html += "</style>";
  html += "</head><body>";

  // Navbar
  html += "<div class='navbar'>";
  html += "<a href='#'>Home</a>";
  html += "<a href='#'>About</a>";
  html += "<a href='#'>Contact</a>";
  html += "</div>";

  // Waste Level Content with Animation
  html += "<h1>Waste Level</h1>";
  html += "<div class='garbage-container'>";
  html += "<div class='garbage-bin'>";
  html += "<div class='water'></div>";
  html += "<div class='garbage-text'>Garbage</div>";
  html += "</div>";
  html += "</div>";
  html += "<p class='";
  html += (distance >= EMPTY_LEVEL_CM) ? "empty" : (distance <= FULL_LEVEL_CM) ? "full" : "notEmpty";
  html += "'>Current waste level: " + String(percentage) + "%</p>";

  // Auto-refresh meta tag
  html += "<meta http-equiv='refresh' content='5'>";

  html += "</body></html>";
  server.send(200, "text/html", html);
}
