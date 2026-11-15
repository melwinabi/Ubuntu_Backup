void Flag_7(){
 if(Flag == 7)
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
    set_C2(); 
   }
   curs(C);
  }
  if(S == LOW)
  {
   if(C == 0)
   {
    S=HIGH;
    lcd.clear();
    C=1;
    curs(C);    
    Set_view3(1); 
   }
   else if(C == 1)
   {
    S=HIGH;
    lcd.clear();
    C=1;
    curs(C);
    Set_view3(2); 
   }
    else if(C == 2)
   {
    S=HIGH;
    lcd.clear();
    C=1;
    curs(C);    
    Set_view3(3); 
   }
    else if(C == 3)
   {
    S=HIGH;
    lcd.clear();
    C=1;
    curs(C);
    Set_view3(4); 
   }
  }
 }
}

void Flag_8()
{
 if(Flag == 8)
 {
  if(U == LOW)
  {
    C=3;
    set_C1(); 
  }
  if(S == LOW)
  {
   if(C == 0)
   {
    S=HIGH;
    lcd.clear();
    C=1;
    curs(C);    
    Set_view3(5);
   } 
  }
 }
}

void Flag_27()
{
  if (EmodeF1 == 0 )
  {
    modeF1 = "OFF";
  }
  else
  {
    modeF1 = "ON";
  }

  if (EmodeT1 == 0)
  {
    modeT1 = "OFF";
  }
  else
  {
    modeT1 = "ON";
  }

  if (Flag == 27)
  {
    if (U == LOW)
    {
      C--;
      if (C < 1)
      {
        C = 2;
      }
      curs(C);
    }

    if (D == LOW)
    {
      C++;
      if (C > 2)
      {
        C = 1;
      }
      curs(C);
    }

    if (S == LOW)
    {
      bool updateEEPROM = false;

      if (C == 1)
      {
        if (modeF1 == "ON")
        {
          modeF1 = "OFF";
          EmodeF1 = 0;
          updateEEPROM = true;
        
        }
        else
        {
          modeF1 = "ON";
          EmodeF1 = 1;
          updateEEPROM = true;
        
        }
      }

      if (C == 2)
      {
        if (modeT1 == "ON")
        {
          modeT1 = "OFF";
          EmodeT1 = 0;
          updateEEPROM = true;
         
        }
        else
        {
          modeT1 = "ON";
          EmodeT1 = 1;
          updateEEPROM = true;
          
        }
      }

      Set_view3(1);

      if (updateEEPROM)
      {
        EEPROM.write(35, EmodeF1);
        EEPROM.write(36, EmodeT1);
      }
    }
  }
}

void Flag_28()
{
  if (EmodeF2 == 0)
  {
    modeF2 = "OFF";
  }
  else
  {
    modeF2 = "ON";
  }

  if (EmodeT2 == 0)
  {
    modeT2 = "OFF";
  }
  else
  {
    modeT2 = "ON";
  }

  if (Flag == 28)
  {
    if (U == LOW)
    {
      C--;
      if (C < 1)
      {
        C = 2;
      }
      curs(C);
    }

    if (D == LOW)
    {
      C++;
      if (C > 2)
      {
        C = 1;
      }
      curs(C);
    }

    if (S == LOW)
    {
      bool updateEEPROM = false;

      if (C == 1)
      {
        if (modeF2 == "ON" )
        {
          modeF2 = "OFF";
          EmodeF2 = 0;
          updateEEPROM = true;
        }
        else
        {
          modeF2 = "ON";
          EmodeF2 = 1;
          updateEEPROM = true;
        }
      }

      if (C == 2)
      {
        if (modeT2 == "ON")
        {
          modeT2 = "OFF";
          EmodeT2 = 0;
          updateEEPROM = true;
        }
        else
        {
          modeT2 = "ON";
          EmodeT2 = 1;
          updateEEPROM = true;
        }
      }

      Set_view3(2);

      if (updateEEPROM)
      {
        EEPROM.write(37, EmodeF2);
        EEPROM.write(38, EmodeT2);
      }
    }
  }
}

