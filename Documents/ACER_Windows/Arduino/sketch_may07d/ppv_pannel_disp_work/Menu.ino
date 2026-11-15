void Menu() {
  S_f = 1;
  Flag = 1;
  lcd.clear();
  curs(C); 
  lcd.setCursor(1, 0);
  lcd.print("Motor Phase");
  lcd.setCursor(1, 1);
  lcd.print("Motor Rating");
  lcd.setCursor(1, 2);
  lcd.print("Pump Mode");
  lcd.setCursor(1, 3);
  lcd.print("Timer");
}

void Menu2() {
  Flag = 2;
  lcd.clear();
  curs(C);
  lcd.setCursor(1, 0);
  lcd.print("Pump Changeover");
  lcd.setCursor(1, 1);
  lcd.print("General");
}
