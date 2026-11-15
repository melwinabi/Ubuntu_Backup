#include <LiquidCrystal.h>
LiquidCrystal lcd(11,12,10,9,8,7);
void setup() {
  lcd.begin(16,2);
  pinMode(5,OUTPUT);
  digitalWrite (5,HIGH);
}

void loop() {
  
  if (digitalRead (5)== LOW)
  
  {
    lcd.setCursor(1,1);
    digitalWrite(0,HIGH);
  lcd.print("light ON ");
  }
   else
  {
    lcd.setCursor(1,1);
    lcd.print("light off");
    digitalWrite(0,LOW);
  }
}