void Flag_29()
{
  if (EmodeF3 == 0)
  {
    modeF3 = "OFF";
  }
  else
  {
    modeF3 = "ON";
  }

  if (EmodeT3 == 0)
  {
    modeT3 = "OFF";
  }
  else
  {
    modeT3 = "ON";
  }

  if (Flag == 29)
  {
    if (U == LOW)
    {
      C--;
      if (C < 1)
      {
        C = 2;
      }
      curs(C);
    }

    if (D == LOW)
    {
      C++;
      if (C > 2)
      {
        C = 1;
      }
      curs(C);
    }

    if (S == LOW)
    {
      bool updateEEPROM = false;

      if (C == 1)
      {
        if (modeF3 == "ON")
        {
          modeF3 = "OFF";
          EmodeF3 = 0;
          updateEEPROM = true;
        }
        else
        {
          modeF3 = "ON";
          EmodeF3 = 1;
          updateEEPROM = true;
        }
      }

      if (C == 2)
      {
        if (modeT3 == "ON")
        {
          modeT3 = "OFF";
          EmodeT3 = 0;
          updateEEPROM = true;
        }
        else
        {
          modeT3 = "ON";
          EmodeT3 = 1;
          updateEEPROM = true;
        }
      }

      Set_view3(3);

      if (updateEEPROM)
      {
        EEPROM.write(39, EmodeF3);
        EEPROM.write(40, EmodeT3);
      }
    }
  }
}

void Flag_30()
{
  if (EmodeF4 == 0)
  {
    modeF4 = "OFF";
  }
  else
  {
    modeF4 = "ON";
  }

  if (EmodeT4 == 0)
  {
    modeT4 = "OFF";
  }
  else
  {
    modeT4 = "ON";
  }

  if (Flag == 30)
  {
    if (U == LOW)
    {
      C--;
      if (C < 1)
      {
        C = 2;
      }
      curs(C);
    }

    if (D == LOW)
    {
      C++;
      if (C > 2)
      {
        C = 1;
      }
      curs(C);
    }

    if (S == LOW)
    {
      bool updateEEPROM = false;

      if (C == 1)
      {
        if (modeF4 == "ON")
        {
          modeF4 = "OFF";
          EmodeF4 = 0;
          updateEEPROM = true;
        }
        else
        {
          modeF4 = "ON";
          EmodeF4 = 1;
          updateEEPROM = true;
        }
      }

      if (C == 2)
      {
        if (modeT4 == "ON")
        {
          modeT4 = "OFF";
          EmodeT4 = 0;
          updateEEPROM = true;
        }
        else
        {
          modeT4 = "ON";
          EmodeT4 = 1;
          updateEEPROM = true;
        }
      }

      Set_view3(4);

      if (updateEEPROM)
      {
        EEPROM.write(41, EmodeF4);
        EEPROM.write(42, EmodeT4);
      }
    }
  }
}

void Flag_31()
{
  if (EmodeF5 == 0)
  {
    modeF5 = "OFF";
  }
  else
  {
    modeF5 = "ON";
  }

  if (EmodeT5 == 0)
  {
    modeT5 = "OFF";
  }
  else
  {
    modeT5 = "ON";
  }

  if (Flag == 31)
  {
    if (U == LOW)
    {
      C--;
      if (C < 1)
      {
        C = 2;
      }
      curs(C);
    }

    if (D == LOW)
    {
      C++;
      if (C > 2)
      {
        C = 1;
      }
      curs(C);
    }

    if (S == LOW)
    {
      bool updateEEPROM = false;

      if (C == 1)
      {
        if (modeF5 == "ON")
        {
          modeF5 = "OFF";
          EmodeF5 = 0;
          updateEEPROM = true;
        }
        else
        {
          modeF5 = "ON";
          EmodeF5 = 1;
          updateEEPROM = true;
        }
      }

      if (C == 2)
      {
        if (modeT5 == "ON")
        {
          modeT5 = "OFF";
          EmodeT5 = 0;
          updateEEPROM = true;
        }
        else
        {
          modeT5 = "ON";
          EmodeT5 = 1;
          updateEEPROM = true;
        }
      }

      Set_view3(5);

      if (updateEEPROM)
      {
        EEPROM.write(43, EmodeF5);
        EEPROM.write(44, EmodeT5);
      }
    }
  }
}
