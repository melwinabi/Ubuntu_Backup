#include <SoftwareSerial.h>
SoftwareSerial LORA(3, 2); 

String ReceivedMessage;
const int SOL_1 = 5;
const int SOL_2 = 6;
int Flag = 1;
String Data;
int j = 1;
unsigned long previousMillis = 0;
const unsigned long interval = 3000;
int k,l,m;

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
  Serial.println("Master");
}

void loop()
{
  unsigned long currentMillis = millis(); 
  if(Flag == 1)
  { 
   Lora_Tx("1111111111");
   Flag = 0;
  }
  if (currentMillis - previousMillis >= interval) {
  previousMillis = currentMillis;
  if(j == 1)
  { 
    Flag=1;
  }
  }
  if (LORA.available()) 
  {
    ReceivedMessage = LORA.readString();
    int startIndex = ReceivedMessage.indexOf("RX") + 4;
    int endIndex = ReceivedMessage.lastIndexOf("\"");
    Data = ReceivedMessage.substring(startIndex, endIndex);  
    Serial.println("Extracted message: " + Data);
    Data.trim();
    Serial.println(Data);
  }
  if(Data == "2210591010")
  {
    digitalWrite(SOL_1,HIGH); 
    delay(100);
    Lora_Tx("1122222222");
    j = 0;  
    Flag=0;
  }
  if(Data == "3210592020")
  {
    digitalWrite(SOL_2,HIGH); 
    delay(100);
    Lora_Tx("1111111111"); 
    j = 1;
    Flag=1;
    digitalWrite(SOL_1,LOW);
    digitalWrite(SOL_2,LOW);
  }
}
  
void Lora_Tx(String message)
{
  LORA.println("AT+TEST=TXLRPKT," + message); 
  delay(100);  
  LORA.println("AT+TEST=RXLRPKT");
  delay(100);
}
















//void loop() 
//{    
//  unsigned long currentMillis = millis(); 
//  if (currentMillis - previousMillis >= interval) {
//  previousMillis = currentMillis;
//  if(j == 0)
//  { 
//    Flag=1;
//  }
//  }
//   if (LORA.available()) 
//  {
//    ReceivedMessage = LORA.readString();
//    int startIndex = ReceivedMessage.indexOf("RX") + 4;
//    int endIndex = ReceivedMessage.lastIndexOf("\"");
//    Data = ReceivedMessage.substring(startIndex, endIndex);  
//    Serial.println("Extracted message: " + Data);
//    Data.trim();
//    Serial.println(Data);
//  }
//  if(Data == "2210591010")
//  {
//    digitalWrite(SOL_1,HIGH); 
//    delay(100);
//    Lora_Tx("1122222222");
//    j=1;  
//  }
//  else if(Data == "3210592010")//3210592010
//  {
//    digitalWrite(SOL_2,HIGH);
//    delay(100);
//    Flag=1;
//    j=1;  
//  }
//  if(Flag == 1)
//  {
//   digitalWrite(SOL_1,LOW); 
//   digitalWrite(SOL_2,LOW); 
//   Lora_Tx("1111111111");
//   Flag=0;
//  }
//}
//
