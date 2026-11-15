#include <LoRa.h>
#include <Wire.h>
#include <EEPROM.h>
#define ss 5
#define rst 14
#define dio0 2
#define TX2 17
#define RX2 16  
#define RS485CommControl 15
#define RS485CommControl 15
#define RS485Transmit    HIGH
#define RS485Receive     LOW

String Master_ID = "3002850";
String Pump_ID = "2002850";
String Valve_Data[] = {"100285001","100285201","100285002","100285301"}; //  "100285301", "100285401", "100285501", "100285601", "100285102", "100285202", "100285302", "100285402", "100285502"}; 
String dta1, dta2, dta3, dta4, Ref_Valve, data;
String Ack_ID = "";
String State = "";
String COM_Data = "";
String Valve_ID[20];
String Table_ID[20];
String TbId;
String Note1,Note2,Note3,Note4;
String VoltSeq_Sender  = "";
String Past_Name = "";
String exes_ONOF = "";
String AVST;
String Exe_Pst ="";
String  PTGLPST_Ack ="";
String motwrk ="";
String Message;
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
int ind = 0;
int RELAYON = 26;
int OFRELAY = 27; 
int OneSequence_togle = 1;
int Volt_ok = 0;
int response_Flag; 
int on_Valve_Count = 0;
int C_I = 0;//temp
int Amps_ok;
int intial_pup_allow;
int Pwr;
int sts;
int case1;

float R_N, Y_N, B_N, R_Y, Y_B, B_R, R_I, Y_I, B_I, RUN_HR, KWH;

unsigned long Strt_tx = 0;
unsigned long lastTime =0;
unsigned long prumplas = 0;
bool isTableID = false;
bool OK_table = false;
bool Check_flag = true;
bool TSMT_Flag = false;
bool AVLFG = true;
bool ON_OF_CKFLG = false;
bool TOT_Flag = false;
uint8_t response[53];

