#include <SPI.h>
#include <LoRa.h>
unsigned long previousMillis = 0;
const int csPin = 10;       
const int resetPin = 9;    
const int irqPin = 2;
String received = "";
int Flag = 1;
int statge1 = 0;

void setup() {
  Serial.begin(9600);
  LoRa.begin(9600);  
  pinMode(8,OUTPUT);
  pinMode(7,OUTPUT);  
  digitalWrite(8,HIGH);
  digitalWrite(7,HIGH); 
  delay(500);
  digitalWrite(8,LOW);
  digitalWrite(7,LOW); 
  LoRa.setPins(csPin, resetPin, irqPin);  
  Serial.println("Starting LoRa...");
  if (!LoRa.begin(500E6)) {  // Set frequency
    Serial.println("LoRa init failed!");
    while (1);
  }
  LoRa.setSyncWord(0xA5);          // Set sync word for receiver matching
  Serial.println("Master");
}

void loop() {
   unsigned long currentMillis = millis();
    Rx_data();
  if (currentMillis - previousMillis >= 1000) {
    previousMillis = currentMillis; 
    Flag = 1;
  }
  if(Flag == 1)
  {
  digitalWrite(7,LOW);  
  delay(250);
  Tx_data("11.10501791.05.02.0");
  Flag = 0;
  } 
  if(received == "22.10501791.05.02.1")
  {
     statge1 = 1;
     if(statge1 == 1)
     {
       Serial.println("Node 1 Set");
       digitalWrite(7,HIGH);
       delay(100);
       //Tx_data("11.10501792.03.01.0");
       statge1 = 0;
       Flag = 1;
     }
  }
  else if(received == "33.10501792.03.01.1")
  {
     statge1 = 1;
     if(statge1 == 1)
     {
       Serial.println("Node 2 Set");
       digitalWrite(8,HIGH);
     } 
  }  
  received = "";
}

void Tx_data( String data)
{
  LoRa.beginPacket();
  LoRa.print(data);
  LoRa.endPacket();  
  Serial.print("Data Sent: ");
  Serial.println(data);
}

void Rx_data()
{
  int packetSize = LoRa.parsePacket();
  if (packetSize) {
    received = "";
    while (LoRa.available()) {
      char receivedChar = (char)LoRa.read();
      received += receivedChar;
    }
  }  
}
