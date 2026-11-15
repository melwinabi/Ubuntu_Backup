#include <LiquidCrystal.h>
LiquidCrystal lcd(12, 11, 5, 4, 3, 2);
int pumpPin = 9;
int moisturePin = A0;
int moistureLevel = 0;
int moistureThreshold = 500;


void waterPlants() {
  digitalWrite(pumpPin, HIGH);  
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Watering plants...");
  delay(10000); 
  digitalWrite(pumpPin, LOW);  
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Done watering!");
}

void setup() {
  
  lcd.begin(16, 2);
  lcd.setCursor(0, 0);
  lcd.print("Automatic Irrig.");
  
 
  pinMode(pumpPin, OUTPUT);
  pinMode(moisturePin, INPUT);
}

void loop() {
 
  moistureLevel = analogRead(moisturePin);
  
  
  if (moistureLevel < moistureThreshold) {
    waterPlants();
  } else {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Soil is moist");
  }
  
  delay(1000);  
}