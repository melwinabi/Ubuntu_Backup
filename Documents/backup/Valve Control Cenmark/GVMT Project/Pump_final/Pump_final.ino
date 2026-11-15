#include <LoRa.h>
#include <Wire.h>
#define ss 5
#define rst 14
#define dio0 2
#define TX2 17
#define RX2 16  
#define RS485CommControl 15
#define RS485Transmit    HIGH
#define RS485Receive     LOW

int RELAYON = 26;
int OFRELAY = 27; 

String Master_ID = "3002850";
String Pump_ID = "2002850";
String Valve_Data[] = {"100285001", "100285101"}; //"100285201", "100285202", "100285301", "100285302", "100285401", "100285402", "100285501", "100285502", "100285601"
String Ack_ID = "";
String State = "";
String dta1, dta2, dta3, dta4, Ref_Valve, data;
String COM_Data = "";
String Valve_ID[20];
String Table_ID[20];
String VLVSTSRY[20];
String TbId;
String AVST;
String Past_Name = "";
String motwrk = "";
String Note1,Note2,Note3,Note4;

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
int for_operation = 0, 
ind;
int Pi = 0;
int Volt_state;
int once_RST;
int AMPS_state;
int vl;
int l = 0 ;
int totflg =0;
int totoff = 0;
int pss = 0;
float R_N, Y_N, B_N, R_Y, Y_B, B_R, R_I, Y_I, B_I, RUN_HR, KWH;

uint8_t response[255];

unsigned long Strt_tx  = 0;
unsigned long lastTime = 0;
unsigned long prumplas = 0;
unsigned long emtimer =0;
bool isTableID = false;
bool OK_table = false;
bool Check_flag = true;
bool TSMT_Flag = false;
bool AVLFG = true;
bool VALP = false;
bool PGEND = false;
bool PUPACH = false;
int PS_flag = 0;
void setup()
{
  Serial.begin(9600);
  Serial2.begin(9600, SERIAL_8N1, RX2, TX2);  
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
    Serial.println("LoRa Failed!!!");
    while (true);
  }
  digitalWrite(RS485CommControl, RS485Receive);
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
  sendEMRequest();   
}


