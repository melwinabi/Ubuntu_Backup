#include<LiquidCrystal.h>
LiquidCrystal lcd(3,4,5,6,7,8);
int fire=2;

void setup(){
lcd.begin(16,2);
lcd.setCursor(0,0);
lcd.print(" LIGHT ");
pinMode(12,INPUT);
pinMode(9,OUTPUT);//led
pinMode(10,OUTPUT);//led fire intrupt out  
pinMode(11,OUTPUT);//bzr
pinMode(2,INPUT);//intrupyt IN
attachInterrupt(digitalPinToInterrupt(2), blink,FALLING);
}
void loop(){
if (digitalRead(12)==HIGH)
{
digitalWrite(9,HIGH);

lcd.setCursor(6,0);
lcd.print(" ON  ");  
}
if (digitalRead(12)==LOW)
{
digitalWrite(9,LOW);  
lcd.setCursor(6,0);
lcd.print(" OFF");  
}

}
 
void blink()
{


digitalWrite(10,HIGH);
delay(500);
digitalWrite(11,HIGH);
delay(1000);
digitalWrite(10,!digitalRead(10));
}
