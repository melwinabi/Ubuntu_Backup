#include<LiquidCrystal.h>
LiquidCrystal lcd(0,1,2,3,4,5);
int ldr;
void setup (){
lcd.begin(16,2);
lcd.setCursor(0,0);
lcd.print(" LIGHT READING ");
pinMode(8,OUTPUT);
}
void loop(){
ldr = analogRead(A0);
lcd.setCursor(13,0);
lcd.print(" ") ;
lcd.setCursor(0,1);
lcd.print(ldr) ;
delay(2000);
if(analogRead(A0)>50TFX0)
{
digitalWrite(8,LOW);
}
else{
digitalWrite(8,LOW);
}
}
