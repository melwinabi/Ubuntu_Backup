#include <LiquidCrystal.h>
LiquidCrystal lcd(0,1,2,3,4,5);
double temp;
int sensorpin=A0;
void setup()
{
digitalWrite(6,HIGH);  //SWITCH 1
digitalWrite(7,HIGH);
pinMode(6,INPUT);  //SWITCH 1
pinMode(7,INPUT);//SWITCH 2
pinMode(8,OUTPUT);     //LED 1
pinMode(9,OUTPUT);     //LED 2
pinMode(10,OUTPUT);    //LED 3
lcd.begin(20,4); 
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
lcd.setCursor(0,0);
lcd.print("TEMPERATURE: ");
lcd.print(temp);
if (temp >=90)
{
lcd.setCursor(0,1);
lcd.print(" TEMP IS HIGH ");
lcd.setCursor(0,2);
lcd.print(" led one on ");
}
if(digitalRead(6)==HIGH){
digitalWrite(8,HIGH);// led red
digitalWrite(9,LOW); // YELLOW
digitalWrite(10,LOW); // GREEN 
}
else if(digitalRead(6)==LOW)
{
digitalWrite(8,LOW);  // led red
digitalWrite(9,HIGH); // YELLOW
digitalWrite(10,LOW); // GREEN
lcd.setCursor(0,1);
lcd.print("  "); 
lcd.setCursor(0,3);
lcd.print(" TEMPERATURE CONTROLED ");
lcd.setCursor(0,2);
lcd.print("  ");
lcd.setCursor(0,2);
lcd.print("led one on");
}
else if(digitalRead(7)==HIGH){
digitalWrite(8,LOW);// led red
digitalWrite(9,HIGH); // YELLOW
digitalWrite(10,LOW); // GREEN
} 

else if(digitalRead(7)==LOW)
{
digitalWrite(8,LOW);  // led red
digitalWrite(9,LOW); // YELLOW
digitalWrite(10,HIGH); // GREEN
}
else if(digitalRead(6)==LOW)
{
digitalWrite(8,LOW);  // led red
digitalWrite(9,LOW); // YELLOW
digitalWrite(10,HIGH); // GREEN
//lcd.setCursor(0,3);
//lcd.print("  ");
//lcd.setCursor(0,1);
//lcd.print(" TEMPERATURE LOW ");
//lcd.setCursor(0,4);
//lcd.print("  ");
//lcd.setCursor(0,2);
//lcd.print(" led one on ");
}
}
