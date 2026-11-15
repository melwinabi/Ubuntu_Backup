#include<LiquidCrystal.h>

double temp;
float voltage;
  LiquidCrystal lcd(0,1,2,3,4,5);

int sensorpin =A0;

void setup(){
  

 lcd.begin(20,4);
 lcd.setCursor(0,0);
lcd.print("temperature:");

}

void loop()
{
  double temp=analogRead(A0);
  float voltage = (temp * (5.0/1023.0));
  float value = (voltage-0.5)*100;
 
  
  
 // temp = double(temp)/1024;
  //temp=temp * 5;
  
  //temp=temp + 0.5;
 
 //temp=temp * 100;
 
 
  lcd.setCursor(1,1);
   
  lcd.print(value);
 

}
