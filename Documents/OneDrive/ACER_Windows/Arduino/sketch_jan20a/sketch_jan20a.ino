void setup() {
  Serial.begin(115200);
  while (!Serial) {}
  Serial.println("Hardware Serial Example using Serial1");
  Serial1.begin(115200);
  Serial.println("Serial1 started");
}

void loop() {
  if (Serial.available()) {
    String data = Serial.readStringUntil('\n');
    Serial1.println(data);
    Serial.print("Sent to Serial1: ");
    Serial.println(data);
  }
  if (Serial1.available()) {
    String data = Serial1.readStringUntil('\n');
    Serial.print("Received from Serial1: ");
    Serial.println(data);
  }
}
