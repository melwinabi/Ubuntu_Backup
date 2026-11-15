#include <LoRa.h>
#include <Wire.h>

//#include <SoftwareSerial.h>
#define ss 5
#define rst 14
#define dio0 2
#define TX2 17
#define RX2 16  
// #define RS485CommControl 15
// #define RS485CommControl 15
// #define RS485Transmit    HIGH
// #define RS485Receive     LOW

static bool btScanAsync = true;
static bool btScanSync = true;

String dta1, dta2, dta3, dta4, Ref_Valve, data, AVST, Message, TbId, Note1, Note2, Note3, Note4;

String Master_ID = "3002852";
String Pump_ID = "2002852";
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
String available        = "";
String Valve_ID[20];
String Table_ID[20];

int red = 32;
int yellow = 33;
int GSM_Ind = 25;
int RELAYON = 26;
int OFRELAY = 27; 
int of_pck = 0;
int o, Pwr, sts, intial_pup_allow, Amps_ok, k , i, valveCount,g;
int z = 0;
int tX_Timer_Flag = 0;
int GSM_Flag = 0;
int GSM_Reset = 7;
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

void setup() {
  Serial.begin(9600);
  Serial2.begin(115200, SERIAL_8N1, RX2, TX2);
  Serial2.println("##INO");
  LoRa.setPins(ss, rst, dio0);
  pinMode(red, OUTPUT);
  pinMode(yellow, OUTPUT);
  pinMode(GSM_Ind, OUTPUT);
  pinMode(RELAYON, OUTPUT);
  pinMode(OFRELAY, OUTPUT);
  delay(500);
  if (!LoRa.begin(433E6)) {
    Serial.println("LoRa init failed");
    while (true) {
      digitalWrite(yellow, HIGH);
      delay(300);
      digitalWrite(yellow, LOW);
      delay(300);
    }
  }
  digitalWrite(red, HIGH);
  digitalWrite(yellow, HIGH);
  delay(700);
  digitalWrite(red, LOW);
  digitalWrite(yellow, LOW);
  delay(200);
  LoRa.setSpreadingFactor(12);
  LoRa.setSignalBandwidth(62.5E3);
  LoRa.setCodingRate4(5);
  LoRa.setSyncWord(0xA5);
  LoRa.setPreambleLength(8);
  delay(100);
  for (int i = 0; i < 5; i++) {
    digitalWrite(yellow, HIGH);
    delay(300);
    digitalWrite(yellow, LOW);
    delay(300);
  }
  delay(1000);
  available = "";
  available = "##*AVL*" + Master_ID + "*010";
  Serial2.println(available);
  digitalWrite(red, HIGH);
  digitalWrite(GSM_Flag, LOW);
  delay(300);
}


void loop() 
{
   if (Serial2.available() > 0) 
  {
    data = Serial2.readStringUntil('\n');
    data.trim();
    delay(100);
    Serial.println(data);
    Data_Handling(data);
  }

  unsigned long currentTime = millis();
  if (currentTime - lastTime >= 8000) 
  {
    AVLFG = false;
    lastTime = currentTime;
    GSM_Flag++; 
    Serial2.println("##" + String(GSM_Flag));
    available = "";
    available = "##*AVL*" + Master_ID + "*000";
    Serial2.println(available);
  }

  if(AVLFG == false)
  {
    RSAVLG = false;
    AVLFG = true;
    AVST  = "";
    AVST  = "##*AVL*" + Master_ID + "*010";
    Serial2.println(AVST); 
    Pump_Info();    
  } 
  if (GSM_Flag > 2) 
  {
    digitalWrite(GSM_Ind, HIGH);
    delay(300); 
    digitalWrite(GSM_Ind, LOW);
    delay(300);  
    digitalWrite(GSM_Ind, HIGH);
    delay(300);  
    digitalWrite(GSM_Ind, LOW);
    delay(300);     
    digitalWrite(GSM_Ind, HIGH);
    delay(300);  
    digitalWrite(GSM_Ind, LOW);
    delay(300);  
    GSM_Flag = 1; 
  } 
  else if(GSM_Flag <= 2 )
  {
    digitalWrite(GSM_Ind, HIGH);
  }
  valve_Processor();
  LoRa_Rx();
  Tx_Timer();
}

