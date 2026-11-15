void Set_view4(int p){
  if(p==1){
  Flag=32;  
  lcd.setCursor(1, 0);
  lcd.print(" RAW WATER PUMB");
  lcd.setCursor(1, 1);
  lcd.print("Toggle:");
  lcd.setCursor(9,1);
  lcd.print(PCOT1);
  if(PCOT1=="ON"){
     lcd.setCursor(11,1);
     lcd.print(" ");
  }
  lcd.setCursor(1, 2);
  lcd.print("Pb1:");
  lcd.setCursor(6,2);
  lcd.print(RWPb1);
  if(RWPb1=="ON"){
     lcd.setCursor(8,2);
     lcd.print(" ");
  }
  lcd.setCursor(11, 2);
  lcd.print("Pb2:");
  lcd.setCursor(16,2);
  lcd.print(RWPb2);
  if(RWPb2=="ON"){
     lcd.setCursor(18,2);
     lcd.print(" ");
  }
  lcd.setCursor(1,3);
  lcd.print(LCD_msg);
  }
 
  if(p==2){
  Flag=33;  
  lcd.setCursor(1, 0);
  lcd.print("    AIR BLOWER");
  lcd.setCursor(1, 1);
  lcd.print("Toggle:");
  lcd.setCursor(9,1);
  lcd.print(PCOT2);
  if(PCOT2=="ON"){
     lcd.setCursor(11,1);
     lcd.print(" ");
  }
  lcd.setCursor(1, 2);
  lcd.print("Pb1:");
  lcd.setCursor(6,2);
  lcd.print(RWPb3);
  if(RWPb3=="ON"){
     lcd.setCursor(8,2);
     lcd.print(" ");
  }
  lcd.setCursor(11, 2);
  lcd.print("Pb2:");
  lcd.setCursor(16,2);
  lcd.print(RWPb4);
  if(RWPb4=="ON"){
     lcd.setCursor(18,2);
     lcd.print(" ");
  }
  lcd.setCursor(1,3);
  lcd.print(LCD_msg);
  }

  if(p==3){
  Flag=35;  
  lcd.setCursor(1, 0);
  lcd.print(" FILTER FEED PUMB");
  lcd.setCursor(1, 1);
  lcd.print("Toggle:");
  lcd.setCursor(9,1);
  lcd.print(PCOT4);
  if(PCOT4=="ON"){
     lcd.setCursor(11,1);
     lcd.print(" ");
  }
  lcd.setCursor(1, 2);
  lcd.print("Pb1:");
  lcd.setCursor(6,2);
  lcd.print(RWPb7);
  if(RWPb7=="ON"){
     lcd.setCursor(8,2);
     lcd.print(" ");
  }
  lcd.setCursor(11, 2);
  lcd.print("Pb2:");
  lcd.setCursor(16,2);
  lcd.print(RWPb8);
  if(RWPb8=="ON"){
     lcd.setCursor(18,2);
     lcd.print(" ");
  }
  lcd.setCursor(1,3);
  lcd.print(LCD_msg);
  }
  if(p==4){
  Flag=36;  
  lcd.setCursor(1, 0);
  lcd.print("TREATED WATER PUMB");
  lcd.setCursor(1, 1);
  lcd.print("Toggle:");
  lcd.setCursor(9,1);
  lcd.print(PCOT5);
  if(PCOT5=="ON"){
     lcd.setCursor(11,1);
     lcd.print(" ");
  }
  lcd.setCursor(1, 2);
  lcd.print("Pb1:");
  lcd.setCursor(6,2);
  lcd.print(RWPb9);
  if(RWPb9=="ON"){
     lcd.setCursor(8,2);
     lcd.print(" ");
  }
  lcd.setCursor(11, 2);
  lcd.print("Pb2:");
  lcd.setCursor(16,2);
  lcd.print(RWPb10);
  if(RWPb10=="ON"){
     lcd.setCursor(18,2);
     lcd.print(" ");
  }
  lcd.setCursor(1,3);
  lcd.print(LCD_msg);
  }
}
 