void loop() 
{
  unsigned long currentTime = millis();
  Sequence_reader();
  if (currentTime - emtimer >= 20000) 
  {
    emtimer=currentTime;
    if(vl > 1)
    {
     Volt_state = 0;
     Serial.print("Volt_state :");
     Serial.println(Volt_state);
     motwrk = "";
     motwrk = "##*" + Master_ID + "*EPS*000";
     Serial.println(motwrk);//gsm
    }
  }
  if (currentTime - lastTime >= 30000) 
  {
    lastTime = currentTime;
    GSM_Flag++;
    Serial.println(GSM_Flag);
  }  
  if (currentTime - prumplas >= 8000) 
  {
     Pump_Info();
    if (Serial2.available()) 
    {
     ind = 0;
     while (Serial2.available() && ind < 255) 
     {
      response[ind] = Serial2.read();
      ind++;
     }
     delay(50);
     EMReading();
  } 
  prumplas = currentTime;
  sendEMRequest();      
  }
  LoRa_Rx();
  if(AVLFG == true)
  {
    if (Serial.available() > 0) //gsm
    {
      data = Serial.readStringUntil('\n');//gsm
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
  if(Pi > 0)
  {
    pss = 1;
  }
  else 
  {
    pss = 0;
  }
  valve_Processor();
  Tx_Timer();
}

void sendEMRequest() 
{

  for_operation = 1;
  digitalWrite(RS485CommControl, RS485Transmit);
  delay(100);
  uint8_t request[] = {0x01, 0x04, 0x00, 0x00, 0x00, 0x7D, 0x30, 0x2B};
  Serial2.write(request, sizeof(request));
  delay(10);
  digitalWrite(RS485CommControl, RS485Receive);
  delay(10);
}

void EMReading()
{
  if (ind >= 50) 
    {
      vl = 0;
      R_N = convertHexToFloat(response[5], response[6], response[3], response[4]);
      Y_N = convertHexToFloat(response[9], response[10], response[7], response[8]);
      B_N = convertHexToFloat(response[13], response[14], response[11], response[12]);

      R_Y = convertHexToFloat(response[21], response[22], response[19], response[20]);
      Y_B = convertHexToFloat(response[25], response[26], response[23], response[24]);
      B_R = convertHexToFloat(response[29], response[30], response[27], response[28]);

      R_I = convertHexToFloat(response[37], response[38], response[35], response[36]);
      Y_I = convertHexToFloat(response[41], response[42], response[39], response[40]);
      B_I = convertHexToFloat(response[45], response[46], response[43], response[44]);

      // RUN_HR = convertHexToFloat(response[157], response[158], response[155], response[156]);

      // KWH = convertHexToFloat(response[189], response[190], response[187], response[188]);

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

      // Serial.print("RUN HR: ");
      // Serial.println(RUN_HR, 2);
      // Serial.print("TOTAL KWH: ");
      // Serial.println(KWH, 2);
    }
    else 
    {
      Serial.println("Insufficient data received.");
    }
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
    Serial.print("Hnd_ok");//gsm
  } 
  else if (Incoming_Value.startsWith("@@"))
  {
    if((Volt_state == 1) || (totoff == 1))
    {
      Serial.println("IN HAndling");
      isTableID = false;  
      delay(50);
      Check_flag = false;
      AVLFG = false;
      AVST  = "##*AVL*" + Master_ID + "*000";
      Serial.println(AVST); //gsm
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
        if(State == "000")
        {
          if(valveCount == Pi || valveCount > Pi )
          {
            PUMP_STATE("00");
            TSMT_Flag = true;
          }
        }
        else
        {
          TSMT_Flag = true;
        }
        Serial.print("Valve_Count: ");
        Serial.println(valveCount);
        Serial.println("True in handle");
      } 
   } 
   else
   {
     Serial.println("NOT IN HAndle");
   }
  }
  else if(Incoming_Value.startsWith("##"))
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
      totflg = 1 ;
      Total_off();
    }
    if(Note2 == "RST")
    {
      esp_restart();
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
        Serial.print("State2 : ");
        Serial.print(dta2);   
        AVLFG = true;  
        Serial.println("Program Ends in Nrml"); 
        totoff= 0;
        PGEND = true;
        Serial.println("PGEND"); 
        AVST  = "##*AVL*" + Master_ID + "*010";
        Serial.print(AVST);     //gsm              
       } 
       else
       {
         TSMT_Flag = true;
         delay(50);
       } 
       Pump_Confermation(dta2);
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
  Serial.println("Sending Response");
  Serial.println(COM_Data);//gsm
  delay(100);
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
        Serial.println("##*NFV*" + Valve_ID[k] + "*000");//gsm
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
          delay(500); 
          Serial.print("State2 : ");
          Serial.println(dta2);
          if((Pi > 0) && (dta2 == "010"))
          {
            Serial.println("Need to ON");
            PUMP_STATE("11");
          } 
         TSMT_Flag = false;
         AVLFG = true; 
         Serial.println("Program Ends in Timer"); 
         totoff= 0;
         PGEND = true;
         Serial.println("PGEND"); 
         delay(50);
        AVST  = "##*AVL*" + Master_ID + "*010";
        Serial.print(AVST);     //gsm              
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

void Pump_Confermation(String PUPST)
{

  if (PUPST == "010") 
  {
      Pi++;
  } 
  else if (PUPST == "000") 
  {
      Pi--;
  }

  if (Pi < 0) 
  {
      Pi = 0;
  }

  Serial.print("PI : ");
  Serial.println(Pi);

}

void PUMP_STATE(String Controll)
{
  if(Controll == "00")
  {
    digitalWrite(OFRELAY,HIGH);
    delay(3500);
    digitalWrite(OFRELAY,LOW);
    delay(100);
    Serial.println("Relay OFF");
  }
  else if(Controll == "11")
  {
    if(Volt_state == 1)
    {
      digitalWrite(RELAYON,HIGH);
      delay(5000);
      digitalWrite(RELAYON,LOW); 
      delay(100); 
      Serial.println("Relay ON");
      PUPACH = true;
    }     
  }
}

