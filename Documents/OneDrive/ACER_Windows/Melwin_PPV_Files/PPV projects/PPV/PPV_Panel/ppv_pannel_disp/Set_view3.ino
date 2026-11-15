void Set_view3(int m){
  if(m==1){
  Flag=27;  
  lcd.setCursor(1, 0);
  lcd.print(" RAW WATER PUMB");
  lcd.setCursor(2, 1);
  lcd.print("Float: ");
  lcd.setCursor(9,1);
  lcd.print(modeF1);
  if(modeF1=="ON"){
     lcd.setCursor(11,1);
     lcd.print(" ");
  }
  lcd.setCursor(2, 2);
  lcd.print("Timer: ");
  lcd.setCursor(9,2);
  lcd.print(modeT1);
  if(modeT1=="ON"){
     lcd.setCursor(11,2);
     lcd.print(" ");
  }
  lcd.setCursor(2,3);
  lcd.print("[SELECT TO FEED]");
  }
  if(m==2){
  Flag=28;
  lcd.setCursor(1, 0);
  lcd.print("    AIR BLOWER");
  lcd.setCursor(2, 1);
  lcd.print("Float: ");
  lcd.setCursor(9,1);
  lcd.print(modeF2);
  if(modeF2=="ON"){
     lcd.setCursor(11,1);
     lcd.print(" ");
  }
  lcd.setCursor(2, 2);
  lcd.print("Timer: ");
  lcd.setCursor(9,2);
  lcd.print(modeT2);
  if(modeT2=="ON"){
     lcd.setCursor(11,2);
     lcd.print(" ");
  }
  lcd.setCursor(1,3);
  lcd.print("SELECT TO FEED");
  }
  if(m==3){
  Flag=29;  
  lcd.setCursor(1, 0);
  lcd.print("RECIRCULATION PUMB");
  lcd.setCursor(2, 1);
  lcd.print("Float: ");
  lcd.setCursor(9,1);
  lcd.print(modeF3);
  if(modeF3=="ON"){
     lcd.setCursor(11,1);
     lcd.print(" ");
  }
  lcd.setCursor(2, 2);
  lcd.print("Timer: ");
  lcd.setCursor(9,2);
  lcd.print(modeT3);
  if(modeT3=="ON"){
     lcd.setCursor(11,2);
     lcd.print(" ");
  }
  lcd.setCursor(1,3);
  lcd.print("SELECT TO FEED");
  }
  if(m==4){
  Flag=30;  
  lcd.setCursor(1, 0);
  lcd.print(" FILTER FEED PUMB");
  lcd.setCursor(2, 1);
  lcd.print("Float: ");
  lcd.setCursor(9,1);
  lcd.print(modeF4);
  if(modeF4=="ON"){
     lcd.setCursor(11,1);
     lcd.print(" ");
  }
  lcd.setCursor(2, 2);
  lcd.print("Timer: ");
  lcd.setCursor(9,2);
  lcd.print(modeT4);
  if(modeT4=="ON"){
     lcd.setCursor(11,2);
     lcd.print(" ");
  }
  lcd.setCursor(1,3);
  lcd.print("SELECT TO FEED");
  }
  if(m==5){
  Flag=31;
  lcd.setCursor(1, 0);
  lcd.print("TREATED WATER PUMB");
  lcd.setCursor(2, 1);
  lcd.print("Float: ");
  lcd.setCursor(9,1);
  lcd.print(modeF5);
  if(modeF5=="ON"){
     lcd.setCursor(11,1);
     lcd.print(" ");
  }
  lcd.setCursor(2, 2);
  lcd.print("Timer: ");
  lcd.setCursor(9,2);
  lcd.print(modeT5);
  if(modeT5=="ON"){
     lcd.setCursor(11,2);
     lcd.print(" ");
  }
  lcd.setCursor(1,3);
  lcd.print("SELECT TO FEED");
  }
}
