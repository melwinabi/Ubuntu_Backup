#include<SoftwareSerial.h>
//#include "Wire.h" // For I2C
//#include "LiquidCrystal_I2C.h" 
int ledPin=12,dist,Buzz=11;
SoftwareSerial Port_Debug(2,3);
//LiquidCrystal_I2C lcd(0x27, 20, 4); 
uint8_t cmd[5]={0x55, 0xAA, 0x81, 0x00,0xFA};

typedef struct
{
  int distance;
  uint8_t ErrorCode;
  boolean receiveComplete; 
}
TF;
TF Lidar = {0,0,false};

void getLidarDate(TF* lidar)
{
  static char i=0;
  static int rx[8];
  if (Serial . available ())
  {
   rx[i] = Serial. read () ;
if (rx[0] != 0x55)
{
  i=0;
}else if (i == 1 && rx[1] != 0xAA)
{
i = 0;
} else if (i == 7)
{
i = 0;
if (rx[7] == 0xFA)
{
lidar->distance = rx[5] + rx[4] * 256;
lidar->ErrorCode = rx[6];
lidar->receiveComplete=true;
} 
}
else
{
  i++;
}
}
}
void setup()
{
Serial. begin (115200);
Port_Debug. begin (115200) ;
pinMode(ledPin,OUTPUT);
pinMode(Buzz,OUTPUT);
/* lcd.begin (20,4); 
  lcd.setBacklight(HIGH);
   lcd.setCursor(5, 0); 
  lcd.print("LiRadar");
  lcd.setCursor(4, 1); 
  lcd.print("Testing....");
   lcd.setCursor(0, 2); 
  lcd.print("Initializing...");*/
  delay(3000);
  digitalWrite(ledPin,LOW);
  digitalWrite(Buzz,LOW);
  
}
void loop() 
{

if(dist>100)
{
  digitalWrite(ledPin,HIGH);
  digitalWrite(Buzz,LOW);
}
if(dist<100)
{
  digitalWrite(ledPin,LOW);
  digitalWrite(Buzz,HIGH);
}

if (!Lidar.receiveComplete)
{
  Serial.write(cmd, 5);
} else
{
  Port_Print_Ascii(&Lidar);
  Port_Print_Benewake_9Byte (&Lidar);
  Lidar.receiveComplete = false;
  delay (33);
}
}
void Port_Print_Ascii(TF* lidar)
{
  Port_Debug. print ("Dist = ");
  Port_Debug. println (lidar->distance);
 /* lcd.clear();
  lcd.print("Distance=");
  lcd.print(lidar->distance);*/
  dist= lidar->distance;
if(lidar->ErrorCode)
{
  Port_Debug. print ("ErrorCode = ");
  Port_Debug. println (lidar->ErrorCode, HEX) ;
 /* lcd.clear();
  lcd.print("Distance=");
  lcd.setCursor(0, 1);
   lcd.print(lidar->ErrorCode);*/
}
}
void Port_Print_Benewake_9Byte (TF* lidar)
{
u8 i = 0;
u8 CheckSum = 0;
u8 B_9Byte[9];
B_9Byte[0] = 0x59;
B_9Byte[1] = 0x59;
B_9Byte [2] = lidar->distance & 0xFF;
B_9Byte[3] = lidar->distance >> 8;
B_9Byte[4] = 0x00;
B_9Byte[5] = 0x00;
B_9Byte [6] = 0x00;
B_9Byte[7] = 0x00;
for (i=0;i<7; i++) {
CheckSum += B_9Byte [i];
}
B_9Byte[8] = CheckSum & 0xFF;
Port_Debug. write (B_9Byte, 9) ;
}
void serialEvent()
{
  getLidarDate(&Lidar);
}
