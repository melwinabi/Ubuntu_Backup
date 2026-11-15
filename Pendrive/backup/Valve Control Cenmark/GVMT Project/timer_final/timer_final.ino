#include <LoRa.h>
#include <Wire.h>
#define ss 5
#define rst 14
#define dio0 2
#define TX2 17
#define RX2 16  

String Master_ID = "3002850";
String Pump_ID = "2002850";
String Valve_Data[] = {"100285001", "100285101", "100285201", "100285202", "100285301", "100285302", "100285401", "100285402", "100285501", "100285502", "100285601"};
String Ack_ID = "";
String State = "";
String dta1, dta2, dta3, dta4, Ref_Valve, data;
String COM_Data = "";
String Valve_ID[20];
String Table_ID[20];
String TbId;
String AVST;
int red = 32;
int yellow = 33;
int green = 25;
int key = 12;
int Path;
int o;
int x = 0, z = 0, k , i, valveCount = 0;
int tX_Timer_Flag = 0;
int GSM_Flag = 0;
int valve_value = 0;

unsigned long Strt_tx = 0;
unsigned long lastTime =0;

bool isTableID = false;
bool OK_table = false;
bool Check_flag = true;
bool TSMT_Flag = false;
bool AVLFG = true;

void setup()
{
  Serial.begin(115200);
  Serial2.begin(115200, SERIAL_8N1, RX2, TX2);  
  LoRa.setPins(ss, rst, dio0);
  pinMode(red, OUTPUT);
  pinMode(yellow, OUTPUT);
  pinMode(green, OUTPUT);
  pinMode(key, OUTPUT);  
  delay(500);
  
  if (!LoRa.begin(433E6)) {
    Serial.println("LoRa Failed!!!");
    while (true);
  }
  digitalWrite(red, HIGH);
  digitalWrite(yellow, HIGH);
  digitalWrite(green, HIGH);
  delay(700);
  digitalWrite(red, LOW);
  digitalWrite(yellow, LOW);
  digitalWrite(green, LOW);  
  delay(200);  
  LoRa.setSpreadingFactor(12);
  LoRa.setSignalBandwidth(62.5E3);
  LoRa.setCodingRate4(5);
  LoRa.setSyncWord(0xA5);
  LoRa.setPreambleLength(8);
  delay(100); 
  digitalWrite(yellow, HIGH);
  delay(300);
  digitalWrite(yellow, LOW);
  delay(300);    
  digitalWrite(yellow, HIGH);
  delay(300);
  digitalWrite(yellow, LOW);
  delay(300); 
  digitalWrite(green, HIGH);
  delay(1000);    
  Serial.print("Master Wake_up ID > ");  
  Serial.println(Master_ID);      
}


void loop() 
{
  unsigned long currentTime = millis();

  if (currentTime - lastTime >= 30000) 
  {
    lastTime = currentTime;
    GSM_Flag++;
    Serial.println(GSM_Flag);
  }  

  LoRa_Rx();
  if(AVLFG == true)
  {
    if (Serial2.available() > 0) 
    {
      data = Serial2.readStringUntil('\n');
      data.trim();
      Serial.println("Received: " + data);
      Data_Handling(data);
    }
  } 
  if (GSM_Flag > 5) 
  {
    Serial.println("Key");
    digitalWrite(key, HIGH);
    delay(3000);    
    digitalWrite(key, LOW);
    delay(1000);
    GSM_Flag = 2; 
  }  
  valve_Processor();
  Tx_Timer();
}


void Data_Handling(String Incoming_Value) 
{
  if (Incoming_Value == "GSMNFO") 
  {
    GSM_Flag = 0;
    Serial2.print("Hnd_ok");
  } 
  else if (Incoming_Value.startsWith("@@"))
  {
    isTableID = false;  
    delay(50);
    Check_flag = false;
    AVLFG = false;
    AVST  = "##*AVL*" + Master_ID + "*000";
    Serial2.println(AVST);
    if (Incoming_Value.startsWith("@@*$&$*")) 
    {
      isTableID = true;     
      delay(100);
    }
    else if (Incoming_Value.startsWith("@@*%$%*")) {
      isTableID = false;  
    }
    Incoming_Value = Incoming_Value.substring(7);
    
    if (Incoming_Value.startsWith(Master_ID)) 
    {
      Incoming_Value = Incoming_Value.substring(Master_ID.length());

      const int maxParts = 20;
      String parts[maxParts];
      int index = 0;
      int start_index = 0;
      int end_index;
      while ((end_index = Incoming_Value.indexOf('*', start_index)) != -1 && index < maxParts) 
      {
        parts[index++] = Incoming_Value.substring(start_index, end_index);
        start_index = end_index + 1;
      }

      if (start_index < Incoming_Value.length() && index < maxParts) 
      {
        parts[index++] = Incoming_Value.substring(start_index);
      }
      State = parts[1];
      valveCount = 0;
      k=0;
      for (int i = 2; i < index; i++) 
      {   
        if (isTableID) 
        {
          int dotIndex = parts[i].indexOf('.');
          if (dotIndex != -1) {
            Table_ID[valveCount] = parts[i].substring(dotIndex + 1);
            Valve_ID[valveCount] = parts[i].substring(0, dotIndex);   
            Serial.print("Valve ID / Table ID [");
            Serial.print(valveCount);
            Serial.print("]: ");
            Serial.print(Valve_ID[valveCount]);
            Serial.print(" / ");
            Serial.println(Table_ID[valveCount]);        
          }
        } 
        else 
        {
          Serial.println("React Operation");
          int dotIndex = parts[i].indexOf('.');
          if (dotIndex != -1) {
            Table_ID[valveCount] = parts[i].substring(dotIndex + 1);
            Valve_ID[valveCount] = parts[i].substring(0, dotIndex);
          } 
          else 
          {
            Valve_ID[valveCount] = parts[i];
            Table_ID[valveCount] = "";  
          }
          Serial.print("Valve ID [");
          Serial.print(valveCount);
          Serial.print("]: ");
          Serial.println(Valve_ID[valveCount]);
        }
        valveCount++;
      }
      Serial.print("Valve_Count: ");
      Serial.println(valveCount);
      TSMT_Flag = true;
      Serial.println("True in handle");
    } 
  }
}

