unsigned long previousMillis = 0;
const unsigned long interval = 60000; // 50 seconds in milliseconds

void setup() {
  Serial.begin(115200);
}

void loop() {
  if (millis() - previousMillis >= interval) {
    previousMillis = millis();
    Serial.println("50 seconds elapsed!");
  }
}
