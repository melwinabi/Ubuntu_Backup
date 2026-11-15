#include <SoftwareSerial.h>
SoftwareSerial loraRX(2, 3);

void setup() {
  loraRX.begin(9600);
  Serial.begin(9600);
  delay(100);
  
  // Configure receiver address and settings
  loraRX.println("AT+CADDRSET=01");   // Set transmitter address
  delay(100);
  loraRX.println("AT+CRXS=866000000,5,0,1,21,1"); // Frequency and settings
  delay(100);
}

void loop() {
  if (loraRX.available()) {
    String receivedData = "";
    while (loraRX.available()) {
      receivedData += (char)loraRX.read();
    }
    Serial.print("Received: ");
    Serial.println(receivedData);
  } else {
    Serial.println("NO TX");
    delay(2000);
  }
}
