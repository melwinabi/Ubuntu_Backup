void setup() {
  Serial.begin(115200);    // Debug monitor
  Serial2.begin(115200);     // Serial2 for communication with Nano
}

void loop() {
  Serial2.println("Hello from ESP32");
  delay(1000);
}
