#include<LiquidCrystal.h>
LiquidCrystal lcd(0,1,2,3,4,5);
double temp;
void setup (){
lcd.begin(16,2);
lcd.setCursor(0,0);
lcd.print(" TEMP READING ");
pinMode(8,OUTPUT);
pinMode(9,OUTPUT);
}
void loop(){
temp = analogRead(A0);
temp = double(temp)/1024;
temp=temp * 5;
temp=temp + 0.5;
temp=temp * 100;
lcd.setCursor(1,1);
lcd.setCursor(13,0);
lcd.print(" ") ;
lcd.setCursor(0,1);
lcd.print(temp) ;
delay(5000);
if(temp>89)
{
digitalWrite(8,HIGH);
digitalWrite(9,HIGH);


}
else{
digitalWrite(8,LOW);
digitalWrite(9,LOW);
}
}
