#include <SoftwareSerial.h>
SoftwareSerial LORA(3, 2); 

String ReceivedMessage;
const int SOL_1 = 5;
const int SOL_2 = 6;    
int Flag = 1;
String Data;

void setup() 
{
  pinMode(SOL_1, OUTPUT);
  pinMode(SOL_2, OUTPUT);
  digitalWrite(SOL_1, LOW); 
  digitalWrite(SOL_2, LOW); 
  Serial.begin(9600);
  LORA.begin(9600);
  LORA.println("AT");
  delay(500);
  LORA.println("AT+MODE=TEST");
  delay(500);   
  LORA.println("AT+TEST=RXLRPKT"); 
  delay(500);
  Serial.println("Slave_2");
}



void loop() 
{    
   if (LORA.available()) 
   
  {
    ReceivedMessage = LORA.readString();
    int startIndex = ReceivedMessage.indexOf("RX") + 4;
    int endIndex = ReceivedMessage.lastIndexOf("\"");
    Data = ReceivedMessage.substring(startIndex, endIndex);  
    Data.trim();
    Serial.println(Data);
  }
  if(Data == "2222222222")
  {
    digitalWrite(SOL_1,HIGH); 
    delay(500);
    Lora_Tx("3310592010"); 
    digitalWrite(SOL_1,LOW);   
  }
}

void Lora_Tx(String message)
{
  LORA.println("AT+TEST=TXLRPKT," + message); 
  digitalWrite(SOL_2,HIGH); 
  delay(100);
  digitalWrite(SOL_2,LOW); 
  delay(100); 
  LORA.println("AT+TEST=RXLRPKT");
  delay(100);
}
