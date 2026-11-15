
#include <SoftwareSerial.h>

const int SW_1    = A0;
const int SW_2    = A1;
const int SW_3    = A2;
const int SW_4    = A3;
const int SW_5    = A4;
const int SW_6    = A5;
const int SW_7    = A6;
const int SW_8    = A7;

const int BUZZER  = 13; // for External 10
const int RELAY_1 = 10;

int SW_1_STATUS = 0;
int SW_2_STATUS = 0;
int SW_3_STATUS = 0;
int SW_4_STATUS = 0;
int SW_5_STATUS = 0;
int SW_6_STATUS = 0;
int SW_7_STATUS = 0;
int SW_8_STATUS = 0;

SoftwareSerial RS_232(2, 3); // RX, TX

int x;
int id = 0;
String str;


String readString;
String weight;
int weightInt = 0;
int StringLocation;
int Loop = 0;

void setup()  
{
  Serial.begin(115200);

  pinMode(BUZZER,  OUTPUT);
  pinMode(RELAY_1, OUTPUT);

  pinMode(SW_1, INPUT);
  pinMode(SW_2, INPUT);
  pinMode(SW_3, INPUT);
  pinMode(SW_4, INPUT);
  pinMode(SW_5, INPUT);
  pinMode(SW_6, INPUT);
}

void loop() 
{
  SW_1_STATUS = digitalRead(SW_1);
  SW_2_STATUS = digitalRead(SW_2);
  SW_3_STATUS = digitalRead(SW_3);
  SW_4_STATUS = digitalRead(SW_4);
  SW_5_STATUS = digitalRead(SW_5);
  SW_6_STATUS = digitalRead(SW_6);
  SW_7_STATUS = digitalRead(SW_7);
  SW_8_STATUS = digitalRead(SW_8);

  if(SW_1_STATUS == HIGH)
  {
    id = 1;
    delay(10);
    ReadWeight();
    
  }
  if(SW_2_STATUS == HIGH)
  {
    id = 2;
    delay(10);
    ReadWeight();
  }
  if(SW_3_STATUS == HIGH)
  {
    id = 3;
    delay(10);
    ReadWeight();
  }
  if(SW_4_STATUS == HIGH)
  {
    id = 4;
    delay(10);
    ReadWeight();
  }
  if(SW_5_STATUS == HIGH)
  {
    id = 5;
    delay(10);
    ReadWeight();
  }
  if(SW_6_STATUS == HIGH)
  {
    id = 6;
    delay(10);
    ReadWeight();
  }

  RS_232.flush();
  while (Serial.available()) 
  {
    char c = Serial.read();
    if (c == 'A')
    {
      digitalWrite(BUZZER, HIGH); // 
      digitalWrite(RELAY_1, HIGH);
      delay(5000);
      digitalWrite(BUZZER, LOW);
      digitalWrite(RELAY_1, LOW);
      c = "";
    }
    else
    {
      digitalWrite(BUZZER, LOW);
      digitalWrite(RELAY_1, LOW);
      c = "";
    }
  }
}

void ReadWeight()
{
  digitalWrite(BUZZER, HIGH);
  digitalWrite(RELAY_1, HIGH);
  delay(1000);
  digitalWrite(BUZZER, LOW);
  digitalWrite(RELAY_1, LOW);
  delay(1000);
  Loop = 1;
  RS_232.begin(9600);
  delay(100);
  while (Loop == 1)
  {
  if (RS_232.available() >0) 
  {
    char c = RS_232.read();
    if (c == 0x0A) 
    {
      StringLocation = readString.indexOf(',');
      weight = readString.substring(0, StringLocation);
      weightInt = (weight.toFloat()*1000);
      if(weight != "")
      {
        if(weightInt > 0)
        {
          Serial.print(weightInt);
          Serial.print(",");
          Serial.print(id);
          Serial.print("*");
          readString="";
          Loop = 0;
          weight = "";
          weightInt = 0;
          RS_232.end();
        }
        else
        {
          readString="";
          Loop = 0;
          weight = "";
          weightInt = 0;
          RS_232.end();
          ReadWeight();
        }
      }
      else
      {
        RS_232.flush();
      }
    }
    else if (c != 0x20)
    {   
      readString += c; 
    }
  }
  }
}
