void Set_view1(int n){
  Flag = 13;
  lcd.clear();
  C=1;
  curs(C);
  if(n==1){
    Flag=16;
    lcd.setCursor(1, 0);
    lcd.print("  RAW WATER PUMB");
    lcd.setCursor(2, 1);
    lcd.print(case1);
    lcd.setCursor(4, 1);
    lcd.print("PHASE");
    lcd.setCursor(1, 3);
    lcd.print("  [SELECT PHASE]");
  }
  if(n==2){
    Flag=17;
    lcd.setCursor(1, 0);
    lcd.print("  AIR BLOWER");
    lcd.setCursor(2, 1);
    lcd.print(case2);
    lcd.setCursor(4, 1);
    lcd.print("PHASE");
    lcd.setCursor(1, 3);
    lcd.print("  [SELECT PHASE]");
  }
  if(n==3){
    Flag=18;
    lcd.setCursor(1, 0);
    lcd.print(" RECIRCULATION PUMB");
    lcd.setCursor(2, 1);
    lcd.print(case3);
    lcd.setCursor(4, 1);
    lcd.print("PHASE");
    lcd.setCursor(1, 3);
    lcd.print("  [SELECT PHASE]");
  }
  if(n==4){
    Flag=19;
    lcd.setCursor(1, 0);
    lcd.print(" FILTER FEED PUMB");
    lcd.setCursor(2, 1);
    lcd.print(case4);
    lcd.setCursor(4, 1);
    lcd.print("PHASE");
    lcd.setCursor(1, 3);
    lcd.print("  [SELECT PHASE]");
  }
  if(n==5){
    Flag=20;
    lcd.setCursor(0, 0);
    lcd.print(" TREATED WATER PUMB");
    lcd.setCursor(2, 1);
    lcd.print(case5);
    lcd.setCursor(4, 1);
    lcd.print("PHASE");
    lcd.setCursor(1, 3);
    lcd.print("  [SELECT PHASE]");
  }
}
