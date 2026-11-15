void setup() {
  Serial.begin(115200);  // Start serial communication at a baud rate of 115200
  while (!Serial);       // Wait for the serial port to connect (optional)
  Serial.println("ESP32 Serial Read Program");
}

void loop() {
  if (Serial.available() > 0) {        // Check if data is available to read
    String receivedData = Serial.readStringUntil('\n');  // Read the incoming data until newline
    Serial.print("Received: ");       
    Serial.println(receivedData);      // Print the received data
  }
}
