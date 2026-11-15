#include <LoRa.h>
#include <Wire.h>
#include <SoftwareSerial.h>

#define ss 53
#define rst 6
#define dio0 2
#define DE_RE 15

//****************************************************************************
String Master_ID        = "3002854";
//****************************************************************************

String device_name      = "";
String Ack_ID           = "";
String State            = "";
String COM_Data         = "";
String VoltSeq_Sender   = "";
String Past_Name        = "";
String exes_ONOF        = "";
String Exe_Pst          = "";
String PTGLPST_Ack      = "";
String motwrk           = "";
String Confermation     = "";
String Serial_Main;
String Previous_Status;
String RS485_Data;
String WPFO = "";
String dta1, dta2, dta3, dta4, Ref_Valve, data, AVST, Message, TbId, Note1, Note2, Note3, Note4;
String Valve_ID[20];
String Table_ID[20];

int red ;
int yellow = 3;
int green = 5;
int key = 12;
int RELAYON = 26;
int OFRELAY = 27; 
int of_pck = 0;
int o, Pwr, sts, intial_pup_allow, Amps_ok, k , i, valveCount,g;
int z = 0;
int tX_Timer_Flag = 0;
int GSM_Flag = 0;
int ind = 0;
int Volt_ok = 1;
int response_Flag = 5; 
int on_Valve_Count ;
int C_I = 10;
int VALC_SRT = 1;
int kol = 0;
float R_N, Y_N, B_N, R_Y, Y_B, B_R, R_I, Y_I, B_I;

unsigned long Strt_tx = 0;
unsigned long lastTime =0;

static bool btScanAsync = true;
static bool btScanSync = true;

bool isTableID = false;
bool OK_table = false;
bool Check_flag = true;
bool TSMT_Flag = false;
bool AVLFG = true;
bool ON_OF_CKFLG = false;
bool TOT_Flag = false;
bool RSAVLG = false;
bool ProgramFLH = false;
uint8_t response[53];
// BluetoothSerial SerialBT;

// ********************** Declaration completed **********************

void setup()
{
  Serial.begin(115200);
  Serial1.begin(115200); //gsm serial 1 
  Serial2.begin(9600); //rs serial 2
  Serial1.println("##INO");
  LoRa.setPins(ss, rst, dio0);
  Serial.println("SPI Initiate");
  pinMode(red, OUTPUT);
  pinMode(yellow, OUTPUT);
  pinMode(green, OUTPUT);
  pinMode(key, OUTPUT); 
  pinMode(RELAYON, OUTPUT);  
  pinMode(OFRELAY, OUTPUT);
  pinMode(DE_RE, OUTPUT);   
  delay(500);
  Serial.print("LoRa Initiate");
  while (!LoRa.begin(433E6)) {
    Serial.print(".");
    delay(1000);
  }
  Serial.println("LoRa Initiate Sucessfull");
  digitalWrite(DE_RE, LOW);  
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
  delay(50);
  delay(500);
  String available = "##*AVL*"+Master_ID+"*010";
  Serial1.println(available);   
  delay(300);  
}

void loop()
{
  Serial_test();
  Serial1.println("123"); 
  unsigned long Current_millis_one = millis();
  if (Serial1.available() > 0) 
  {
    data = Serial1.readStringUntil('\n');
    data.trim();
    delay(100);
    Serial.println(data);
    Serial.println("GSM_Data" + String(data));
    Data_Handling(data);
  }
  if(Current_millis_one - lastTime >= 10000)
  {
    lastTime = Current_millis_one;
    if(AVLFG == true)
    {
      AVST  = "";
      AVST  = "##*AVL*" + Master_ID + "*010";
      Serial1.println(AVST); 
      Serial.println(AVST); 
      delay(1000);
      Serial1.println(WPFO);
      WPFO = "";
    }
    else
    {

    }
  }

  if (Serial2.available() > 0) 
  {
    RS485_Data = Serial2.readStringUntil('@');
    RS485_Data.trim();
    delay(100);
    if(RS485_Data !=  0)
    {
      Pump_Handel(RS485_Data);
    }
  }  

  if(intial_pup_allow == 1)
  {
    if(on_Valve_Count > 0)
    {
      PUMP_STATE("ON");
    }
    else if(on_Valve_Count < 0)
    {
      PUMP_STATE("OFF");
    }
    intial_pup_allow = 0;
  }

  LoRa_Rx(); 
  Tx_Timer(); 
  valve_Processor(); 
}
void Serial_test()
{
  if (Serial.available() > 0) 
  {
    Serial_Main = Serial.readStringUntil('\n');
    Serial_Main.trim();
    delay(100);
    Serial.println("Main_Serial" + String(Serial_Main));
    if(Serial_Main.startsWith("##*"))
    {
      Lora_Handling(Serial_Main);
    }
    else if(Serial_Main.startsWith("@@*"))
    {
      Data_Handling(Serial_Main);
    }
  }
}

