#include <LiquidCrystal_I2C.h>
LiquidCrystal_I2C lcd(0x27, 20, 4);
int Year;
int Month;
int Date;
int Hour;
int Minute;
int Seconds;
void setup() {
  Serial.begin(9600);
  lcd.init();
  lcd.backlight();
}
void loop() 
{
  Serial.print("bgfogkb");
  lcd.setCursor(6,0);
  lcd.print("SET TIME");
  lcd.setCursor(2,1);
  lcd.print("H ");
  lcd.setCursor(4,1);  
  lcd.print("00");
  lcd.print(":");
  lcd.print("M ");
  lcd.setCursor(9,1);
  lcd.print("00");
  lcd.print(":");
  lcd.print("D ");
  lcd.setCursor(14,1);  
  lcd.print("29");
  lcd.setCursor(2,3);
  lcd.print("[SELECT TO FEED]");
}





//void DATE()
//{
// Flag=46;
// lcd.setCursor(6,0);
// lcd.print("SET DATE");
// lcd.setCursor(2,1);
// lcd.print("Y");
// lcd.setCursor(4,1);  
// lcd.print("2024");
// lcd.print(":");
// lcd.print("M");
// lcd.setCursor(11,1);
// lcd.print("09");
// lcd.print(":");
// lcd.print("D");
// lcd.setCursor(16,1);  
// lcd.print("29");
// lcd.setCursor(2,3);
// lcd.print("[SELECT TO FEED]");
//}
//
//void TIME()
//{
//// Flag=47; 
// lcd.setCursor(6,0);
// lcd.print("SET TIME");
// lcd.setCursor(2,1);
// lcd.print("H ");
// lcd.setCursor(4,1);  
// lcd.print("00");
// lcd.print(":");
// lcd.print("M ");
// lcd.setCursor(9,1);
// lcd.print("00");
// lcd.print(":");
// lcd.print("D ");
// lcd.setCursor(14,1);  
// lcd.print("29");
// lcd.setCursor(2,3);
// lcd.print("[SELECT TO FEED]");  
//}
