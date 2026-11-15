#include <SoftwareSerial.h>
SoftwareSerial mySerial(2, 3); // RX, TX

void setup() {
  // Start the hardware serial for monitoring
  Serial.begin(9600);
  // Start the software serial for communication
  mySerial.begin(9600);
}

void loop() {
  // If data is available to read on the hardware serial
  if (Serial.available()) {
    // Read the data and send it through SoftwareSerial
    String data = Serial.readString();
    mySerial.print(data);
    Serial.println(data);
  }

  // If data is available to read from SoftwareSerial
  if (mySerial.available()) {
    // Read the data and send it to the hardware serial for monitoring
    String data2 = mySerial.readString();
    Serial.println(data2);
  }
}
