#include <SPI.h>
#include <LoRa.h>
#define ss 10
#define rst 9
#define dio0 2

int D8 = 8;
int D7 = 7;
int sts_v1;
int sts_v2;
float Percentage = 100;
String Node_Id = "1002851"; //change id for upcomming nodes
String received = "";
String lastTransmitted = ""; 
String part1;
String part2;
String part3;
String Message =" ";
void setup() 
{
  Serial.begin(9600);
  pinMode(rst, OUTPUT);
  pinMode(D8, OUTPUT);
  pinMode(D7, OUTPUT);
  LoRa.setPins(ss, rst, dio0);  
  if (!LoRa.begin(433E6)) {
    Serial.println("LoRa initialization failed. Check connections.");
    while (true); 
  } 
  LoRa.setSyncWord(0xA5);          
  Serial.println("LoRa Slave Initialized");
}

void loop() 
{
  Rx_data();
  if(part1 == Node_Id)
  {
    if(part2 == "01")
    {
      if(part3 == "1")
      {
        digitalWrite(D7, HIGH);
        delay(200);
        sts_v1=1;
        Message = "Ack." + part1 + "." + "01" + "." + "1";  
        delay(100);
        Message.trim();    
        Tx_data(Message);      
      }
      else if(part3 == "0")
      {
        digitalWrite(D7, LOW);
        delay(200);
        sts_v1=0;  
        Message = "Ack." + part1 + "." + "01" + "." + "0";  
        delay(100);
        Message.trim();               
        Tx_data(Message);               
      }
      else if(part3 == "?")
      {
        if(sts_v1 == "1")
        {
          Message = "Ack." + part1 + "." + "01" + "." + "1";  
          delay(100);
          Message.trim();               
          Tx_data(Message);          
        }
        else if(sts_v1 == "0")
        {
          Message = "Ack." + part1 + "." + "01" + "." + "0";  
          delay(100);
          Message.trim();               
          Tx_data(Message);            
        }
      }
    }
    else if(part2 == "02")
    {
      if(part3 == "1")
      {
        digitalWrite(D8, HIGH);
        delay(200);
        sts_v2=1;
        Message = "Ack." + part1 + "." + "02" + "." + "1";  
        delay(100);
        Message.trim();    
        Tx_data(Message);      
      }
      else if(part3 == "0")
      {
        digitalWrite(D8, LOW);
        delay(200);
        sts_v2=0;  
        Message = "Ack." + part1 + "." + "02" + "." + "0";  
        delay(100);
        Message.trim();               
        Tx_data(Message);               
      }
      else if(part3 == "?")
      {
        if(sts_v2 == "1")
        {
          Message = "Ack." + part1 + "." + "02" + "." + "1";  
          delay(100);
          Message.trim();               
          Tx_data(Message);          
        }
        else if(sts_v2 == "0")
        {
          Message = "Ack." + part1 + "." + "02" + "." + "0";  
          delay(100);
          Message.trim();               
          Tx_data(Message);            
        }
      }
    }
    else if(part2 == "Bt")
    {
          Message = "Ack." + Node_Id + "." + Percentage;  
          delay(100);
          Message.trim();               
          Tx_data(Message);
    }
  }
  else if(part1 != Node_Id)
  {
    if (received != lastTransmitted) 
    { 
      delay(100);
      Tx_data(received);     
      lastTransmitted = received;   
    } 
  }
  received = "";  
  Message ="";
  part1 = ""; 
  part2 = ""; 
  part3 = "";                
}

void Rx_data() {
  int packetSize = LoRa.parsePacket();
  if (packetSize) {
    received = "";
    while (LoRa.available()) {
      char receivedChar = (char)LoRa.read();
      received += receivedChar;
    }
    received.trim();
    Serial.print("Received: ");
    Serial.println(received); 
       
    if (received.startsWith("Tx.") || received.startsWith("Ack.")) 
    {
    received.trim();
    delay(100);
    int start_index = received.indexOf('.') + 1;
    int part1_end_index = received.indexOf('.', start_index);
    part1 = received.substring(start_index, part1_end_index);

    start_index = part1_end_index + 1;
    int part2_end_index = received.indexOf('.', start_index);
    part2 = received.substring(start_index, part2_end_index);

    start_index = part2_end_index + 1;
    part3 = received.substring(start_index);    
    }
    Serial.println(part1);
    Serial.println(part2);
    Serial.println(part3);
  }
}

void Tx_data(String data) {
  delay(200);
  LoRa.beginPacket();
  delay(200);
  digitalWrite(D8, HIGH);
  delay(200);
  digitalWrite(D8, LOW);
  delay(200);
  LoRa.print(data);
  LoRa.endPacket();
  Serial.println(data);
  delay(200); 
}
