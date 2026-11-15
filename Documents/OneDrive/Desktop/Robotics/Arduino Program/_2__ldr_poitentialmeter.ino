#include<LiquidCryst    al.h>
LiquidCrystal lcd(2,3,4,5,6,7);
int analoginput=A0;
int analoginput2=A1;
void setup()
{
  Serial.begin(9600);
pinMode(8,OUTPUT);
 pinMode(9,OUTPUT);
 
  }
  void loop(){
    analogRead(A0);
    lcd.setCursor(0,0);
    lcd.println(A0);
    analogRead(A1);
    Serial.println(A1);
    
  }
 
