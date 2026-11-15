 #include <SPI.h>
#include <LoRa.h>

#define ss 10
#define rst 9
#define dio0 2
int statge1 = 0;
String received = "";

void setup() {
  Serial.begin(9600);
  LoRa.setPins(ss, rst, dio0);  
  if (!LoRa.begin(433E6)) {
    Serial.println("Starting LoRa failed!");
    while (1);
  }

  digitalWrite(rst,HIGH);
  LoRa.setSyncWord(0xF3);  
  LoRa.setSpreadingFactor(12); 
  LoRa.setSignalBandwidth(125E3);  
  LoRa.setCodingRate4(5);  
  LoRa.setTxPower(20); 
  Serial.println("LoRa Receiver");
}

void loop() 
{
    Rx_data();
    if(received == "32.10501792.03.01.0")
    {
       statge1 = 1;
       if(statge1 == 1)
       {
         delay(100);
         Tx_data("23.10501792.03.01.1");
         statge1 = 0;
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
    Serial.print("Received packet: ");
    received = "";
    while (LoRa.available()) {
      char receivedChar = (char)LoRa.read();
      received += receivedChar;
    }
    Serial.print("Message: ");
    Serial.println(received);
  } 
}
