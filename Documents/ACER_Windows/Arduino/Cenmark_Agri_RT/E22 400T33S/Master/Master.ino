#include <SoftwareSerial.h>

// Define LoRa pins for TX
SoftwareSerial loraSerial(2, 3);  // RX on pin 2, TX on pin 3
const int M0 = 4;
const int M1 = 5;
int message = 10;
int index;
void setup() {
  Serial.begin(9600);       // For serial monitor
  loraSerial.begin(9600);   // LoRa module baud rate 
  pinMode(M0, OUTPUT);
  pinMode(M1, OUTPUT);
  digitalWrite(M0, LOW);    // Enter normal mode
  digitalWrite(M1, LOW);
  Serial.println("LoRa Transmitter Ready");
}

void loop() {

      message++;
      if (message > 99) {
        message = 10;
      }
  // Send the message
  loraSerial.print(message);  // Use `print` instead of `println` to avoid adding a newline
  //Serial.println("Sent: " + message);

  // Check for response from the node
  if (loraSerial.available()) {
    String receivedMessage = loraSerial.readString();
      int start_index = receivedMessage.indexOf('.')+1;
      int part1_end_index = receivedMessage.indexOf('.')+3;
      String part1 = receivedMessage.substring(start_index, part1_end_index);
       part1.trim();  // Remove extra spaces/newlines
      Serial.println(part1);
    }
    //BTSerial.println(receivedMessage);
 

  delay(2000);  // Wait 2 seconds before sending the next message
}
