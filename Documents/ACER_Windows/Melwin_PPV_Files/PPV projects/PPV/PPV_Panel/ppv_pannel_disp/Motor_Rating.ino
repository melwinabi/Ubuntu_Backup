void Flag_5(){
  if(Flag == 5)
  {
    if(U == LOW) 
  {
   C--;
   if(C < 0)
   {
    C=0;  
   }
   curs(C);
  }
  if(D == LOW)
  {
   C++;
   if(C > 3)
   {
    C=0;
    D = HIGH;
    set_B2();
   }
   curs(C);
  }
  if(S == LOW)
  {
   if(C == 0)
   {
    S=HIGH;
    lcd.clear();
    Set_view2(1);
   }

   else if(C == 1)
   {
    S=HIGH;
    lcd.clear();
    Set_view2(2);
   }

   else if(C == 2)
   {
    S=HIGH;
    lcd.clear();
    Set_view2(3);
   }

   else if(C == 3)
   {
    S=HIGH;
    lcd.clear();
    Set_view2(4);
   }

   if(C == 4)
   {
    S=HIGH;
    lcd.clear();
    Set_view2(5);
   }
  }
 }
}

void Flag_6()
{
 if(Flag == 6)
 { 
  if(S == LOW)
  {
   if(C == 0)
   {
    S=HIGH;
    lcd.clear();
    Set_view2(5);
   }  
  }
 }
}


void Flag_22()
{
 unit1=EEPROM.read(10);
 unit2=EEPROM.read(11);
 if(Flag == 22)
 {
  if(S == LOW)
  {
   Sl++;
   if(Sl == 1)
   {
    R=4;
   }
   else if(Sl == 2)
   {
    R=7;
   }
   else if(Sl > 2)
   {
    R=4;
    Sl=1;
   }
   Curs_2(R);
  }
  if(U == LOW)
  {
   if(R == 4)
   {  
    unit1--;
    if(unit1 < 0)
    {
     unit1=15;
    }
   } 
   else if(R == 7) 
   {
     unit2--;
     if(unit2 < 0)
     {
      unit2=9;     
    }
   }  
  }
  if(D == LOW)
  {
   if(R == 4)
   { 
    unit1++;
    if(unit1 > 15)
    {
     unit1=0;
    } 
   }
   else if(R == 7)
   {
    unit2++;
    if(unit2 > 9)
    {
     unit2=0;
    }  
   }  
  }
   Set_view2(1);
   EEPROM.write(10,unit1);
   EEPROM.write(11,unit2);
 }
 Rating1= unit1 + (unit2 /10.0);
// Serial.println(Rating1);
}


void Flag_23()
{
 unit3=EEPROM.read(12);
 unit4=EEPROM.read(13);
 if(Flag == 23)
 {
  if(S == LOW)
  {
   Sl++;
   if(Sl == 1)
   {
    R=4;
   }
   else if(Sl == 2)
   {
    R=7;
   }
   else if(Sl > 2)
   {
    R=4;
    Sl=1;
   }
   Curs_2(R);
  }
  if(U == LOW)
  {
   if(R == 4)
   {  
    unit3--;
    if(unit3 < 0)
    {
     unit3=15;
    }
   } 
   else if(R == 7) 
   {
     unit4--;
     if(unit4 < 0)
     {
      unit4=9;     
    }
   }  
  }
  if(D == LOW)
  {
   if(R == 4)
   { 
    unit3++;
    if(unit3 > 15)
    {
     unit3=0;
    } 
   }
   else if(R == 7)
   {
    unit4++;
    if(unit4 > 9)
    {
     unit4=0;
    }  
   }   
  }
   Set_view2(2);
   EEPROM.write(12,unit3);
   EEPROM.write(13,unit4);
 }
  Rating2= unit3 + (unit4 /10.0);
 // Serial.println(Rating2); 
}

void Flag_24() {
  unit5 = EEPROM.read(14);
  unit6 = EEPROM.read(15);
  if (Flag == 24) {
    if (S == LOW) {
      Sl++;
      if (Sl == 1) {
        R = 4;
      } else if (Sl == 2) {
        R = 7;
      } else if (Sl > 2) {
        R = 4;
        Sl = 1;
      }
      Curs_2(R);
    }
    if (U == LOW) {
      if (R == 4) {
        unit5--;
        if (unit5 < 0) {
          unit5 = 15;
        }
      } else if (R == 7) {
        unit6--;
        if (unit6 < 0) {
          unit6 = 9;
        }
      }
    }
    if (D == LOW) {
      if (R == 4) {
        unit5++;
        if (unit5 > 15) {
          unit5 = 0;
        }
      } else if (R == 7) {
        unit6++;
        if (unit6 > 9) {
          unit6 = 0;
        }
      }
    }
    Set_view2(3);
    EEPROM.write(14, unit5);
    EEPROM.write(15, unit6);
  }
   Rating3= unit5 + (unit6 /10.0);
  // Serial.println(Rating3);
}

void Flag_25() {
  unit7 = EEPROM.read(16);
  unit8 = EEPROM.read(17);
  if (Flag == 25) {
    if (S == LOW) {
      Sl++;
      if (Sl == 1) {
        R = 4;
      } else if (Sl == 2) {
        R = 7;
      } else if (Sl > 2) {
        R = 4;
        Sl = 1;
      }
      Curs_2(R);
    }
    if (U == LOW) {
      if (R == 4) {
        unit7--;
        if (unit7 < 0) {
          unit7 = 15;
        }
      } else if (R == 7) {
        unit8--;
        if (unit8 < 0) {
          unit8 = 9;
        }
      }
    }
    if (D == LOW) {
      if (R == 4) {
        unit7++;
        if (unit7 > 15) {
          unit7 = 0;
        }
      } else if (R == 7) {
        unit8++;
        if (unit8 > 9) {
          unit8 = 0;
        }
      }
    }
    Set_view2(4);
    EEPROM.write(16, unit7);
    EEPROM.write(17, unit8);
  }
  Rating4= unit7 + (unit8 /10.0);
 // Serial.println(Rating4);  
}

void Flag_26() {
  unit9 = EEPROM.read(18);
  unit10 = EEPROM.read(19);
  if (Flag == 26) {
    if (S == LOW) {
      Sl++;
      if (Sl == 1) {
        R = 4;
      } else if (Sl == 2) {
        R = 7;
      } else if (Sl > 2) {
        R = 4;
        Sl = 1;
      }
      Curs_2(R);
    }
    if (U == LOW) {
      if (R == 4) {
        unit9--;
        if (unit9 < 0) {
          unit9 = 15;
        }
      } else if (R == 7) {
        unit10--;
        if (unit10 < 0) {
          unit10 = 9;
        }
      }
    }
    if (D == LOW) {
      if (R == 4) {
        unit9++;
        if (unit9 > 15) {
          unit9 = 0;
        }
      } else if (R == 7) {
        unit10++;
        if (unit10 > 9) {
          unit10 = 0;
        }
      }
    }
    Set_view2(5);
    EEPROM.write(18, unit9);
    EEPROM.write(19, unit10);
  }
   Rating5= unit9 + (unit10 /10.0);
   //Serial.println(Rating5);
}
