void Set_view5(int t)
{
  
 if(t == 1)
 {
  Flag=40;
  lcd.setCursor(1, 0);
  lcd.print(" RAW WATER PUMB");
  lcd.setCursor(3, 1);
  lcd.print("H");
  lcd.setCursor(5, 1);
  lcd.print(Sh1);
  lcd.setCursor(8, 1);
  lcd.print(":");
  lcd.setCursor(10, 1);
  lcd.print("M");
  lcd.setCursor(12, 1);
  lcd.print(Sm1);
  if(Sm1 < 10)
  {
   lcd.setCursor(13, 1);
   lcd.print(" ");  
  }
  lcd.setCursor(1, 3);
  lcd.print("[Select to Feed]");  
 }

  if(t == 2)
 {
  Flag=41;
  lcd.setCursor(1, 0); 
  lcd.print("  AIR BLOWER");
  lcd.setCursor(3, 1);
  lcd.print("H");
  lcd.setCursor(5, 1);
  lcd.print(Sh2);
  lcd.setCursor(8, 1);
  lcd.print(":");
  lcd.setCursor(10, 1);
  lcd.print("M");
  lcd.setCursor(12, 1);
  lcd.print(Sm2);
  if(Sm2 < 10)
  {
   lcd.setCursor(13, 1);
   lcd.print(" ");  
  }  
  lcd.setCursor(1, 3);
  lcd.print("[Select to Feed]");      
 }

  if(t == 3)
 {
  Flag=42;
  lcd.setCursor(1, 0);
  lcd.print("RECIRCULATION PUMB");
  lcd.setCursor(1, 1);
  lcd.print("ON.T :");    
  lcd.setCursor(7, 1);
  lcd.print("H");
  lcd.setCursor(9, 1);
  lcd.print(Sh3);  
  lcd.setCursor(11, 1);
  lcd.print(":"); 
  lcd.setCursor(12, 1);
  lcd.print("M"); 
  if(Sm3 < 10)
  {
  lcd.setCursor(15, 1);
  lcd.print(" ");    
  }
  lcd.setCursor(14, 1);
  lcd.print(Sm3);  
  
  
  lcd.setCursor(1, 2);
  lcd.print("OFF.T:"); 
  lcd.setCursor(7, 2);
  lcd.print("H");
  lcd.setCursor(9, 2);
  lcd.print(Sh32);
  lcd.setCursor(11, 2);
  lcd.print(":"); 
  lcd.setCursor(12, 2);
  lcd.print("M");
  lcd.setCursor(14, 2);
  lcd.print(Sm32);    
        
  lcd.setCursor(1, 3);
  lcd.print("[Select to Feed]");        
 }

  if(t == 4)
 {
  Flag=43;
  lcd.setCursor(1, 0);
  lcd.print(" FILTER FEED PUMB");  
  lcd.setCursor(3, 1);
  lcd.print("H");
  lcd.setCursor(5, 1);
  lcd.print(Sh4);
  lcd.setCursor(8, 1);
  lcd.print(":");
  lcd.setCursor(10, 1);
  lcd.print("M");
  lcd.setCursor(12, 1);
  lcd.print(Sm4);
  if(Sm4 < 10)
  {
   lcd.setCursor(13, 1);
   lcd.print(" ");  
  }  
  lcd.setCursor(1, 3);
  lcd.print("[Select to Feed]");      
 }

  if(t == 5)
 {
  Flag=44;
  lcd.setCursor(1, 0); 
  lcd.print(" TREATED WATER PUMB"); 
  lcd.setCursor(3, 1);
  lcd.print("H");
  lcd.setCursor(5, 1);
  lcd.print(Sh5);
  lcd.setCursor(8, 1);
  lcd.print(":");
  lcd.setCursor(10, 1);
  lcd.print("M");
  lcd.setCursor(12, 1);
  lcd.print(Sm5);
  if(Sm5 < 10)
  {
   lcd.setCursor(13, 1);
   lcd.print(" ");  
  }  
  lcd.setCursor(1, 3);
  lcd.print("[Select to Feed]");      
 }
}
