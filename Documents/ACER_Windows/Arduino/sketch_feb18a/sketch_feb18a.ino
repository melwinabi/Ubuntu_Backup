void setup() {
  Serial.begin(115200);
  Serial2.begin(115200, SERIAL_8N1, 16, 17);
}

void loop() {
  if (Serial2.available()) {
    String data = Serial2.readString();
    Serial.println(data);
    delay(200);    
  }
}
