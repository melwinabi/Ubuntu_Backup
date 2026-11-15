#include <Wire.h>
#include <LiquidCrystal_I2C.h>


LiquidCrystal_I2C lcd(0x27, 20, 4);

void setup() {

  lcd.begin(20, 4);


  lcd.backlight();
}

void loop() {
 
  unsigned long currentTime = millis();
  unsigned long seconds = currentTime / 1000;
  unsigned long minutes = seconds / 60;
  unsigned long hours = minutes / 60;

  // Calculate the time components
  unsigned int secondsComponent = seconds % 60;
  unsigned int minutesComponent = minutes % 60;
  unsigned int hoursComponent = hours % 24;

  
  lcd.setCursor(0, 0);

  lcd.print("Time: ");
  if (hoursComponent < 10) {
    lcd.print("0");
  }
  lcd.print(hoursComponent);
  lcd.print(":");
  if (minutesComponent < 10) {
    lcd.print("0");
  }
  lcd.print(minutesComponent);
  lcd.print(":");
  if (secondsComponent < 10) {
    lcd.print("0");
  }
  lcd.print(secondsComponent);
delay(1000);
lcd.clear();  
}
