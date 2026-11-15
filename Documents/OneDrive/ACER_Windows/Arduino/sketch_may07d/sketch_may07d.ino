#include <ESP8266WiFi.h>

// Define pins for ultrasonic sensor
const int trigPin = 0; // GPIO pin connected to the trigger pin of ultrasonic sensor
const int echoPin = 2; // GPIO pin connected to the echo pin of ultrasonic sensor

// WiFi credentials
const char* ssid = "your-ssid";
const char* password = "your-password";

// Initialize WiFi client
WiFiClient client;

void setup() {
  Serial.begin(9600);
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);
  
  // Connect to WiFi
  connectWiFi();
}

void loop() {
  long duration, distance;
  
  // Trigger ultrasonic sensor
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);
  
  // Read echo pin
  duration = pulseIn(echoPin, HIGH);
  
  // Calculate distance
  distance = duration * 0.034 / 2;
  
  // Print distance to Serial Monitor
  Serial.print("Distance: ");
  Serial.println(distance);
  
  delay(1000); // Wait for a second before taking another measurement
}

void connectWiFi() {
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
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}
