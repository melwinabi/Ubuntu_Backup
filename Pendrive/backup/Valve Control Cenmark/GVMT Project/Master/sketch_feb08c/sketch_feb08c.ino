#include<LoRa.h>
#include<Wire.h>

#define ss 4 
#define rst 5
#define dio0 2 

String Node_IDs[] = { "1002850", "1002851", "1002852", "1002853", "1002854", "1002855", "1002856" };
String Pump_ID = "2002851";

String part1;
String part2;
String part3;
String part4;
String Percentage_rssi;
String MsgSnr;
String MESSAGE = " ";
String received = " ";

unsigned long previousMillis = 0;
unsigned long currentMillis;
const long interval = 60000;

int Hold = 33; //red
int Start = 34; //green
int Signal = 32; //yellow
int Switch = 1;
int x = 0;
int Timer;

bool ID;

void setup() {
  Serial.begin(9600);
  pinMode(Hold, OUTPUT);
  pinMode(Start, OUTPUT);
  pinMode(Signal, OUTPUT);
  digitalWrite(Hold, HIGH);
  digitalWrite(Start, HIGH);
  digitalWrite(Signal, HIGH);
  delay(1000);
  digitalWrite(Start, LOW);
  digitalWrite(Signal, LOW);
  Serial.print("Pump_ID > ");
  Serial.println(Pump_ID);
  LoRa.setPins(ss, rst, dio0);
  if (!LoRa.begin(433E6)) {
    Serial.println("LoRa initialization failed!");
    while (true);
  }  
  LoRa.setSpreadingFactor(12); 
  LoRa.setSignalBandwidth(62.5E3);  
  LoRa.setCodingRate4(5);     
  LoRa.setSyncWord(0xA5);         
  LoRa.setPreambleLength(8);  
  delay(1000);
  digitalWrite(Hold, LOW);
  digitalWrite(Start, HIGH);
}

void loop() 
{
  COUNT_t();

     Rx_data();

}

void LoRa_Tx(String Data) {
  Serial.println("in");
  LoRa.beginPacket();
  LoRa.print(Data);
  LoRa.endPacket();
  Serial.println(Data);
  digitalWrite(Signal, HIGH);
  delay(100);
  digitalWrite(Signal, LOW);
  delay(100);
  Serial.println("out");
}

void Network_Data(String incomingByte) 
{
  if (incomingByte.startsWith("Tx.") || incomingByte.startsWith("Ack.")) 
  {
    incomingByte.trim();
    int start_index = incomingByte.indexOf('.') + 1;
    int part1_end_index = incomingByte.indexOf('.', start_index);
    part1 = incomingByte.substring(start_index, part1_end_index);
    
    start_index = part1_end_index + 1;
    int part2_end_index = incomingByte.indexOf('.', start_index);
    part2 = incomingByte.substring(start_index, part2_end_index);
    
    start_index = part2_end_index + 1;
    int part3_end_index = incomingByte.indexOf('.', start_index);
    part3 = incomingByte.substring(start_index, part3_end_index);
    
    start_index = part3_end_index + 1;
    part4 = incomingByte.substring(start_index);  
    isValidNode(part1);

  }
}

void COUNT_t() {
    if (Timer == 1) {
        currentMillis = millis();
        
        if (currentMillis - previousMillis >= interval) {
            Serial.println("Timer expired, sending ACK...");
            delay(500);
            Timer = 0; 
            Switch = 1;
        }
    }
}

void Rx_data() {
  int packetSize = LoRa.parsePacket();
  if (packetSize) {
    received = " ";
    while (LoRa.available()) {
      char receivedChar = (char)LoRa.read();
      received += receivedChar;  
    }
    received.trim();
    Serial.println("LoRa received: [" + received + "]");
    Network_Data(received); 
  }  
}

void isValidNode(String incomingData) {
  int totalNodes = sizeof(Node_IDs) / sizeof(Node_IDs[0]);
  for (int i = 0; i < totalNodes; i++) {
    if (incomingData.equals(Node_IDs[i])) 
    {
      ID = true;
      break;
    }
    else
    {  
      ID = false;
      Serial.println("INVALID ID FOUND!");
    }  
  }
}