void Data_Handling(String Incoming_Value) 
{
  if (Incoming_Value == "GSMNFO") 
  {
    GSM_Flag = 0;
  }
  else if(Incoming_Value.startsWith("cout")) 
  {
    if (VALC_SRT == 1)
    {
      String Valve_bef_count = Incoming_Value.substring(5);  // need to work
      int count = Valve_bef_count.toInt();
      on_Valve_Count = count;
      if(on_Valve_Count == 0)
      {
        Pump_OFF();
      }
      else
      {
        Pump_ON();
      }
      VALC_SRT = 0;
    }
  }
  else if (Incoming_Value.startsWith("@@*"))
  {
    Confermation = "";
    Confermation = "##*" + Master_ID + "*C@K";
    Serial2.println(Confermation);
    Serial.println(Confermation);
    Incoming_Value = Incoming_Value.substring(3); // Proper assignment
    if((Volt_ok == 1) || (TOT_Flag == true) || (Volt_ok == 0))
    {
      if (Incoming_Value.startsWith("$&$*")) 
      {
        Serial.println("program mode");
        isTableID = true;     
        delay(100);
        Incoming_Value = Incoming_Value.substring(4); // Correct

      }
      else if (Incoming_Value.startsWith("%$%*")) 
      {
        Serial.println("Manual mode");
        isTableID = false;  
        Incoming_Value = Incoming_Value.substring(4); // Correct
      }
      if (Incoming_Value.startsWith(Master_ID)) 
      {
        ProgramFLH= true;
        AVLFG = false;
        AVST  = "";
        AVST  = "##*AVL*" + Master_ID + "*000";
        RSAVLG = false;
        Serial2.println(AVST);
        Serial.println(AVST);
        delay(100); 
        Check_flag = false;
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
          Serial.println("state off");
          if(valveCount == on_Valve_Count || valveCount > on_Valve_Count )
          {
            Serial.println("Need to off");
            delay(50);
            Pump_OFF();
            on_Valve_Count=0;
          }
        }
        TSMT_Flag = true;
      }
      else
      {
        Serial2.println("##*5001");
        Serial.println("##*5001");
      }
   }
   else
   {

   } 
  }
  else if (Incoming_Value.startsWith("&&*"))
  {
    Confermation = "";
    Confermation = "##*" + Master_ID + "*C@K";
    Serial.println(Confermation);
    Serial.println(Confermation);
    Incoming_Value.trim();
    int start_index = Incoming_Value.indexOf('*') + 1;
    int part1_end_index = Incoming_Value.indexOf('*', start_index);
    Note1 = Incoming_Value.substring(start_index, part1_end_index);

    start_index = part1_end_index + 1;
    int part2_end_index = Incoming_Value.indexOf('*', start_index);
    Note2 = Incoming_Value.substring(start_index, part2_end_index);

    start_index = part2_end_index + 1;
    int part3_end_index = Incoming_Value.indexOf('*', start_index);
    Note3 = Incoming_Value.substring(start_index, part3_end_index);

    start_index = part3_end_index + 1;
    Note4 = Incoming_Value.substring(start_index);

    Serial.println(Note1);
    Serial.println(Note2);
    Serial.println(Note3);
    Serial.println(Note4);
    if(Note1 == "!!!")
    { 
      on_Valve_Count = 0 ;
      ON_OF_CKFLG = true;
      TOT_Flag = true;
      Pump_OFF();
      delay(2000);
      String hyper = "";
      hyper = "@@*"+Master_ID+"*111";
      LoRa_Tx(hyper);
    }
    else if(Note1 == Master_ID)
    {
      if(Note2 == "RST")
      {
        //esp_restart(); //Need to change for arduino.........................................
      }
    }
    if(Note2 == Pump_ID)
    {
      if(Note3 == "010")
      {
        Pump_ON();
        ON_OF_CKFLG = false; 
        sts = 1;  
      }
      if(Note3 == "000")
      {
        Pump_OFF();
        ON_OF_CKFLG = false;  
        sts = 0;         
      }      
    }
    Note1 = "";
    Note2 = "";
    Note3 = "";
    Note4 = "";   
  }
  else
  {

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
  if(INCvalve.length() > 0)
  {
  int start = INCvalve.length() - 2;
  String Node = INCvalve.substring(0, start);
  String Valve = INCvalve.substring(start);
  Message = ""; 
  if(Node != "")
  {
    Message = "@@*" + Master_ID + "*" + Node + "*" + Valve + "*" + State;
    Serial.println("Message : " + String(Message));
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
  Serial.println("44");
  LoRa.beginPacket();
  LoRa.print(Tx_Data);
  LoRa.endPacket(); 
  Serial.println("Lora_Transmitted : "+String(Tx_Data)); 
  Serial.println("55");
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
        Serial2.println(AVST);     //gsm  
        Serial.println(AVST); 
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
          if(on_Valve_Count > 0)
          {
            Pump_ON();
          }
        }
        else if(dta2 == "000")
        {
          on_Valve_Count--;
        }
       if(on_Valve_Count < 0)
       {
         on_Valve_Count = 0;
       }
       Serial.print("on_Valve_Count > ");
       Serial.println(on_Valve_Count);
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
    Serial.println(".NET");
  } 
  else if (isTableID == false) 
  {
    COM_Data =  "";
    COM_Data = "##*%$%*" + Master_ID + "*" + Mode + "*" + ID + "*" + Strength;
    Serial.println("REACT");
  }
  else
  {

  }
  Serial2.println(COM_Data);// gsm
  Serial.println(COM_Data);// gsm
  delay(50);
}