void setup()
{
  Serial.begin(9600);
  Serial2.begin(115200, SERIAL_8N1, RX2, TX2);  
  LoRa.setPins(ss, rst, dio0);
  pinMode(red, OUTPUT);
  pinMode(yellow, OUTPUT);
  pinMode(green, OUTPUT);
  pinMode(key, OUTPUT); 
  pinMode(RELAYON, OUTPUT);
  pinMode(OFRELAY, OUTPUT);
  pinMode(RS485CommControl, OUTPUT);   
  delay(500);
  
  if (!LoRa.begin(433E6)) {
    //Serial.println("LoRa Failed!!!");
    while (true);
    digitalWrite(yellow, HIGH);
    delay(300);
    digitalWrite(yellow, LOW);
    delay(300);    
    digitalWrite(yellow, HIGH);
    delay(300);
    digitalWrite(yellow, LOW);
    delay(300);     
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
  digitalWrite(RS485CommControl, RS485Receive);  
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
  //EEPROM.write(5,0);// need to comment   
  case1 = EEPROM.read(5);
  on_Valve_Count = case1;
  Message = "";
  Message = "##*"+Master_ID+"*RSTOK";
  Serial2.println(Message);
  delay(50);
  Serial.print("on_Valve_Count : ");
  Serial.print(on_Valve_Count);
  //Serial.print("Master Wake_up ID > ");  
  //Serial.println(Master_ID);      
}


void loop() 
{
  volt_sequence();
  unsigned long currentTime = millis();
  if (currentTime - lastTime >= 30000) 
  {
    Serial2.flush();
    sendEMRequest();
    lastTime = currentTime;
    GSM_Flag++;
    //Serial.println(GSM_Flag);
  } 
  if(AVLFG == true)
  {   
    if (Serial2.available() > 0) 
    {
      ind = 0;
      while ((Serial2.available() > 0) && (ind < 53)) 
      {
        response[ind] = Serial2.read();
        ind++;
      }
      delay(50);
      EMReading();
      Serial2.flush();
      //Serial.print("IND :");
      //Serial.println(ind);
    }
    delay(1000);      
  }  
  if(AVLFG == true)
  {   
    if (Serial.available() > 0) 
    {
      data = Serial.readStringUntil('\n');
      data.trim();
      //Serial.println("Received: " + data);
      Data_Handling(data);
    }
    PUMP_Critera();
  }
  if (GSM_Flag > 4) 
  {
    digitalWrite(green, HIGH);
    delay(300);
    digitalWrite(green, LOW);
    delay(300);    
    digitalWrite(green, HIGH);
    delay(300);
    //Serial.println("Key");
    digitalWrite(key, HIGH);
    delay(3000);    
    digitalWrite(key, LOW);
    delay(1000);
    GSM_Flag = 2; 
  } 
  if(response_Flag > 3)
  {
    Volt_ok = 0;
    VoltSeq_Sender = "";
    VoltSeq_Sender = "##*"+ Master_ID +"*EPS*000";
    Togle_pimp_ONOF(VoltSeq_Sender);
  } 
  if(response_Flag <= 3)
  {
    Pwr = 1;
    digitalWrite(red, LOW);
  }
  else
  {
    Pwr = 0;
    digitalWrite(red, LOW);
  }
  valve_Processor();
  LoRa_Rx();
  Tx_Timer();
}


void EMReading()
{
 // Serial.print("INDEX > ");
  // Serial.println(ind);
  if (ind >= 50) 
    {
      response_Flag = 0;
      R_N = convertHexToFloat(response[5], response[6], response[3], response[4]);
      Y_N = convertHexToFloat(response[9], response[10], response[7], response[8]);
      B_N = convertHexToFloat(response[13], response[14], response[11], response[12]);

      R_Y = convertHexToFloat(response[21], response[22], response[19], response[20]);
      Y_B = convertHexToFloat(response[25], response[26], response[23], response[24]);
      B_R = convertHexToFloat(response[29], response[30], response[27], response[28]);

      R_I = convertHexToFloat(response[37], response[38], response[35], response[36]);
      Y_I = convertHexToFloat(response[41], response[42], response[39], response[40]);
      B_I = convertHexToFloat(response[45], response[46], response[43], response[44]);


      // Serial.print("PHASE R-N (V): ");
      // Serial.println(R_N, 2);
      // Serial.print("PHASE Y-N (V): ");
      // Serial.println(Y_N, 2);
      // Serial.print("PHASE B-N (V): ");
      // Serial.println(B_N, 2);

      // Serial.print("PHASE R-Y (V): ");
      // Serial.println(R_Y, 2);
      // Serial.print("PHASE Y-B (V): ");
      // Serial.println(Y_B, 2);
      // Serial.print("PHASE B-R (V): ");
      // Serial.println(B_R, 2);

      // Serial.print("PHASE R (I): ");
      // Serial.println(R_I, 2);
      // Serial.print("PHASE Y (I): ");
      // Serial.println(Y_I, 2);
      // Serial.print("PHASE B (I): ");
      // Serial.println(B_I, 2);
    }
    else 
    {
      //Serial.println("Insufficient data received.");
    }
}
void sendEMRequest() 
{
  response_Flag++;
 // Serial.println("EM Request Sent");
  digitalWrite(RS485CommControl, RS485Transmit);  
  delay(10);  
  uint8_t request[] = {0x01 ,0x04 ,0x00 ,0x00 ,0x00 ,0x18 ,0xF0 ,0x00}; //{0x01, 0x04, 0x00, 0x00, 0x00, 0x7D, 0x30, 0x2B};  
  Serial.write(request, sizeof(request)); // to rs 1
  delay(20);  
  digitalWrite(RS485CommControl, RS485Receive);  
  delay(100);  
}

float convertHexToFloat(uint8_t byte1, uint8_t byte2, uint8_t byte3, uint8_t byte4) 
{
  uint32_t combined = ((uint32_t)byte1 << 24) | ((uint32_t)byte2 << 16) | ((uint32_t)byte3 << 8) | byte4;
  union 
  {
    uint32_t i;
    float f;
  } data;
  data.i = combined;
  if (isfinite(data.f)) 
  {
    return data.f;
  } else 
  {
    //Serial.println("Float value is invalid (ovf).");
    return NAN;
  }
}


void Data_Handling(String Incoming_Value) 
{
  if (Incoming_Value == "GSMNFO") 
  {
    GSM_Flag = 0;
    Pump_Info();
  } 
  else if (Incoming_Value.startsWith("@@"))
  {
    if((Volt_ok == 1) || (TOT_Flag == 1))
    {
      AVLFG = false;
      AVST = "";
      AVST  = "##*AVL*" + Master_ID + "*000";
      Serial2.println(AVST); //gsm
      isTableID = false;  
      delay(50);
      Check_flag = false;
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
            //Serial.println("React Operation");
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
            PUMP_STATE("00");
            on_Valve_Count=0;
          }
        }
        // Serial.print("Valve_Count: ");
        // Serial.println(valveCount);
        TSMT_Flag = true;
        Serial.println("True in handle");
      }
   }
   else
   {
    //Serial.println("Voltage Problem");
    VoltSeq_Sender = "";
    VoltSeq_Sender = "##*" + Master_ID + "*EPS*000";
    Serial2.println(VoltSeq_Sender);   //gsm 
   } 
  }
  else if (Incoming_Value.startsWith("&&"))
  {
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
      TOT_Flag = 1;
      Total_off();
    }
    if(Note2 == "RST")
    {
      esp_restart();
    }
    if(Note2 == Pump_ID)
    {
      if(Note3 == "010")
      {
        digitalWrite(RELAYON,HIGH);
        delay(5000);
        digitalWrite(RELAYON,LOW); 
        delay(100); 
        motwrk = "##*" + Master_ID + "*" + Pump_ID + "*STS*010";      
        Pmp_ACk_TGL(motwrk);
        ON_OF_CKFLG = true; 
        sts = 1;  
      }
      if(Note3 == "000")
      {
        digitalWrite(OFRELAY,HIGH);
        delay(3500);
        digitalWrite(OFRELAY,LOW);
        delay(100);
        //Serial.println("Relay OFF"); 
        motwrk = "";
        motwrk = "##*" + Master_ID + "*" + Pump_ID + "*STS*000";      
        Pmp_ACk_TGL(motwrk);
        ON_OF_CKFLG = true;  
        sts = 0;         
      }      
    }
    Note1 = "";
    Note2 = "";
    Note3 = "";
    Note4 = "";    
  }
}

