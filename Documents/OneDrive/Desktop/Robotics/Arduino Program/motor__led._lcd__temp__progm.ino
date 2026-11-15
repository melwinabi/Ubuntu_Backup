#include<LiquidCrystal.h>
LiquidCrystal lcd(2,5,3,4,11,12);
double temp;
int Sensorpin =A0;
char state;
void setup()
{
  Serial.begin(9600);
  lcd.begin(20,4);
  lcd.setCursor(0,0);
  lcd.print("temperature");
  digitalWrite(6,HIGH);//switch
  pinMode(6,INPUT);
 pinMode(7,OUTPUT);
  pinMode(8,OUTPUT);
  pinMode(9,OUTPUT);
  
  
  
}
void loop()
{
  temp=analogRead (A0);
  temp= temp/1024;
  temp= 5*temp;
  temp=temp +0.5 ;
  temp = temp*100;
  lcd.setCursor(12,0);
  lcd.print(temp);
  if (temp>90)
  {
    digitalWrite(7,HIGH);
    lcd.setCursor(0,1);
    lcd.print("Motor ON ");
    }
    else
    {
     digitalWrite(7,LOW);
     lcd.setCursor(0,1);
     lcd.print("Motor OFF"); 
      }
      if(digitalRead(6)==LOW)
      {
        digitalWrite(9,HIGH);
        lcd.setCursor(0,2);
        lcd.print("led ON ");
      }
      else
      {
       digitalWrite(9,LOW);
       lcd.setCursor(0,2);
        lcd.print("led OFF "); 
      }
      if(Serial.available()>0)
      {
        char state = Serial.read();
        if(state=='A')
        {
          digitalWrite(8,HIGH);
          Serial.println("Led ON");
        }
        if(state=='B')
        {
         digitalWrite(8,LOW);
          Serial.println("Led Off"); 
        }
        
        }
      
      
}
 