void Tx_Timer() 
{
 if (tX_Timer_Flag == 1) 
 {
  unsigned long TX_Current_Millis = millis();
   if (TX_Current_Millis - Strt_tx >= 20000)
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
        Serial2.println("##*NFV*" + Master_ID +"*"+ Valve_ID[k] + "*000");
        Serial.println("##*NFV*" + Master_ID +"*"+ Valve_ID[k] + "*000");
        delay(50);
        k++;      
        if (k >= valveCount) 
        {
          TSMT_Flag = false;
          Serial.println("Program Ends in Timer"); 
          AVLFG = true; 
          AVST ="";
          AVST  = "##*AVL*" + Master_ID + "*010";
          Serial2.print(AVST); 
          Serial.println(AVST); 
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

void Pump_Info()
{
 String PWFRO = " ";
 PWFRO = "##*WPfO*" + Master_ID + "*" + Pump_ID + "*" + R_N + "*"+ Y_N + "*" + B_N + "*" + R_Y + "*" + Y_B + "*" + B_R + "*" + R_I+ "*" + Y_I + "*" + B_I + "*" + Pwr + "*" + sts + "*00.0000000*11.1111111*1" ;
 Serial2.println(PWFRO);
 Serial.println(PWFRO);
 delay(100);
}

void Pump_OFF(){
  digitalWrite(OFRELAY,HIGH);
  delay(3500);
  digitalWrite(OFRELAY,LOW);
  delay(100);
  motwrk = "";
  motwrk = "##*" + Master_ID + "*" + Pump_ID + "*STS*000";      
  Pmp_ACk_TGL(motwrk);  
  Serial.println("off");
}

void Pump_ON(){
  digitalWrite(RELAYON,HIGH);
  delay(5000);
  digitalWrite(RELAYON,LOW); 
  delay(100); 
  motwrk = "";
  motwrk = "##*" + Master_ID + "*" + Pump_ID + "*STS*010";      
  Pmp_ACk_TGL(motwrk);  
  Serial.println("ON");
}

void Pmp_ACk_TGL(String packtgl)
{
 if((packtgl != PTGLPST_Ack) || (TOT_Flag = true))
 {
  PTGLPST_Ack = "";
  PTGLPST_Ack = packtgl;
  Serial2.println(packtgl);
  Serial.println(packtgl);
  TOT_Flag = false;
 }
}
//@@*$&$*3002852*010*100285001.1*100285002.2
//@@*$&$*3002850*000*100285101.1*100285101.1*100285201.1*100285301.1*100285401.1*100285601.1
//@@*$&$*3002850*010*100285001.1*100285002.2
//......
