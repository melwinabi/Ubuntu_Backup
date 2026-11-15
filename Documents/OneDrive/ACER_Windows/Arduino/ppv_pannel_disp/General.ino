void Gen()
{  
 DateTime now = DS3231M.now(); 
 yr=now.year();
 mon=now.month();
 dy=now.day();
 hr=now.hour();
 mn=now.minute();
 sec=now.second();
 if(Flag == 45)
 {
  Set_view6();
  if(U == LOW)
  {
   C--;
   if(C < 1)
   {
    C=2;  
   }
   curs(C);
  }
  if(D == LOW)
  {
   C++;
   if(C > 2)
   {
    C=1;
   }
   curs(C);
  }
  if(S == LOW)
  {
   if(C == 1)
   {
    lcd.clear();
    lcd.setCursor(3,1);
    lcd.print(">");
    YR = yr;
    MON = mon;
    DY = dy;
    C=1;
    DATE();
   }
   else if(C == 2)
   {
    lcd.clear(); 
    HR = hr;
    MN = mn;
    SEC = sec;  
    C=1;
    TIME();
   }
  }
 } 
}

void Flag_46()
{
 if(Flag == 46)
 {
  if(S == LOW)
  {
   dl++; 
   if(dl > 4)
   {
    dl=1;
   }
   if(dl == 1)
   {
    lcd.setCursor(3,1);
    lcd.print(">");
    lcd.setCursor(15,1);
    lcd.print(" "); 
    lcd.setCursor(10,1);
    lcd.print(" ");
    lcd.setCursor(7,2);
    lcd.print(" ");        
   }
   else if(dl == 2)
   {
    lcd.setCursor(10,1);
    lcd.print(">"); 
    lcd.setCursor(3,1);
    lcd.print(" ");
    lcd.setCursor(15,1);
    lcd.print(" ");
    lcd.setCursor(7,2);
    lcd.print(" ");        
   }
   else if(dl == 3)
   {
    lcd.setCursor(15,1);
    lcd.print(">");
    lcd.setCursor(10,1);
    lcd.print(" "); 
    lcd.setCursor(3,1);
    lcd.print(" "); 
    lcd.setCursor(7,2);
    lcd.print(" ");
            
   }
   else if(dl == 4)
   {
    lcd.setCursor(15,1);
    lcd.print(" ");
    lcd.setCursor(10,1);
    lcd.print(" "); 
    lcd.setCursor(3,1);
    lcd.print(" ");
    lcd.setCursor(7,2);
    lcd.print(">");
   }
  }  
  
  if(U == LOW)
  {
   if(dl == 1)
   {
    YR--;
    if(YR < 2024)
    {
     YR=2099;
    }
   }
   else if(dl == 2)
   {
    MON--;
    if(MON < 1)
    {
     MON=12; 
    }
   }
   else if(dl == 3)
   {
    DY--;
    if(DY < 1)
    {
     DY=31; 
    }
   }
   DATE();
  }
  if(dl == 4)
  {
   if(U == LOW || D == LOW)
   {
    set_scr_D();
    }
  }
  
  if(D == LOW)
  {
   if(dl == 1)
   {
    YR++;
    if(YR > 2099)
    {
     YR=2024; 
    }
   }
   else if(dl == 2)
   {
    MON++;
    if(MON > 12)
    {
     MON=1; 
    }
   }
   else if(dl == 3)
   {
    DY++;
    if(DY > 31)
    {
     DY=1; 
    }
   }
   DATE();
  }
 }
}


void Flag_47()
{
 if(Flag == 47)
 {
  if(S == LOW)
  {
   dl++; 
   if(dl > 4)
   {
    dl=1;
   }
   if(dl == 1)
   {
    lcd.setCursor(3,1);
    lcd.print(">");
    lcd.setCursor(8,1);
    lcd.print(" "); 
    lcd.setCursor(13,1);
    lcd.print(" ");
    lcd.setCursor(7,2);
    lcd.print(" ");        
   }
   else if(dl == 2)
   {
    lcd.setCursor(8,1);
    lcd.print(">"); 
    lcd.setCursor(3,1);
    lcd.print(" ");
    lcd.setCursor(13,1);
    lcd.print(" ");
    lcd.setCursor(7,2);
    lcd.print(" ");        
   }
   else if(dl == 3)
   {
    lcd.setCursor(13,1);
    lcd.print(">");
    lcd.setCursor(8,1);
    lcd.print(" "); 
    lcd.setCursor(3,1);
    lcd.print(" "); 
    lcd.setCursor(7,2);
    lcd.print(" ");
            
   }
   else if(dl == 4)
   {
    lcd.setCursor(3,1);
    lcd.print(" ");
    lcd.setCursor(8,1);
    lcd.print(" "); 
    lcd.setCursor(13,1);
    lcd.print(" ");
    lcd.setCursor(7,2);
    lcd.print(">");
   }
  }  
  
  if(U == LOW)
  {
   if(dl == 1)
   {
    HR--;
    if(HR < 1)
    {
     HR=24;
    }
   }
   else if(dl == 2)
   {
    MN--;
    if(MN < 0)
    {
     MN=60; 
    }
   }
   else if(dl == 3)
   {
    SEC--;
    if(SEC < 0)
    {
     SEC=60; 
    }
   }
   TIME();
  }
  if(dl == 4)
  {
   if(U == LOW || D == LOW)
   {
    set_scr_T();
    }
  }
  
  if(D == LOW)
  {
   if(dl == 1)
   {
    HR++;
    if(HR > 24)
    {
     HR=1; 
    }
   }
   else if(dl == 2)
   {
    MN++;
    if(MN > 60)
    {
     MN=0; 
    }
   }
   else if(dl == 3)
   {
    SEC++;
    if(SEC > 60)
    {
     SEC=0; 
    }
   }
   TIME();
  }
    if(dl == 4)
  {
   if(U == LOW || D == LOW)
   {
    set_scr_T();
    }
  }
 }
}  