void valve_Processor()
{
  if(TSMT_Flag == true)
  {
    Ref_Valve = Valve_ID[k];
    Serial.println(Ref_Valve);
    Valve_Sequencer(Ref_Valve);
    TSMT_Flag = false;  
  }
}

void Valve_Sequencer(String INCvalve)
{
  Serial.println(INCvalve);
  int start = INCvalve.length() - 2;
  String Node = INCvalve.substring(0, start);
  String Valve = INCvalve.substring(start);
  String Message = "@@*" + Master_ID + "*" + Node + "*" + Valve + "*" + State;
  Serial.println(Message);
  LoRa_Tx(Message);
  delay(200);  
}

void LoRa_Tx(String Tx_Data) 
{
  TSMT_Flag = false;
  LoRa.beginPacket();
  LoRa.print(Tx_Data);
  LoRa.endPacket();
  tX_Timer_Flag = 1; 
  delay(50);
  Strt_tx = millis();
  digitalWrite(yellow, HIGH);
  delay(300);
  digitalWrite(yellow, LOW);
  delay(300);    
  digitalWrite(yellow, HIGH);
  delay(300);
  digitalWrite(yellow, LOW);
  delay(300); 
}

void LoRa_Rx() 
{
  int packetSize = LoRa.parsePacket();
  if (packetSize) 
  {
    String received = "";
    while (LoRa.available()) 
    {
      char receivedChar = (char)LoRa.read();
      received += receivedChar;
    }
    received.trim();
    Serial.println(received);
    Lora_Handling(received);
  }
}

void Lora_Handling(String Lora_Procs) 
{
  if (Lora_Procs.startsWith("##*")) 
  {
    Lora_Procs.trim();

    int Start = Lora_Procs.indexOf('*') + 1;
    int part1_end_index = Lora_Procs.indexOf('*', Start);
    dta1 = Lora_Procs.substring(Start, part1_end_index);

    Start = part1_end_index + 1;
    int part2_end_index = Lora_Procs.indexOf('*', Start);
    dta2 = Lora_Procs.substring(Start, part2_end_index);

    Start = part2_end_index + 1;
    int part3_end_index = Lora_Procs.indexOf('*', Start);
    dta3 = Lora_Procs.substring(Start, part3_end_index);

    Start = part3_end_index + 1;
    dta4 = Lora_Procs.substring(Start);

    dta1.trim();
    dta2.trim();
    dta3.trim();
    dta4.trim();
    if (dta3 == Ref_Valve) 
    {
      Serial.println("Equal");
      tX_Timer_Flag = 0;
      TSMT_Flag = false;
      delay(50);
      OK_table = true;
      delay(50);
      Uart_Send(dta2,dta3,dta4);
      k++;
      if (k >= valveCount)
       {
        TSMT_Flag = false;
        k = 0;  
        delay(50);
        for (int h = 0; h < 20; h++)
        {
          Table_ID[h] = "";  
          delay(50);
          Valve_ID[h] = "";  
          delay(50);
        }
        delay(500);
        AVLFG = true;  
        AVST  = "##*AVL*" + Master_ID + "*010";
        Serial2.print(AVST); 
        Serial.println("Program Ends");       
       } 
       else
       {
         TSMT_Flag = true;
         delay(50);
       } 
    }
  }
}

void Uart_Send(String Mode, String ID, String Strength) 
{
  if (isTableID == true) 
  {
    if (OK_table) 
    {
      if (o <= valveCount)  
      {
        TbId = Table_ID[o];
        o++;
      }
      else
      {
        o = 0;  
        TbId = Table_ID[o];  
        o++;
      }
      OK_table = false;
    }  
    COM_Data =  "";
    COM_Data = "##*$&$*" +  TbId + "*" + Master_ID + "*" + Mode + "*" + ID  + "*" + Strength;
  } 
  else 
  {
    COM_Data =  "";
    COM_Data = "##*%$%*" + Master_ID + "*" + Mode + "*" + ID + "*" + Strength;
  }

  Serial2.print(COM_Data);
  delay(50);
}

void Tx_Timer() 
{
 if (tX_Timer_Flag == 1) 
 {
  unsigned long TX_Current_Millis = millis();
   if (TX_Current_Millis - Strt_tx >= 10000)
   {
     Strt_tx = TX_Current_Millis; 
     z++;
     TSMT_Flag = true;
     delay(50);
     if(z >= 3)
     {
        Serial.println("Timer over");
        z=0;
        OK_table = true;
        Serial2.println("##*NF*" + Valve_ID[k] + "*000");
        delay(50);
        k++;      
        if (k >= valveCount) 
        {
        for (int p = 0; p < 20; p++)
         {
           Table_ID[p] = "";  
           delay(50);
           Valve_ID[p] = "";  
           delay(50);
         }
          TSMT_Flag = false;
          delay(500);
          AVST  = "##*AVL*" + Master_ID + "*010";
          Serial2.print(AVST);           
          AVLFG = true; 
          Serial.println("Program Ends");                  
        }
        else
        {
          TSMT_Flag = true;
          delay(50);
        }
        delay(50);
        tX_Timer_Flag = 0;  
     }
   }
 }
}


//@@*$&$*3002850*010*100285001.1