#include <LoRa.h>
#include <Wire.h>
#define ss 5
#define rst 14
#define dio0 2

String Master_ID = "3002850";
int valve_value = 0;
String Valve_Data[] = {"100285001", "100285101", "100285201", "100285202", "100285301", "100285302", "100285401", "100285402", "100285501", "100285502", "100285601"};
String Node_Data[]  = {"1002850,1002851,1002852,1002853,1002854,1002855,1002856"} ;
bool TSMT_Flag = false;
String Ack_ID = "";
String State = "";
String dta1, dta2, dta3, dta4, Ref_Valve, data;
String COM_Data = "";
String Valve_ID[20];
String Table_ID[20];
int x = 0, z = 0, k = 0, i, valveCount = 0;
int tX_Timer_Flag = 0;
unsigned long Strt_tx = 0;
int Path;
int o;
bool isTableID = false;
bool OK_table = false;
bool Check_flag = true;
String TbId;
void setup()
{
  Serial.begin(115200);
  LoRa.setPins(ss, rst, dio0);
  delay(500);
  
  if (!LoRa.begin(433E6)) {
    //Serial.println("LoRa Failed!!!");
    while (true);
  }

  LoRa.setSpreadingFactor(12);
  LoRa.setSignalBandwidth(62.5E3);
  LoRa.setCodingRate4(5);
  LoRa.setSyncWord(0xA5);
  LoRa.setPreambleLength(8);
  delay(1000);         
}


void loop() {
  LoRa_Rx();
  if (Serial.available() > 0) 
  {
    data = Serial.readStringUntil('\n');
    data.trim();
    //Serial.println("Received: " + data);
    Data_Handling(data);
  }
  valve_Processor();
  Tx_Timer();
}


void Data_Handling(String Incoming_Value) 
{
  if (Incoming_Value == "GSM & PINFO") 
  {
    //Serial.println("Pump OK");
  } 
  else if (Incoming_Value.startsWith("@@"))
  {
    isTableID = false;  
    delay(50);
    Check_flag = false;
    if (Incoming_Value.startsWith("@@*$&$*")) 
    {
      isTableID = true;     
      delay(100);
      //Serial.println(".NET");
    }
    else if (Incoming_Value.startsWith("@@*%$%*")) {
      isTableID = false;  
      //Serial.println("React");
    }

    // Extract the remaining string after the prefix
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
      //Serial.println("State : " + State);
      valveCount = 0;

      for (int i = 2; i < index; i++) 
      {   
        if (isTableID) 
        {
          //Serial.println(".NET Operation");
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
      //Serial.println("TSMT_Flag 1 True");
    } 
    else {
      //Serial.println("Invalid Master ID");
    }
  }
}

void valve_Processor()
{
  if(TSMT_Flag == true)
  {
    Ref_Valve = Valve_ID[k];
    Serial.print("Ref_Valve : ");
    Serial.println(Ref_Valve);
    Valve_Sequencer(Ref_Valve);
    TSMT_Flag = false;  
    //Serial.println("TSMT_Flag False In Valve Processor");    
  }
}

void Valve_Sequencer(String INCvalve)
{
  int start = INCvalve.length() - 2;
  String Node = INCvalve.substring(0, start);
  String Valve = INCvalve.substring(start);
  String Message = "@@*" + Master_ID + "*" + Node + "*" + Valve + "*" + State;
  Serial.println("Valve Sequencer : " + Message);
  LoRa_Tx(Message);
  delay(200);  
}

void LoRa_Tx(String Tx_Data) 
{
  TSMT_Flag = false;
 // Serial.println("TSMT_Flag False In LoRa_Tx");        
 // Serial.println("Starting Transmit");
  LoRa.beginPacket();
  LoRa.print(Tx_Data);
  LoRa.endPacket();
 // Serial.println(Tx_Data);
 // Serial.println("End Transmit");
  tX_Timer_Flag = 1; 
  delay(50);
  Strt_tx = millis();
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
    //Serial.println("LoRa Received: " + received);
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
    Serial.println("FRM_LORA_MAster_ID : " + dta1);
    Serial.println("FRM_LORA_State     : " + dta2);
    Serial.println("FRM_LORA_Valve_ID  : " + dta3);
    Serial.println("FRM_LORA_Signal    : " + dta4);
    if (dta3 == Ref_Valve) 
    {
      tX_Timer_Flag = 0;
      TSMT_Flag = false;
      delay(50);
      //Serial.println("ID Match, transmitting next command...");
      OK_table = true;
      delay(50);
      Uart_Send(dta2,dta3,dta4);
      k++;
      if (k >= valveCount)
       {
        TSMT_Flag = false;
        k = 0;  
        //Serial.println("Program Ends");
        delay(50);
        for (int h = 0; h < 20; h++)
        {
          Table_ID[h] = "";  
          delay(50);
          Valve_ID[h] = "";  
          delay(50);
        }
        valveCount = 0; 
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
    //Serial.println("yes table id");

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
    COM_Data = "##*$&$*" +  TbId + "*" + Master_ID + "*" + Mode + "*" + ID  + "*" + Strength;
  } 
  else 
  {
    COM_Data = "##*%$%" + Master_ID + "*" + Mode + "*" + ID + "*" + Strength;
  }
  Serial.println(COM_Data);
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
     //Serial.println(z);
     TSMT_Flag = true;
     delay(50);
     if(z >= 3)
     {
        z=0;
        OK_table = true;
        Uart_Send("NF",Valve_ID[k],"00");
        k++;      
        if (k >= valveCount) 
        {
          TSMT_Flag = false;
          //Serial.println("Program Ends");
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