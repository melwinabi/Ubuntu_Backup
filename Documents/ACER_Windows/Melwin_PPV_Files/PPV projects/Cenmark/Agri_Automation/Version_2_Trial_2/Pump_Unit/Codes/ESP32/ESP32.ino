#include <Wire.h>

#define SLAVE_ADDR 0x08
#define SDA_PIN 21
#define SCL_PIN 22
#define I2C_CHUNK 30
#define RXD2 16
#define TXD2 17
#define DERE_PIN 15 
#define START_PIN 12
#define STOP_PIN  14

//*********************************************************************************
String Pump_ID = "2002855";
//*********************************************************************************

String rxData = "";
String beforePercent = "";
String afterPercent  = "";
String Previous_sts  = "";

void setup()
{
  Serial.begin(115200);
  Serial2.begin(9600, SERIAL_8N1, RXD2, TXD2);
  Wire.begin(SDA_PIN, SCL_PIN);
  pinMode(DERE_PIN, OUTPUT);
  pinMode(START_PIN, OUTPUT);
  pinMode(STOP_PIN, OUTPUT);
  digitalWrite(DERE_PIN, LOW);
  digitalWrite(START_PIN, LOW);
  digitalWrite(STOP_PIN, LOW);  
  delay(1000);  
}

void loop()
{
  Status_Listener();
  Test_Serial();
  String rxData = "";
  if (Serial2.available() > 0 ) {
    rxData = Serial2.readStringUntil('\n');  // read until newline
    rxData.trim();                            // remove \r \n spaces
    Serial.println("Received:");
    Serial.println(rxData);
    Pump_Handling(rxData);
  }  
  while (true)
  {
    Wire.requestFrom(SLAVE_ADDR, I2C_CHUNK);

    while (Wire.available())
    {
      char c = Wire.read();
      rxData += c;

      if (c == '\n')   // end marker
        goto DONE;
    }

    delay(5); // small gap between requests
  }
DONE:
  rxData.trim();
  Data_Handling(rxData);
  delay(3000);
}

void Data_Handling(String input)
{
  if (input.endsWith("$")) {
    input.remove(input.length() - 1);
  }
  int percentIndex = input.indexOf('%');

  if (percentIndex != -1) {
    beforePercent = input.substring(0, percentIndex);      // upto *%
    //afterPercent  = input.substring(percentIndex + 1);     // CT
  }
  String Rs_info = "";
  Rs_info = "WPfO*" + String(Pump_ID) + "*" + String(beforePercent) + "1*" + String(afterPercent) + "*00.0000000*11.1111111@" + "\n";
  Rs485_tx(Rs_info);
}
void Rs485_tx(String Rs_Data)
{
  Serial2.flush(); 
  digitalWrite(DERE_PIN, HIGH);   // 🔼 TX MODE
  delay(2);
  for (int i = 0; i < Rs_Data.length(); i++) {
    char c = Rs_Data[i];
    Serial2.write(c);             // send one char
    Serial.write(c);              // debug

    if (c == '\n')
    {
      Serial2.println("\t\n"); 
      delay(10);
      break;         // stop at newline
      delay(1);  
    } 
  }
  Serial2.flush();                // wait till sent
  delay(2);
  digitalWrite(DERE_PIN, LOW);    // 🔽 RX MODE
  delay(2000);
}

void Pump_Handling(String Input)
{
  if(Input == "sts111")
  {
    digitalWrite(START_PIN, HIGH);
    delay(3000);
    digitalWrite(START_PIN, LOW);
    delay(100);
    Serial.println("PUMP_ON");
    afterPercent = "1";
  }
  else if(Input == "sts000")
  {
    digitalWrite(STOP_PIN, HIGH);
    delay(3000);
    digitalWrite(STOP_PIN, LOW);
    delay(100);
    Serial.println("PUMP_OFF");
    afterPercent = "0";
  }
}
void Status_Listener()
{
  if(afterPercent == "1")
  {
    Status_Sender("010");
  }
  else if(afterPercent == "0")
  {
    Status_Sender("000");
  }
}
void Status_Sender(String Sts)
{
  if(Sts != Previous_sts)
  {
    Previous_sts = Sts;
    String RS_STS = "";
    RS_STS = "sts*" + Pump_ID +"*" +"STS*" + Sts + "@";
    Serial.println(RS_STS);
    Rs485_tx(RS_STS);
  }
}
void Test_Serial()
{
  if(Serial.available()>0)
  {
    String Main_Serial = Serial.readStringUntil('\n');
    Serial.println(Main_Serial);
    Pump_Handling(Main_Serial);
  }
}

//sts*2002845*STS*010 send
//sts111 receive
//master wpfo char length 84