void valve_Processor()
{
  if(TSMT_Flag == true)
  {
    Ref_Valve = Valve_ID[k];
   // Serial.println(Ref_Valve);
    Valve_Sequencer(Ref_Valve);
    TSMT_Flag = false;  
  }
}

void Valve_Sequencer(String INCvalve)
{
  //Serial.println(INCvalve);
  int start = INCvalve.length() - 2;
  String Node = INCvalve.substring(0, start);
  String Valve = INCvalve.substring(start);
  Message = ""; 
  Message = "@@*" + Master_ID + "*" + Node + "*" + Valve + "*" + State;
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
        for (int h = 0; h < 20; h++)
        {
          Table_ID[h] = "";  
          delay(50);
          Valve_ID[h] = "";  
          delay(50);
        } 
        delay(50);  
        //Serial.println("Program Ends in Nrml"); 
        AVLFG = true; 
        AVST = "";
        AVST  = "##*AVL*" + Master_ID + "*010";
        Serial2.print(AVST);     //gsm   
        intial_pup_allow = 1;        
       } 
       else
       {
         TSMT_Flag = true;
         delay(50);
       } 
       if(dta2 == "010")
       {
         on_Valve_Count ++;
         EEPROM.write(5, on_Valve_Count);
       }
       else if(dta2 == "000")
       {
         on_Valve_Count--;
         EEPROM.write(5, on_Valve_Count);
       }
       if(on_Valve_Count < 0)
       {
         on_Valve_Count = 0;
         EEPROM.write(5, on_Valve_Count);
       }
       //Serial.print("on_Valve_Count > ");
       //Serial.println(on_Valve_Count);
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

  Serial2.print(COM_Data);// gsm
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
        //Serial.println("Timer over");
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
          //Serial.println("Program Ends in Timer"); 
          AVLFG = true; 
          AVST ="";
          AVST  = "##*AVL*" + Master_ID + "*010";
          Serial2.print(AVST); 
          delay(50);
          intial_pup_allow = 1;                             
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

void volt_sequence()
{
  if(response_Flag <= 3)
  {
    
    VoltSeq_Sender = "";
    VoltSeq_Sender = "##*" + Master_ID + "*EPS*010";
    Togle_pimp_ONOF(VoltSeq_Sender);
    Volt_ok = 1;
    digitalWrite(red, LOW);
  } 
  else
  {
    Volt_ok = 0;
    digitalWrite(red, HIGH);
    if ((R_N <= 180) && (Y_N >= 180) && (B_N >= 180))
    {
      Phase_Reader("R");
    } 
    else if ((R_N >= 180) && (Y_N <= 180) && (B_N >= 180))
    {
      Phase_Reader("Y");
    } 
    else if ((R_N >= 180) && (Y_N >= 180) && (B_N <= 180))
    {
      Phase_Reader("B");
    }
  } 
  if(C_I >= 0.1)
  {
      Amps_ok = 1;
  }
  else if(C_I < 0.1) // || use OR
  {
     Amps_ok = 0;
  }    
}

