#include <SoftwareSerial.h>

const int SOL_1 = 5;
const int SOL_2 = 6;
const int DI    = 11;

const int TEMP  = A1;
const int HUM   = A0;

const int SEN_1 = 7;
const int SEN_2 = 8;
const int SEN_3 = 9;
const int SEN_4 = 10;

const int LORA_RST = 4;

int DI_STATUS = 0;
int SEN_1_STATUS = 0;
int SEN_2_STATUS = 0;
int SEN_3_STATUS = 0;
int SEN_4_STATUS = 0;

int TEMP_VALUE = 0;
int HUM_VALUE = 0;

SoftwareSerial LORA(2, 3); // RX, TX

void setup() 
{
  pinMode(SOL_1, OUTPUT);
  pinMode(SOL_2, OUTPUT);
  pinMode(LORA_RST, OUTPUT);

  pinMode(DI,INPUT);
  pinMode(SEN_1, INPUT);
  pinMode(SEN_2, INPUT);
  pinMode(SEN_3, INPUT);
  pinMode(SEN_4, INPUT);

  Serial.begin(9600);
  LORA.begin(9600);
}

void loop() 
{

}

  // TEMP_VALUE = analogRead(TEMP);
  // HUM_VALUE = analogRead(HUM);

  // if(TEMP_VALUE > 512)
  // {
  //   digitalWrite(SOL_1, HIGH);
  // }
  // else
  // {
  //   digitalWrite(SOL_1, LOW);
  // }

  // if(HUM_VALUE > 512)
  // {
  //   digitalWrite(SOL_2, HIGH);
  // }
  // else
  // {
  //   digitalWrite(SOL_2, LOW);
  // }

  // SEN_1_STATUS = digitalRead(SEN_4);
  // if(SEN_1_STATUS == LOW)
  // {
  //   digitalWrite(SOL_2, HIGH);
  // }
  // else
  // {
  //   digitalWrite(SOL_2, LOW);
  // }

  // digitalWrite(SOL_1, HIGH);
  // delay(3000);
  // digitalWrite(SOL_1, LOW);
  // delay(3000);
  // digitalWrite(SOL_2, HIGH);
  // delay(3000);
  // digitalWrite(SOL_2, LOW);
  // delay(3000);