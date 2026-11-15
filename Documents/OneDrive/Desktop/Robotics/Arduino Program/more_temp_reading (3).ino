#include<LiquidCrystal.h>
LiquidCrystal lcd(0,1,2,3,4,5);
double temp;
float voltage;
int sensorpin =A0;
void setup(){
pinMode(10,OUTPUT);
pinMode(9,OUTPUT);
pinMode(8,OUTPUT);
lcd.begin(20,4);
lcd.setCursor(0,0);
lcd.print("temperature:");
}
void loop()
{
double temp=analogRead(A0);
// float voltage = (temp * (5.0/1023.0));
//float value = (voltage-0.5)*100;
temp = double(temp)/1024;
temp=temp * 5;
temp=temp + 0.5;
temp=temp * 100;
lcd.setCursor(1,1);
lcd.print(temp);
if ((temp>50)&&(temp<60))
{
digitalWrite(8,HIGH);
lcd.setCursor(2,2);
lcd.print("led 1 ON "); 
}
else if((temp>60)&&(temp<70))
{
digitalWrite(9,HIGH);
digitalWrite(8,LOW);
lcd.setCursor(2,2);
lcd.print("led 2 ON ");
}
else if((temp>70)&&(temp<80))
{
digitalWrite(10,HIGH);
digitalWrite(9,LOW);
digitalWrite(8,LOW);
lcd.setCursor(2,2);
lcd.print(" led 3 ON ");
}
else if (temp>80)
{
lcd.setCursor(2,2);
digitalWrite(8,LOW);
digitalWrite(9,LOW);
digitalWrite(10,LOW);
 lcd.print("               ");
lcd.setCursor(1,3);

lcd.print(" temp high than 80c ");
}
}
