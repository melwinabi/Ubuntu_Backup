#include<LiquidCrystal.h>
LiquidCrystal lcd(0,1,2,3,4,5);
void setup(){
pinMode(6,INPUT);
pinMode(7,OUTPUT);  
lcd.begin(16,2);
lcd.setCursor(0,0);
lcd.print("LIGHT");
}
void loop()
{
if(digitalRead(6)==HIGH)
{      
digitalWrite(7,HIGH);
lcd.setCursor(6,0);
lcd.print("ON ");
}
else{
digitalWrite(7,LOW);
lcd.setCursor(6,0);
lcd.print("OFF");  
}
}