void Sequence_reader()
{

  if ((R_N > 180) && (Y_N > 180) && (B_N > 180))
  {
    Volt_state = 1;
    if(PS_flag == 1)
    {
      motwrk = "";
      motwrk = "##*" + Master_ID + "*EPS*010";
      Serial.println(motwrk);//gsm
      PS_flag = 0;
    }
  }
  else
  {
    if ((R_N < 180) && (Y_N > 180) && (B_N > 180))
    {
      Phase_Reader("R");
    } 
    else if ((R_N > 180) && (Y_N < 180) && (B_N > 180))
    {
      Phase_Reader("Y");
    } 
    else if ((R_N > 180) && (Y_N > 180) && (B_N < 180))
    {
      Phase_Reader("B");
    }
    else if((R_N < 180) && (Y_N < 180) && (B_N < 180))
    {
      Volt_state = 0;
      PS_flag = 1;
      motwrk = "";
      motwrk = "##*" + Master_ID + "*EPS*000";
      Serial.println(motwrk);//gsm
      if(Pi > 0)
      {
        motwrk = "";
        motwrk = "##*" + Master_ID + "*" + Pump_ID + "*STS*000";
        Serial.println(motwrk);//gsm
      }
    }  
  }
  if (Pi > 0)
  {
    if(PGEND)
    {
      Serial.println("Need to ON"); 
      PUMP_STATE("11");
      PGEND = false;
    }
  } 
  if (R_I >= 0.1)
  {
    AMPS_state = 1;
    if (PUPACH == true)
    {
      motwrk = "";
      motwrk = "##*" + Master_ID + "*" + Pump_ID + "*STS*000";
      Serial.println(motwrk);//gsm
      PUPACH = false;
    }
  }
  else if (R_I < 0.1)
  {
    if(Pi > 0)
    {
      AMPS_state = 0;
      motwrk = "";
      motwrk = "##*"+ Master_ID +"*" + Pump_ID  +"*INT*000";
      Serial.println(motwrk);//gsm
      Serial.println("IN");
      Total_off();
      Serial.println("OUT");
      Pi = 0;
    }
  }
}

void Phase_Reader(String Name)
{ 
  if(Name != Past_Name)
  {
   Past_Name = Name;
   if (Name == "R")
   {
      motwrk = "";
      motwrk = "##*"+ Master_ID +"*PHL*" + Name;
      Serial.println(motwrk);//gsm
   }
   else if (Name == "R")
   {
      motwrk = "";
      motwrk = "##*"+ Master_ID +"*PHL*" + Name;
      Serial.println(motwrk);//gsm    
   }
   else if (Name == "R")
   {
      motwrk = "";
      motwrk = "##*"+ Master_ID +"*PHL*" + Name;
      Serial.println(motwrk);//gsm    
   }      
  } 
}

void Total_off()
{
  totoff = 1;
  String TOTOFMSG = "@@*%$%*" + Master_ID + "*000*";
  for (int l = 0; l < sizeof(Valve_Data) / sizeof(Valve_Data[0]); l++) 
  {  
      TOTOFMSG += Valve_Data[l];
      if (l < (sizeof(Valve_Data) / sizeof(Valve_Data[0])) - 1) 
      {  
          TOTOFMSG += "*";  
      }
  } 
  Serial.println(TOTOFMSG);
  Data_Handling(TOTOFMSG);
}

void Pump_Info()
{
 String PWFRO = "";
 PWFRO = "##*WPfO*" + Master_ID + "*" + Pump_ID + "*" + R_N + "*"+ Y_N + "*" + B_N + "*" + R_Y + "*" + Y_B + "*" + B_R + "*" + R_I+ "*" + Y_I + "*" + B_I + "*" + Volt_state + "*" + pss + "*00.0000000*11.1111111*1" ;
//Serial.println(PWFRO);
}
//@@*$&$*3002850*000*100285001.1