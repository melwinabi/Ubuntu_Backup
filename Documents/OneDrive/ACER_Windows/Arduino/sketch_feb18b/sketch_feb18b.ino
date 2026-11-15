void setup() {
  Serial.begin(4800);
  Serial2.begin(4800, SERIAL_8N1, 16, 17);
}

void loop() {
  if (Serial.available()) {
    String data_one = Serial.readString();
    Serial2.println(data_one);
    data_one = "";
  }

  if (Serial2.available()) {
    String data = Serial2.readString();
    Serial.println(data);
    data = "";
  }  
}