void setup() {
  // Begin serial communication
  Serial2.begin(9600); // SoftwareSerial
  Serial.begin(9600);   // Primary Serial
}

void loop() {
  if (Serial.available()) {
     String dataFromSerial = Serial.readString();
     Serial2.println(dataFromSerial);
  }


  if (Serial2.available()) {
    String dataFromMySerial = Serial2.readString();
    Serial.println(dataFromMySerial);
  }

  delay(100);
}
