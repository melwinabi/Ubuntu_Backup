#include <SoftwareSerial.h>

// Define LoRa pins for RX
SoftwareSerial loraSerial(2, 3);  // RX on pin 2, TX on pin 3
const int M0 = 4;
const int M1 = 5;


void setup() {
  Serial.begin(9600);       // For serial monitor
  loraSerial.begin(9600);   // LoRa module baud rate

  pinMode(M0, OUTPUT);
  pinMode(M1, OUTPUT);
  digitalWrite(M0, LOW);    // Enter normal mode
  digitalWrite(M1, LOW);

  Serial.println("LoRa Receiver Ready");
}

void loop() 
{
  if (loraSerial.available()) 
  {
     String receivedMessage = loraSerial.readStringUntil("⸮");
     receivedMessage.trim();  
     Serial.println("Sending Response: " + receivedMessage);
     String sendDX= "ACK."+ receivedMessage;
     loraSerial.println(sendDX);   
   }
 }
