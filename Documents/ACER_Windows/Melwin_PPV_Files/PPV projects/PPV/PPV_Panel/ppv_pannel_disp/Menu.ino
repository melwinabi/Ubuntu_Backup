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

void LOW_ALL()
{
  digitalWrite(LED11, LOW);
  digitalWrite(LED12, LOW);
  digitalWrite(LED21, LOW);
  digitalWrite(LED22, LOW);
  digitalWrite(LED41, LOW);
  digitalWrite(LED42, LOW);
  digitalWrite(LED51, LOW);
  digitalWrite(LED52, LOW);
  digitalWrite(LED3, LOW);
  digitalWrite(LED6, LOW);
}
