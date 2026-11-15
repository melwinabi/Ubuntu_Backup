

void setup() {
  Serial.begin(115200); // Initialize Serial (UART0) for debugging
  Serial.println("Ser2 initialized");
}


void loop() {
  Serial.println("Hello"); // Send "Hello" over Serial2
  delay(1000);              // Add a delay to avoid flooding the Serial2
}
