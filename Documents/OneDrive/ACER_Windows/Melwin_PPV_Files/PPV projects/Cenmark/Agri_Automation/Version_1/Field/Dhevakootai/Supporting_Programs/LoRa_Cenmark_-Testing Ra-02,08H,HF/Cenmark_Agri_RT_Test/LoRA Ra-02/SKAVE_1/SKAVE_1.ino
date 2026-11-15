#include <SPI.h>
#include <LoRa.h>

const int csPin = 10;       
const int resetPin = 9;    
const int irqPin = 2;
int statge1 = 0;
int statge2 = 0;
int statge3 = 0;
String received = "";

void setup() {
  Serial.begin(9600);
  LoRa.begin(9600);   
  LoRa.setPins(csPin, resetPin, irqPin);  
  Serial.println("Starting LoRa...");
  if (!LoRa.begin(500E6)) {  // Set frequency
    Serial.println("LoRa init failed!");
    while (1);
  }
  LoRa.setSyncWord(0xA5);          // Set sync word for receiver matching
  Serial.println("Reciver");
}


void loop() 
{
    Rx_data();

    if(received == "11.10501791.05.02.0")
    {
       statge1 = 1;
       if(statge1 == 1)
       {
         delay(100);
         Tx_data("22.10501791.05.02.1");
         statge1 = 0;
       } 
    }
    else if(received == "11.10501792.03.01.0")
    {
       statge2 = 1;
       if(statge2 == 1)
       {
         delay(100);
         Tx_data("32.10501792.03.01.0");
         statge2 = 0;
       }
    } 
    else if(received == "23.10501792.03.01.1")
    {          
       statge3 = 1;
       if(statge3 == 1)
       {
         delay(100);
         Tx_data("33.10501792.03.01.1");
         statge3 = 0;
       }
    }        
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
    Serial.print("Received packet: ");
    received = " ";
    while (LoRa.available()) {
      char receivedChar = (char)LoRa.read();
      received += receivedChar;
    }
    received.trim();
    Serial.print("Message:");
    Serial.println(received);
  }
}
