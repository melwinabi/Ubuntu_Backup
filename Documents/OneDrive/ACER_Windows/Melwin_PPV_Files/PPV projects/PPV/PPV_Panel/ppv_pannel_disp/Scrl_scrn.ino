void Scrolling_screen(int y) {
  Flag=0;
  if (y == 1) index_1();
  if (y == 2) index_2();
  if (y == 3) index_3();
  if (y == 4) index_4();
  if (y == 5) index_5();
}

void index_1() {
  lcd.clear();
  lcd.setCursor(1, 0);
  lcd.print("231 242 210");
  lcd.setCursor(13, 0);
  lcd.print(case1);
  lcd.setCursor(14,0);
  lcd.print("*");
  lcd.setCursor(16,0);
  lcd.print(Mode1);
  lcd.setCursor(1, 1);
  lcd.print("R.W.P -> Mode");
  lcd.setCursor(1, 2);
  lcd.print("1 ON | 2 OFF");
  lcd.setCursor(1, 3);
  lcd.print("NS @60 Min");
  delay(2000);
}

void index_2() {
lcd.clear();
  lcd.setCursor(1, 0);
  lcd.print("231 242 210");
  lcd.setCursor(13, 0);
  lcd.print(case2);
  lcd.setCursor(14,0);
  lcd.print("*");
  lcd.setCursor(16,0);
  lcd.print(Mode2);
  lcd.setCursor(1, 1);
  lcd.print("Air Blower -> Mode");
  lcd.setCursor(1, 2);
  lcd.print("1 ON | 2 OFF");
  lcd.setCursor(1, 3);
  lcd.print("NS @60 Min");
  delay(2000);
}

void index_3() {
  lcd.clear();
  lcd.setCursor(1, 0);
  lcd.print("231 242 210");
  lcd.setCursor(13, 0);
  lcd.print(case3);
  lcd.setCursor(14,0);
  lcd.print("*");
  lcd.setCursor(16,0);
  lcd.print(Mode3);
  lcd.setCursor(1, 1);
  lcd.print("R.C.P -> Mode");
  lcd.setCursor(1, 2);
  lcd.print("ON/OFF");
  lcd.setCursor(1, 3);
  lcd.print("NS @60 Min");
  delay(200);
}

void index_4() {
  lcd.clear();
  lcd.setCursor(1, 0);
  lcd.print("231 242 210");
  lcd.setCursor(13, 0);
  lcd.print(case4);
  lcd.setCursor(14,0);
  lcd.print("*");
  lcd.setCursor(16,0);
  lcd.print(Mode4);
  lcd.setCursor(1, 1);
  lcd.print("F.F.P -> Mode");
  lcd.setCursor(1, 2);
  lcd.print("1 ON | 2 OFF");
  lcd.setCursor(1, 3);
  lcd.print("NS @60 Min");
  delay(200);
}

void index_5() {
  lcd.clear();
  lcd.setCursor(1, 0);
  lcd.print("231 242 210");
  lcd.setCursor(13, 0);
  lcd.print(case5);
  lcd.setCursor(14,0);
  lcd.print("*");
  lcd.setCursor(16,0);
  lcd.print(Mode5);
  lcd.setCursor(1, 1);
  lcd.print("T.W.P -> Mode");
  lcd.setCursor(1, 2);
  lcd.print("1 ON | 2 OFF");
  lcd.setCursor(1, 3);
  lcd.print("NS @60 Min  20Amps");
  delay(2000);
}
