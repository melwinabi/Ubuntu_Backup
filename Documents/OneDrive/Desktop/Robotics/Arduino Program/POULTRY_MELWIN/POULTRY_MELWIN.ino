#include <Wire.h>
#include <LCD_I2C.h>
#include <DHT.h>

#define DHTPIN 5
#define DHTTYPE DHT11

DHT dht(DHTPIN, DHTTYPE);
LCD_I2C lcd(0x27, 16, 2);

void setup() {
  Serial.begin(9600);
  
  lcd.begin();
  dht.begin();
  lcd.backlight();
}

void loop() {
  delay(2000);

  int humidity = dht.readHumidity();
  int temperature = dht.readTemperature();

  if (isnan(humidity) || isnan(temperature)) {
    return;
  }

  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Temp: ");
  lcd.print(temperature);
  Serial.print(temperature);
  lcd.print(" C");

  lcd.setCursor(0, 1);
  lcd.print("Humidity: ");
  lcd.setCursor(10,1);
  lcd.println(humidity);
  lcd.setCursor(12, 1);
  lcd.print("&");
  lcd.setCursor(14, 1);
  lcd.print("  ");
  Serial.print("\t");
  Serial.println(humidity);
 
}
