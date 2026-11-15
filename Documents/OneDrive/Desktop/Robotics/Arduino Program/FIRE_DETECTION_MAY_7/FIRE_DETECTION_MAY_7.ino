#include <LiquidCrystal_I2C.h>
#include <Wire.h>
#define buzzer 3
#define fire_pin 2
LiquidCrystal_I2C lcd(0x27,16,2);

void setup() {
  Serial.begin(9600);
  lcd.init();
  lcd.backlight();
  pinMode(fire_pin,INPUT);
  pinMode(buzzer,OUTPUT);
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print("FIRE DETECTION");
    delay(3000);
}

void loop(){
  int fire=digitalRead(fire_pin);
  Serial.println(fire);
  if(fire==LOW){
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print("ALERT....");
    lcd.setCursor(0,1);
    lcd.print("FIRE DETECTED");
    digitalWrite(buzzer,HIGH);
    delay(2000);
    digitalWrite(buzzer,LOW);
     }
     else{
       lcd.setCursor(0,0);
       lcd.print("FIRE DETECTION");
       lcd.setCursor(0,1);
       lcd.print("STATUS NORMAL");
       noTone(buzzer);
      }
      delay(1000);
}
