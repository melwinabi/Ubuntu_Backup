void set_A1() {
  Flag = 3;
  lcd.clear();
  curs(C);
  lcd.setCursor(1, 0);
  lcd.print("Raw Water Pump");
  lcd.setCursor(1, 1);
  lcd.print("Air Blower");
  lcd.setCursor(1, 2);
  lcd.print("Recirculation Pump");
  lcd.setCursor(1, 3);
  lcd.print("Filter Feed Pump");
}

void set_A2() {
  Flag = 4;
  lcd.clear();
  curs(C);
  lcd.setCursor(1, 0);
  lcd.print("Treated Water Pump");
}

void set_B1() {
  Flag = 5;
  lcd.clear();
  curs(C);
  lcd.setCursor(1, 0);
  lcd.print("Raw Water Pump");
  lcd.setCursor(1, 1);
  lcd.print("Air Blower");
  lcd.setCursor(1, 2);
  lcd.print("Recirculation Pump");
  lcd.setCursor(1, 3);
  lcd.print("Filter Feed Pump");
}

void set_B2() {
  Flag = 6;
  lcd.clear();
  curs(0);
  lcd.setCursor(1, 0);
  lcd.print("Treated Water Pump");
}

void set_C1() {
  Flag = 7;
  lcd.clear();
  curs(C);
  lcd.setCursor(1, 0);
  lcd.print("Raw Water Pump");
  lcd.setCursor(1, 1);
  lcd.print("Air Blower");
  lcd.setCursor(1, 2);
  lcd.print("Recirculation Pump");
  lcd.setCursor(1, 3);
  lcd.print("Filter Feed Pump");
}

void set_C2() {
  Flag = 8;
  lcd.clear();
  curs(C);
  lcd.setCursor(1, 0);
  lcd.print("Treated Water Pump");
}

void set_D1() {
  Flag = 9;
  lcd.clear();
  curs(C);
  lcd.setCursor(1, 0);
  lcd.print("Raw Water Pump");
  lcd.setCursor(1, 1);
  lcd.print("Air Blower");
  lcd.setCursor(1, 2);
  lcd.print("Recirculation Pump");
  lcd.setCursor(1, 3);
  lcd.print("Filter Feed Pump");
}

void set_D2() {
  Flag = 10;
  lcd.clear();
  curs(C);
  lcd.setCursor(1, 0);
  lcd.print("Treated Water Pump");
}

void set_E1() {
  Flag = 38;
  lcd.clear();
  curs(C);
  lcd.setCursor(1, 0);
  lcd.print("Raw Water Pump");
  lcd.setCursor(1, 1);
  lcd.print("Air Blower");
  lcd.setCursor(1, 2);
  lcd.print("Recirculation Pump");
  lcd.setCursor(1, 3);
  lcd.print("Filter Feed Pump");
}

void set_E2() {
  Flag = 39;
  lcd.clear();
  curs(C);
  lcd.setCursor(1, 0);
  lcd.print("Treated Water Pump");
}
