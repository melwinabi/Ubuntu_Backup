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
   float HUM_VALUE  = analogRead(HUM); 
   float humidity = HUM_VALUE /614;
   float hum = humidity*100;
   Serial.println(hum);
   delay(1000);
//   voltage_t /= 1024.0;
//   float temp = (voltage_t - 0.5) * 100; 
//   Serial.print("Current Temperature:"); 
//   Serial.print(temp); 
//   Serial.println(" degrees C");
//
//   int HUM_VALUE = analogRead(HUM);
//   float voltage_h = HUM_VALUE * (5.0 / 1023.0); 
//   float hum = (voltage_h - 0.8) / 0.031;
//   Serial.print("Humidity (%): ");
//   Serial.println(hum);
}