void Data_Handling(String Incoming_Value) 
{
 Serial.println("Part_1 : " + String(Incoming_Value));
 if(Incoming_Value.startsWith("cout")) 
 {
  if (VALC_SRT == 1)
  {
    String Valve_bef_count = Incoming_Value.substring(5);  // need to work
    int count = Valve_bef_count.toInt();
    on_Valve_Count = count;
    VALC_SRT = 0;
  }
 } 
  else if (Incoming_Value.startsWith("@@*"))
  {
    Confermation = "";
    Confermation = "##*" + Master_ID + "*C@K";
    Serial1.println(Confermation);
    Incoming_Value = Incoming_Value.substring(3); // Proper assignment
    Serial.println("Part_2 : " + String(Incoming_Value));
      if (Incoming_Value.startsWith("$&$*")) 
      {
        Serial.println(Incoming_Value);
        Serial.println("program mode");
        isTableID = true;     
        delay(100);
        Incoming_Value = Incoming_Value.substring(4); // Correct
        Serial.println("Part_3 : " + String(Incoming_Value));
      }
      else if (Incoming_Value.startsWith("%$%*")) 
      {
        Serial.println("Manual mode");
        isTableID = false;  
        Incoming_Value = Incoming_Value.substring(4); // Correct
        Serial.println("Part_4 : " + String(Incoming_Value));
      }
      if (Incoming_Value.startsWith(Master_ID)) 
      {
        ProgramFLH= true;
        AVLFG = false;
        AVST  = "";
        AVST  = "##*AVL*" + Master_ID + "*000";
        RSAVLG = false;
        Serial1.println(AVST);
        delay(100); 
        Check_flag = false;
        Incoming_Value = Incoming_Value.substring(Master_ID.length());
        Serial.println("Part_5 : " + String(Incoming_Value));
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
        if(State == "000")
        {
          if(valveCount == on_Valve_Count || valveCount > on_Valve_Count )
          {
            ON_OF_CKFLG = true;
            delay(50);
            PUMP_STATE("OFF");
            on_Valve_Count=0;
          }
        }
        TSMT_Flag = true;
      }
      else
      {
        Serial1.println("##*5001");
      }
  
  }   
}

