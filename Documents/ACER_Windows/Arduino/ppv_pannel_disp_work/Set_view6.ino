void Set_view6()
{
  Flag=45;
  DateTime now = rtc.now();
  lcd.setCursor(1, 1);
  lcd.print("Date: ");
  lcd.print(now.year());
  lcd.print('/');
  lcd.print(now.month());
  lcd.print('/');
  lcd.print(now.day());
  lcd.setCursor(1, 2);
  lcd.print("Time: ");
  lcd.print(now.hour());
  lcd.print(':');
  lcd.print(now.minute());
  lcd.print(':');
  lcd.print(now.second());  
}  
