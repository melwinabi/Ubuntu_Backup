HardwareSerial mySerial(2);  // Use Hardware Serial 2 (TX2 = GPIO17, RX2 = GPIO16)
String received;

void setup() {
  Serial.begin(9600);                   // For Serial Monitor
  mySerial.begin(9600, SERIAL_8N1, 16, 17);  // Initialize Serial2 with correct pins
  Serial.println("ESP32 ready");
}

void loop() {
  if (mySerial.available()) {
    received = mySerial.readStringUntil('\n');
    Serial.print("Received from Arduino: ");
    Serial.println(received);
  
  }
  
  String data = received + "Ack";  // Append "Ack" to received data
  mySerial.println(data);          // Send the modified data back
  delay(1000);  
   received = "";                     // Add a delay to prevent flooding
}
