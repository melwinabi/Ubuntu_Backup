void clockD(){
    lcd.clear();
    lcd.setCursor(1, 1);
    lcd.print("TIME :");
    lcd.setCursor(1, 2);
    lcd.print("DATE : ");
}
void curs(int c) {
  for (int i = 0; i < 4; i++) {
    lcd.setCursor(0, i);
    lcd.print(" ");
  }  
  lcd.setCursor(0, c);
  lcd.print(">");
}
void Curs_2(int r) {
    lcd.setCursor(0, 2);
    lcd.print("                    ");
    lcd.setCursor(r, 2);
    lcd.print("^");
} 
