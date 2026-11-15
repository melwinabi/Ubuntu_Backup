#include <SoftwareSerial.h>

SoftwareSerial LORA(3, 2);

const int SOL_1         = 5;
const int SOL_2         = 6;

int valveCount          = 0;

String Master_ID        = "3002852";
String Pump_ID          = "2002852";
String Confermation     = "";
String AVST             = "";
String State            = "";
String receivedMessage  = "";
String Data;
String Valve_ID[20];
String Table_ID[20];
String Pump_sts  ;

bool isTableID ;

void setup() {
  Serial.begin(9600);
  LORA.begin(9600);
  delay(100);
  pinMode(SOL_1, OUTPUT);
  pinMode(SOL_2, OUTPUT);
  digitalWrite(SOL_1, LOW);
  digitalWrite(SOL_2, LOW);
  LORA.println("AT"); 
  delay(500);                             
  LORA.println("AT+MODE=TEST");  
  delay(500);           
  LORA.println("AT+TEST=RXLRPKT");
  delay(500); 
  AVST  = "";
  AVST  = "##*AVL*" + Master_ID + "*010";
  Serial.println(AVST);         
  //Serial.println("LoRa Master Ready");
}

void loop() 
{
  if(Serial.available() > 0)
  {
    String IoT_Data = Serial.readStringUntil("\n");
    IoT_Data.trim();
    Data_Handling(IoT_Data);
    IoT_Data = "";
    
  }
  Lora_Rx();
}

void Lora_Rx()
{
  if (LORA.available() > 0) 
  {
    receivedMessage = LORA.readStringUntil('\n');
    receivedMessage.trim(); 
    //Serial.print("Raw Received: ");
   // Serial.println(receivedMessage);    
    int stIndex = receivedMessage.indexOf("RX") + 4;
    int eIndex = receivedMessage.lastIndexOf("\"");
    String hexData = receivedMessage.substring(stIndex, eIndex);
    receivedMessage= "";
   // Serial.println("Hex Payload: " + hexData);
    Data = decodeHex(hexData);
    Data.trim();
    Serial.println(Data);
    if(Data.startsWith("0*##*"))
    {
      Pump_sts ="";
      Pump_sts = "##*" +  Master_ID +"*" + Pump_ID + "*" + "STS" + "*" + "000";
      Serial.println(Pump_sts);    
      digitalWrite(SOL_1, LOW);
      delay(100);      
    }
    else 
    {
    
    }
    Data = Data.substring(2);
    Data_Handling(Data);       
  }
}

void Data_Handling(String Incoming_Value) 
{

  if (Incoming_Value.startsWith("@@*"))
  {
    Confermation = "";
    Confermation = "##*" + Master_ID + "*C@K";
    //Serial.println(Confermation);
    Incoming_Value = Incoming_Value.substring(3);
    if (Incoming_Value.startsWith("$&$*")) 
    {
      //Serial.println("program mode");
      isTableID = true;
      Incoming_Value = Incoming_Value.substring(4);
    } 
    else if (Incoming_Value.startsWith("%$%*")) 
    {
      //Serial.println("Manual mode");
      isTableID = false;
      Incoming_Value = Incoming_Value.substring(4);
    }
    if (Incoming_Value.startsWith(Master_ID)) 
    { 
      Incoming_Value = Incoming_Value.substring(Master_ID.length());
      //Serial.println(Incoming_Value);
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
      State.trim();  
      //Serial.println(State);
      Incoming_Value = Incoming_Value.substring(1);
      if(State == "000" && Incoming_Value.length() > 20)
      {
        //Serial.println("Pump OFF");
        Pump_sts ="";
        Pump_sts = "##*" +  Master_ID +"*" + Pump_ID + "*" + "STS" + "*" + "000";
        Serial.println(Pump_sts);
        digitalWrite(SOL_1, LOW);
        delay(100);
        Incoming_Value = encodeToHex(Incoming_Value);
        Lora_Tx(Incoming_Value);
        //Serial.println(Incoming_Value);
      }
      else 
      {
        Incoming_Value = encodeToHex(Incoming_Value);
        Lora_Tx(Incoming_Value);
        //Serial.println(Incoming_Value);
      }    
    }
  }
  else if (Incoming_Value.startsWith("##*"))
  {
    if(State == "010")
    {
      //Serial.println("Pump ON");
      Pump_sts ="";
      Pump_sts = "##*" +  Master_ID +"*" + Pump_ID + "*" + "STS" + "*" + "010";
      Serial.println(Pump_sts);      
      digitalWrite(SOL_1, HIGH);
      delay(100);
    }
    Serial.println(Incoming_Value);
  }
  else if (Incoming_Value.startsWith("&&*!!!*")) 
  {
    //Serial.println("Pump OFF");
    Pump_sts ="";
    Pump_sts = "##*" +  Master_ID +"*" + Pump_ID + "*" + "STS" + "*" + "000";
    Serial.println(Pump_sts);    
    digitalWrite(SOL_1, LOW);
    delay(100);
    String TotO_OFF = "@@*TTOTFF";
    Serial.println(TotO_OFF);
    TotO_OFF = encodeToHex(TotO_OFF);
    Lora_Tx(TotO_OFF);
    //Serial.println(TotO_OFF);
  } 
  else
  {
    
  }
} 

String decodeHex(String hexString) {
  String result = "";
  for (int i = 0; i < hexString.length(); i += 2) {
    if (i + 1 >= hexString.length()) break; // prevent out-of-bound
    String hexByte = hexString.substring(i, i + 2);
    char c = (char) strtol(hexByte.c_str(), NULL, 16);
    if (isPrintable(c)) result += c;
  }
  return result;
}

void Lora_Tx(String message) {
  if (message.length() == 0) return;
  //Serial.println("Data need to Send >> " + message);
  digitalWrite(SOL_2, HIGH);
  delay(300);
  digitalWrite(SOL_2, LOW);
  delay(300);
  delay(500);
  LORA.println("AT+TEST=TXLRPKT," + message);
  delay(1000); 
  LORA.println("AT+MODE=TEST");
  delay(1000);
  LORA.println("AT+TEST=RXLRPKT");
  delay(500);
}



String encodeToHex(String str) 
{
  String hexString = "";
  //Serial.println("hex ok");
  for (int i = 0; i < str.length(); i++) {
    if ((uint8_t)str[i] < 16) hexString += "0"; // Pad single digit hex
    hexString += String((uint8_t)str[i], HEX);
  }
  return hexString;
}

//@@*$&$*3002852*010*100288001.1*100288002.2
//@@*$&$*3002851*000*100285101.1*100288101.1*100288201.1*100288301.1*100288401.1*100288601.1
////@@*$&$*3002851*010*100288101.1*100288001.2

//&&*!!!3002852*000