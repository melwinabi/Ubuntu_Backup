#include <LoRa.h>
#include <Wire.h>

#define ss 5
#define rst 4
#define dio0 2
#define TX2 17
#define RX2 16  


String Master_ID = "3002850";
String Node_IDs[] = { "1002850", "1002851", "1002852", "1002853", "1002854", "1002855", "1002856" };
String Valve_Data[] = {"100285001", "100285101", "100285102", "100285201", "100285202", "100285301", "100285302", "100285401", "100285402", "100285501", "100285502", "100285601"};
String part1, part2, part3, part4, part5, part6;
String Pump_ID = "2002850";
String Message = "";

bool State = true;

int led_flag;
int red = 32;
int yellow = 33;
int green = 25;
int key = 12;
int PSR_Vlt = 1;
int PSR_Amps = 1;
int ON_Relay = 27;
int OFF_Relay = 34;
long int GSM_Flag = 0;
long int lastPrintTime = 0;

void setup()
{
  Serial.begin(115200);
  Serial2.begin(115200, SERIAL_8N1, RX2, TX2);
  LoRa.setPins(ss, rst, dio0);
  LoRa.begin(433E6);
  pinMode(red, OUTPUT);
  pinMode(yellow, OUTPUT);
  pinMode(green, OUTPUT);
  pinMode(key, OUTPUT);
  pinMode(ON_Relay, OUTPUT);
  pinMode(OFF_Relay, OUTPUT);  
  digitalWrite(red, HIGH);
  digitalWrite(yellow, HIGH);
  digitalWrite(green, HIGH);
  delay(700);
  digitalWrite(red, LOW);
  digitalWrite(yellow, LOW);
  digitalWrite(green, LOW);  
  delay(200);
  digitalWrite(key, LOW);
  delay(100);
  digitalWrite(key, HIGH);  
  delay(500);
  if (!LoRa.begin(433E6)) 
  {
    Serial.println("LoRa initialization failed!");
    
    while (true)  
    {
      digitalWrite(red, HIGH);
      delay(300);
      digitalWrite(red, LOW);
      delay(300);
    }
  }
  LoRa.setSpreadingFactor(12); 
  LoRa.setSignalBandwidth(62.5E3);  
  LoRa.setCodingRate4(5);     
  LoRa.setSyncWord(0xA5);         
  LoRa.setPreambleLength(8); 
  digitalWrite(red, LOW);
  digitalWrite(yellow, HIGH);
  delay(300);
  digitalWrite(yellow, LOW);
  delay(300);    
  digitalWrite(yellow, HIGH);
  delay(300);
  digitalWrite(yellow, LOW);
  delay(300); 
  digitalWrite(green, HIGH);
  delay(100); 
  Serial.println("ok");
  delay(600);
}

void loop()
{
  unsigned long currentTime = millis();
  if (currentTime - lastPrintTime >= 30000) 
  {
    lastPrintTime = currentTime;
    GSM_Flag++;
    Serial.println(GSM_Flag);
  }
   if(Serial2.available() > 0)
   {
      String Data = Serial2.readStringUntil('\n');
      Serial.println(Data);
      Data_Handling(Data);
   }
   if (GSM_Flag > 3) //value > 4
   {
     Serial.println("Key");
     digitalWrite(key, HIGH);
     delay(3000);    
     digitalWrite(key, LOW);
     delay(1000);
     GSM_Flag = 2; //=3
   }
}

void Data_Handling(String Incoming_Value)
{
  if(Incoming_Value == "GSM & PINFO")
  {
    Serial2.print("Pump OK");  need to send pump info
    GSM_Flag = 0;
  } 
  else if (Incoming_Value.startsWith("@@")) 
  {
   
  }
}
