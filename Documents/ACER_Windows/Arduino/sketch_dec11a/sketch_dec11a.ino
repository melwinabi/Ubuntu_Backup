#include <SPI.h>
#include <LoRa.h>
#define ss 10
#define rst 9
#define dio0 2

int D8 = 8;
int D4 = 4;
int D3 = 3;
int Length;
int x = 1;
int n;
int Flag;
int min = -122; 
int max = -23;  
String mqttstring;
String Node[10];
String incomingByte = "";
String received = "";
String part1;
String part2;
String part3;
String string;
String Message;

void setup() 
{
  Serial.begin(115200);  
  pinMode(D3, OUTPUT);  
  pinMode(D8, OUTPUT); 
  pinMode(D4, OUTPUT); 
  LoRa.setPins(ss, rst, dio0);
  if (!LoRa.begin(433E6)) 
  {
    //Serial.println("LoRa initialization failed. Check connections.");
    while (true); 
  } 
  LoRa.setSyncWord(0xA5);          // Set sync word for receiver matching
  //Serial.println("LoRa Master Initialized");
}

void loop() 
{
   if (Serial.available()) 
   {
     incomingByte = "";
     incomingByte = Serial.readString();
     //Serial.println(incomingByte);
      Alingment();
      mqtt_in();     
   } 
   Rx_data();
   if (received.startsWith("Ack."))
   {
     if(received == "Ack.1002850.01.1")
     {
       digitalWrite(D8, HIGH);     
     }
     else if(received == "Ack.1002850.01.0")
     {
       digitalWrite(D8, LOW);  
     }
     else if(received == "Ack.1002851.01.1")
     {
       digitalWrite(D4, HIGH); 
     }
     else if(received == "Ack.1002851.01.0")
     {
       digitalWrite(D4, LOW);
     } 
     Serial.println(received);
   }
   received=" ";      
}


void Alingment()
{
    if (incomingByte.startsWith("*")) 
    {
      int data1 = incomingByte.indexOf("$");
      if (data1 != -1) {
        string = incomingByte.substring(1, data1);
        incomingByte = " ";
        Length = string.length() / 7;
        //Serial.print("Length: ");
       // Serial.println(Length);
        for (n = 1; n <= Length; n++) 
        {
          int Start = (n - 1) * 7;
          int End = Start + 7;
          if (End <= string.length()) {
            Node[n] = string.substring(Start, End);
            delay(100);
          } 
          else 
          {
           // Serial.println("Error: Invalid substring range.");
            break;
          }
        }
       // Serial.println("Node Parsing Complete");
      } else {
        //Serial.println("Error: Missing '$' in input.");
      }
     // Serial.println(Node[1]);
   } 
}  

void mqtt_in() {
  if (incomingByte.startsWith("Cenmark_PPVTx.")) {
    incomingByte.trim();
    delay(100);
   // Serial.println(incomingByte);
    int start_index = incomingByte.indexOf('.') + 1;
    int part1_end_index = incomingByte.indexOf('.', start_index);
    part1 = incomingByte.substring(start_index, part1_end_index);

    start_index = part1_end_index + 1;
    int part2_end_index = incomingByte.indexOf('.', start_index);
    part2 = incomingByte.substring(start_index, part2_end_index);

    start_index = part2_end_index + 1;
    part3 = incomingByte.substring(start_index);
    
    for (int s = 1 ; s <= Length ; s++)
    {
      if (part1 == Node[s])
      {
        Flag = 1;
      }
    }
   }
    if (Flag == 1)
    {
      Message = "Tx." + part1 + "." + part2 + "." + part3;
      Serial.println(Message);
      delay(100);     
      Tx_data(Message);
      Flag = 0;
    } 
}

void Rx_data()
{
  int packetSize = LoRa.parsePacket();
  if (packetSize) {
    received = " ";
    while (LoRa.available()) {
      char receivedChar = (char)LoRa.read();
      received += receivedChar;
    }
    received.trim();
    float snr = LoRa.packetSnr();
    String MsgSnr= "Snr."+String(snr);
    
    Serial.println(MsgSnr);
    int rssi = LoRa.packetRssi();
        Serial.println(rssi);
    float percentage = calculatePercentage(rssi, min, max);
    Serial.print("Percentage: ");
    Serial.println(percentage);
  }
}

void Tx_data(String data)
{ 
  LoRa.beginPacket();
  digitalWrite(D3, HIGH);
  delay(200);
  digitalWrite(D3, LOW);
  delay(200);
  LoRa.print(data);
  LoRa.endPacket(); 
  Message=" ";   
}

float calculatePercentage(int input, int min, int max) {
  if (input < min) input = min; // Clamp to min
  if (input > max) input = max; // Clamp to max
  return ((float)(input - min) / (max - min)) * 100;
}