void Phase_Reader(String Name)
{ 

  if(Name != Past_Name)
  { 
   Past_Name = Name;
   if (Name == "R")
   {
      VoltSeq_Sender = "";
      VoltSeq_Sender = "##*"+ Master_ID +"*PHL*" + Name;
      Serial2.println(VoltSeq_Sender);//gsm
   }
   else if (Name == "Y")
   {
      VoltSeq_Sender = "";
      VoltSeq_Sender = "##*"+ Master_ID +"*PHL*" + Name;
      Serial2.println(VoltSeq_Sender);//gsm    
   }
   else if (Name == "B")
   {
      VoltSeq_Sender = "";
      VoltSeq_Sender = "##*"+ Master_ID +"*PHL*" + Name;
      Serial2.println(VoltSeq_Sender);//gsm    
   }      
  } 
}

void Togle_pimp_ONOF(String togle_Pump)
{
   if(togle_Pump != exes_ONOF)
   {
     Serial2.println(togle_Pump); // gsm
     exes_ONOF = togle_Pump;
   }
}

void Pump_Info()
{
 String PWFRO = "";
 PWFRO = "##*WPfO*" + Master_ID + "*" + Pump_ID + "*" + R_N + "*"+ Y_N + "*" + B_N + "*" + R_Y + "*" + Y_B + "*" + B_R + "*" + R_I+ "*" + Y_I + "*" + B_I + "*" + Pwr + "*" + sts + "*00.0000000*11.1111111*1" ;
 Serial2.println(PWFRO);
}
void PUMP_Critera()
{
  if(on_Valve_Count > 0)
  {
    if(Amps_ok == 0)
    {
      PUMP_STATE("11");
    }
    else if(Amps_ok == 1)
    {
      motwrk = "";
      motwrk = "##*" + Master_ID + "*" + Pump_ID + "*STS*010";      
      Pmp_ACk_TGL(motwrk);
    }
  }
  else if((on_Valve_Count == 0) && (intial_pup_allow == 1) )
  {
    PUMP_STATE("00");
    intial_pup_allow == 0;
  }
}

void PUMP_STATE(String Controll)
{
  if(Controll !=  Exe_Pst)
  {
    Exe_Pst = Controll;
    if(Controll == "11")
    {
      digitalWrite(RELAYON,HIGH);
      delay(5000);
      digitalWrite(RELAYON,LOW); 
      delay(100); 
      motwrk = "##*" + Master_ID + "*" + Pump_ID + "*STS*010";      
      Pmp_ACk_TGL(motwrk);
      ON_OF_CKFLG = true; 
      sts = 1;     
    }
    else if(Controll == "00")
    {
      digitalWrite(OFRELAY,HIGH);
      delay(3500);
      digitalWrite(OFRELAY,LOW);
      delay(100);
      //Serial.println("Relay OFF"); 
      motwrk = "";
      motwrk = "##*" + Master_ID + "*" + Pump_ID + "*STS*000";      
      Pmp_ACk_TGL(motwrk);
      ON_OF_CKFLG = true;  
      sts = 0;          
    }
  }
} 

void Pmp_ACk_TGL(String packtgl)
{
 if(packtgl != PTGLPST_Ack)
 {
   PTGLPST_Ack = packtgl;
   Serial2.println(packtgl);
 }
}

void Total_off()
{
  String TOTOFMSG = "@@*%$%*" + Master_ID + "*000*";
  for (int l = 0; l < sizeof(Valve_Data) / sizeof(Valve_Data[0]); l++) 
  {  
      TOTOFMSG += Valve_Data[l];
      if (l < (sizeof(Valve_Data) / sizeof(Valve_Data[0])) - 1) 
      {  
          TOTOFMSG += "*";  
      }
  } 
  //Serial.println(TOTOFMSG);
  Data_Handling(TOTOFMSG);
}
//@@*$&$*3002850*010*100285001.1*100285002.2