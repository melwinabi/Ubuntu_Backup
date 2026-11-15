#include<LiquidCrystal.h>
LiquidCrystal lcd(0,1,2,3,4,5);
void setup(){
lcd.begin(16,2);
lcd.setCursor(0,0);
lcd.print(" MOTOR ");
pinMode(8,INPUT);
pinMode(9,INPUT);
digitalWrite(8,HIGH);
digitalWrite(9,HIGH);
pinMode(10,OUTPUT);
pinMode(11,OUTPUT);
}
void loop(){
if(digitalRead(8)==LOW)
{
digitalWrite(10,HIGH);
digitalWrite(11,LOW);
lcd.setCursor(7,0);
lcd.print("forward  ");
delay(2000);
}
else{
  digitalWrite(10,LOW);
digitalWrite(11,LOW);
lcd.setCursor(7,0);
lcd.print("processing");
  
}
if(digitalRead(9)==LOW)
{
digitalWrite(11,HIGH);
digitalWrite(10,LOW);
lcd.setCursor(7,0);
lcd.print("backward");
delay(2000);
}
else
{
digitalWrite(10,LOW);
digitalWrite(11,LOW);
lcd.setCursor(7,0);
lcd.print("processing");
}
}
