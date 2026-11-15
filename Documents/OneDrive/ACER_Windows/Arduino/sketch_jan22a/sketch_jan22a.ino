void setup() {
  // Start the primary hardware serial port for debugging
  Serial.begin(115200);
  while (!Serial) {
    // Wait for Serial to initialize
  }
  Serial.println("Hardware Serial Example using Serial1");

  // Initialize Serial1
  Serial1.begin(115200);
  Serial.println("Serial1 started");
}

void loop() {
  // Send data from the primary hardware serial to Serial1

  // Send data from Serial1 back to the primary hardware serial
  if (Serial1.available()) {
    String data = Serial1.readStringUntil('\n'); // Read data until newline
    Serial.print("Received from Serial1: ");
    Serial.println(data);                        // Display received data
  }
}
