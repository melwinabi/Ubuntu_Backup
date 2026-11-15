void Set_view6()
{
  Flag = 45;
  
  lcd.setCursor(1, 1);
  lcd.print("Date: ");
  if(dy < 10)
  {
    lcd.print('0');
  }  
  lcd.print(dy);
  lcd.print('/');
  if(mon < 10)
  {
    lcd.print('0');
  }    
  lcd.print(mon);
  lcd.print('/');
  lcd.print(yr);

  lcd.setCursor(17, 1);
  lcd.print("  ");  
  
  lcd.setCursor(1, 2);
  lcd.print("Time: ");
  
  if(hr < 10)
  {
    lcd.print('0');
  }
  lcd.print(hr);
  
  lcd.print(':');
  
  if(mn < 10)
  {
    lcd.print('0');
  }
  lcd.print(mn);
  
  lcd.print(':');
  
  if(sec < 10)
  {
    lcd.print('0');
  }
  lcd.print(sec);
}


void DATE()
{
 Flag=46;
 lcd.setCursor(6,0);
 lcd.print("SET DATE");
 lcd.setCursor(2,1);
 lcd.print("Y");
 lcd.setCursor(4,1);  
 lcd.print(YR);
 lcd.print(":");
 lcd.print("M");
 lcd.setCursor(11,1);
 lcd.print(MON);
   if(MON < 10)
 {
  lcd.setCursor(12,1);  
  lcd.print(" ");
 }
 lcd.print(":");
 lcd.print("D");
 lcd.setCursor(16,1);  
 lcd.print(DY);
  if(DY < 10)
 {
  lcd.setCursor(17,1);  
  lcd.print(" ");
 }
 lcd.setCursor(8,2);
 lcd.print("SET");
 lcd.setCursor(2,3);
 lcd.print("[SELECT TO FEED]");
}

void TIME()
{
 Flag=47; 
 lcd.setCursor(6,0);
 lcd.print("SET TIME");
 lcd.setCursor(2,1);
 lcd.print("H");
 lcd.setCursor(4,1);  
 lcd.print(HR);
 if(HR < 10)
 {
 lcd.setCursor(5,1);  
 lcd.print(" ");  
 }
 lcd.print(":");
 lcd.print("M");
 lcd.setCursor(9,1);
 lcd.print(MN);
 if(MN < 10)
 {
 lcd.setCursor(10,1);  
 lcd.print(" ");  
 } 
 lcd.print(":");
 lcd.print("S");
 lcd.setCursor(14,1);  
 lcd.print(SEC);
 if(SEC < 10)
 {
 lcd.setCursor(15,1);  
 lcd.print("   ");  
 }  
 lcd.setCursor(8,2);
 lcd.print("SET"); 
 lcd.setCursor(2,3);
 lcd.print("[SELECT TO FEED]");  
}

void set_scr_D()
{
 DS3231M.adjust(DateTime(YR, MON, DY, hr, mn, sec));
 delay(150);
 lcd.clear();
 C=1;
 curs(C);
 Set_view6();
}

void set_scr_T()
{
 DS3231M.adjust(DateTime(yr, mon, dy, HR, MN, SEC));
 delay(150);
 lcd.clear();
 C=1;
 curs(C);
 Set_view6();
}