void Tx_Timer() 
{
 if (tX_Timer_Flag == 1) 
 {
  unsigned long TX_Current_Millis = millis();
   if (TX_Current_Millis - Strt_tx >= 8000)
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
        Serial1.println("##*NFV*" + Master_ID +"*"+ Valve_ID[k] + "*000");
        delay(50);
        k++;      
        if (k >= valveCount) 
        {
          TSMT_Flag = false;
          Serial.println("Program Ends in Timer"); 
          AVLFG = true; 
          AVST ="";
          AVST  = "##*AVL*" + Master_ID + "*010";
          Serial1.print(AVST); 
          delay(50);
          intial_pup_allow = 1;   
          ProgramFLH= false;  
          for (int h = 0; h < 20; h++)
          {
            Table_ID[h] = "";  
            delay(50);
            Valve_ID[h] = "";  
            delay(50);
          }                                    
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

void Uart_Send(String Mode, String ID, String Strength) 
{
  if (isTableID == true) 
  {
    if (OK_table) 
    {
      Table_ID[o].trim();
      if (Table_ID[o] != "")  
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
    Serial.print(".NET");
  } 
  else if (isTableID == false) 
  {
    COM_Data =  "";
    COM_Data = "##*%$%*" + Master_ID + "*" + Mode + "*" + ID + "*" + Strength;
    Serial.print("REACT");
  }
  else
  {

  }
  Serial1.print(COM_Data);// gsm
  delay(50);
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
  if(INCvalve.length() > 0)
  {
  int start = INCvalve.length() - 2;
  String Node = INCvalve.substring(0, start);
  String Valve = INCvalve.substring(start);
  Message = ""; 
  if(Node != "")
  {
    Message = "@@*" + Master_ID + "*" + Node + "*" + Valve + "*" + State;
    Serial.println(Message);
    LoRa_Tx(Message);
    delay(200); 
   }
  } 
}

void LoRa_Tx(String Tx_Data) 
{
  digitalWrite(yellow, HIGH);
  delay(300);
  digitalWrite(yellow, LOW);
  delay(300);    
  digitalWrite(yellow, HIGH);
  delay(300);
  digitalWrite(yellow, LOW);
  delay(300);
  TSMT_Flag = false;
  LoRa.beginPacket();
  delay(300);
  Serial.println(Tx_Data);
  delay(300);
  LoRa.print(Tx_Data);
  delay(300);
  Serial.println("C");
  LoRa.endPacket();
  delay(100);
  Serial.println("D");
  delay(100);
  Serial.println("E");
  tX_Timer_Flag = 1; 
  Serial.println("F");
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
    //Serial.println(received);
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
      //Serial.println("Equal");
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
        Serial.println("Program Ends in Nrml"); 
        AVLFG = true; 
        AVST = "";
        AVST  = "##*AVL*" + Master_ID + "*010";
        Serial1.print(AVST);     //gsm   
        intial_pup_allow = 1; 
        ProgramFLH= false;  
        for (int h = 0; h < 20; h++)
        {
          Table_ID[h] = "";  
          delay(50);
          Valve_ID[h] = "";  
          delay(50);
        }               
       } 
       else
       {
         TSMT_Flag = true;
         delay(50);
       }           
        if(dta2 == "010")
        {
          on_Valve_Count ++;
        }
        else if(dta2 == "000")
        {
          on_Valve_Count--;
        }
       if(on_Valve_Count < 1)
       {
         PUMP_STATE("OFF");
         on_Valve_Count = 0;
       }
       else
       {
         PUMP_STATE("ON");
       }
       Serial.print("on_Valve_Count > ");
       Serial.println(on_Valve_Count);
    }
  }
}


void PUMP_STATE(String Status)
{
  digitalWrite(DE_RE, HIGH);
  delay(5);
  if (Status != Previous_Status)
  {
    Previous_Status = Status;
    if (Status == "ON")
    {
      Serial.println("sts111");
      delay(100);  
      Serial2.println("sts00"); 
    }
    else if (Status == "OFF")
    {
      Serial.println("PUMP_OFF"); 
      delay(100);  
      Serial2.println("PUMP_OFF");  
    }
  }
  Serial2.flush(); 
  delay(5); 
  digitalWrite(DE_RE, LOW);
  delay(5); 
}

void Pump_Handel(String Pump_info)
{
  //Serial.println("RS485_Data : " + Pump_info);
  if(Pump_info.startsWith("WPfO"))
  {
    WPFO = "";
    WPFO = "##*"+Pump_info;
    delay(5);
  }
  else if(Pump_info.startsWith("sts*"))
  {
    int firstStar = Pump_info.indexOf('*');
    String input = "";
    if (firstStar != -1) 
    {
      input = Pump_info.substring(firstStar + 1);
    }
    String Pump_Status ="";
    Pump_Status = "##*" + Master_ID + "*" + input;
    Serial.println("Data_Send > " + Pump_Status);
    Serial1.println(Pump_Status);
    delay(100);
  }
}
//##*3002854*000*100288301*62
//@@*$&$*3002850*010*100285001.1*100285002.2
//@@*$&$*3002850*000*100285101.1*100285101.1*100285201.1*100285301.1*100285401.1*100285601.1
//@@*$&$*3002850*010*100285001.1*100285002.2
//##*3002854*000*100288302*62