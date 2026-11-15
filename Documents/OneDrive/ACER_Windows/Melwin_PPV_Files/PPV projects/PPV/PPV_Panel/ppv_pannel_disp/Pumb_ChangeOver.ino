void Flag_38()
{
 if(Flag == 38)
 {
  if(U == LOW)
  {
   C--;
   if(C < 0)
   {
    C=3; 
   }
   curs(C);
  }
  if(D == LOW)
  {
   C++;
   if(C > 3)
   {
    C=0;
   }
  }
  curs(C);
  if(S == LOW){
  if(C == 0)
  {
   lcd.clear(); 
   C=1; 
   curs(C);
   LCD_msg=" [Select to Feed]  "; 
   Set_view4(1); 
   S=HIGH;
  }
  else if(C == 1)
  {
   lcd.clear(); 
   C=1; 
   curs(C);
   LCD_msg=" [Select to Feed]  ";
   Set_view4(2); 
   S=HIGH; 
  }
  else if(C == 2)
  {
   lcd.clear(); 
   C=1; 
   curs(C);
   LCD_msg=" [Select to Feed]  ";
   Set_view4(3); 
   S=HIGH; 
  }
  else if(C == 3)
  {
   lcd.clear(); 
   C=1; 
   curs(C);
   LCD_msg=" [Select to Feed]  ";
   Set_view4(4); 
   S=HIGH;
  }
 }
}
}


void Flag_32()
{
 if (EPCOT1 == 1) {
    PCOT1 = "ON";
} else {
    PCOT1 = "OFF";
}

if (ERWPb1 == 1) {
    RWPb1 = "ON";
} else {
    RWPb1 = "OFF";
}

if (ERWPb2 == 1) {
    RWPb2 = "ON";
} else {
    RWPb2 = "OFF";
}

 if(Flag == 32)
 {
  if(U == LOW)
  {
   ns--;
   if(ns < 1)
   {
    ns=3; 
   }
  }
  if(D == LOW)
  {
   ns++;
   if(ns > 3)
   {
    ns=1; 
   }
  }
  if(ns == 1)
  {
     C=1;
     lcd.setCursor(0,1);
     lcd.print(">");
     lcd.setCursor(10,2);
     lcd.print(" ");
     lcd.setCursor(0,2);
     lcd.print(" ");    
  }
  else if(ns == 2)
  {
     C=2;
     lcd.setCursor(0,1);
     lcd.print(" ");
     lcd.setCursor(10,2);
     lcd.print(" ");
     lcd.setCursor(0,2);
     lcd.print(">");    
  }
  else if(ns == 3)
  {   
     C=3;
     lcd.setCursor(0,1);
     lcd.print(" ");
     lcd.setCursor(10,2);
     lcd.print(">");
     lcd.setCursor(0,2);
     lcd.print(" "); 
  }
  if(S == LOW)
  {
   if(C == 1)
   {
    if(PCOT1 == "ON")
    {
     PCOT1="OFF";
     EPCOT1=0;
     LCD_msg=" [Select to Feed]  "; 
    }
    else
    {
     PCOT1="ON";
     EPCOT1=1;
     RWPb1="OFF";
     ERWPb1=0;
     RWPb2="OFF";
     ERWPb2=0; 
     LCD_msg="Toggle Mode Engaged";     
    }
   }
   else if(C == 2 && RWPb2 == "OFF")
   {
    if(EPCOT1 == 0)
    {
     if(RWPb1 == "ON" )
     {
      RWPb1="OFF";
      ERWPb1=0;
     }
     else
     {
      RWPb1="ON";
      ERWPb1=1;
      LCD_msg=" Pump One Active  ";
     }    
    }
    else if(EPCOT1 == 1)
    {
     LCD_msg="Turn Off Toggle      ";
    }
    
   }
   else if(C == 3 && RWPb1 == "OFF")
   {
    if(EPCOT1 == 0)
    {
     if(RWPb2 == "ON")
     {  
      RWPb2="OFF";
      ERWPb2=0;
     }
     else
     {
      RWPb2="ON";
      ERWPb2=1;
      LCD_msg="Pump Two Active   "; 
     }  
    }  
    else if(EPCOT1 == 1)
    {
     LCD_msg="Turn Off Toggle      ";
    }
   }
   EEPROM.write(20,EPCOT1);
   EEPROM.write(21,ERWPb1);
   EEPROM.write(22,ERWPb2);
   Set_view4(1);
  }
 }
}



void Flag_33()
{
  if (EPCOT2 == 1) {
    PCOT2 = "ON";
  } else {
    PCOT2 = "OFF";
  }

  if (ERWPb3 == 1) {
    RWPb3 = "ON";
  } else {
    RWPb3 = "OFF";
  }

  if (ERWPb4 == 1) {
    RWPb4 = "ON";
  } else {
    RWPb4 = "OFF";
  }

  if (Flag == 33)
  {
    if (U == LOW)
    {
      Serial.println("up");
      ns--;
      if (ns < 1)
      {
        ns = 3;
      }
    }

    if (D == LOW)
    {
      Serial.println("dn");
      ns++;
      if (ns > 3)
      {
        ns = 1;
      }
    }

    if (ns == 1)
    {
      C = 1;
      lcd.setCursor(0, 1);
      lcd.print(">");
      lcd.setCursor(10, 2);
      lcd.print(" ");
      lcd.setCursor(0, 2);
      lcd.print(" ");
    }
    else if (ns == 2)
    {
      C = 2;
      lcd.setCursor(0, 1);
      lcd.print(" ");
      lcd.setCursor(10, 2);
      lcd.print(" ");
      lcd.setCursor(0, 2);
      lcd.print(">");
    }
    else if (ns == 3)
    {
      C = 3;
      lcd.setCursor(0, 1);
      lcd.print(" ");
      lcd.setCursor(10, 2);
      lcd.print(">");
      lcd.setCursor(0, 2);
      lcd.print(" ");
    }

  if(S == LOW)
  {
   if(C == 1)
   {
    if(PCOT2 == "ON")
    {
     PCOT2="OFF";
     EPCOT2=0;
     LCD_msg=" [Select to Feed]  "; 
    }
    else
    {
     PCOT2="ON";
     EPCOT2=1;
     RWPb3="OFF";
     ERWPb3=0;
     RWPb4="OFF";
     ERWPb4=0; 
     LCD_msg="Toggle Mode Engaged";     
    }
   }
   else if(C == 2 && RWPb4 == "OFF")
   {
    if(EPCOT2 == 0)
    {
     if(RWPb3 == "ON" )
     {
      RWPb3="OFF";
      ERWPb3=0;
     }
     else
     {
      RWPb3="ON";
      ERWPb3=1;
      LCD_msg=" Pump One Active  ";
     }    
    }
    else if(EPCOT2 == 1)
    {
     LCD_msg="Turn Off Toggle     ";
    }
    
   }
   else if(C == 3 && RWPb3 == "OFF")
   {
    if(EPCOT2 == 0)
    {
     if(RWPb4 == "ON")
     {  
      RWPb2="OFF";
      ERWPb2=0;
     }
     else
     {
      RWPb4="ON";
      ERWPb4=1;
      LCD_msg="Pump Two Active   "; 
     }  
    }  
    else if(EPCOT2 == 1)
    {
     LCD_msg="Turn Off Toggle     ";
    }
   }
   EEPROM.write(23,EPCOT2);
   EEPROM.write(24,ERWPb3);
   EEPROM.write(25,ERWPb4);
   Set_view4(2);
  }
 }
}

void Flag_35()
{
  if (EPCOT4 == 1) {
    PCOT4 = "ON";
  } else {
    PCOT4 = "OFF";
  }

  if (ERWPb7 == 1) {
    RWPb7 = "ON";
  } else {
    RWPb7 = "OFF";
  }

  if (ERWPb8 == 1) {
    RWPb8 = "ON";
  } else {
    RWPb8 = "OFF";
  }

  if (Flag == 35)
  {
    if (U == LOW)
    {
      ns--;
      if (ns < 1)
      {
        ns = 3;
      }
    }

    if (D == LOW)
    {
      ns++;
      if (ns > 3)
      {
        ns = 1;
      }
    }

    if (ns == 1)
    {
      C = 1;
      lcd.setCursor(0, 1);
      lcd.print(">");
      lcd.setCursor(10, 2);
      lcd.print(" ");
      lcd.setCursor(0, 2);
      lcd.print(" ");
    }
    else if (ns == 2)
    {
      C = 2;
      lcd.setCursor(0, 1);
      lcd.print(" ");
      lcd.setCursor(10, 2);
      lcd.print(" ");
      lcd.setCursor(0, 2);
      lcd.print(">");
    }
    else if (ns == 3)
    {
      C = 3;
      lcd.setCursor(0, 1);
      lcd.print(" ");
      lcd.setCursor(10, 2);
      lcd.print(">");
      lcd.setCursor(0, 2);
      lcd.print(" ");
    }

  if(S == LOW)
  {
   if(C == 1)
   {
    if(PCOT4 == "ON")
    {
     PCOT4="OFF";
     EPCOT4=0;
     LCD_msg=" [Select to Feed]  "; 
    }
    else
    {
     PCOT4="ON";
     EPCOT4=1;
     RWPb7="OFF";
     ERWPb7=0;
     RWPb8="OFF";
     ERWPb8=0; 
     LCD_msg="Toggle Mode Engaged";     
    }
   }
   else if(C == 2 && RWPb8 == "OFF")
   {
    if(EPCOT4 == 0)
    {
     if(RWPb7 == "ON" )
     {
      RWPb7="OFF";
      ERWPb7=0;
     }
     else
     {
      RWPb7="ON";
      ERWPb7=1;
      LCD_msg=" Pump One Active  ";
     }    
    }
    else if(EPCOT4 == 1)
    {
     LCD_msg="Turn Off Toggle      ";
    }
    
   }
   else if(C == 3 && RWPb7 == "OFF")
   {
    if(EPCOT4 == 0)
    {
     if(RWPb8 == "ON")
     {  
      RWPb8="OFF";
      ERWPb8=0;
     }
     else
     {
      RWPb8="ON";
      ERWPb8=1;
      LCD_msg="Pump Two Active   "; 
     }  
    }  
    else if(EPCOT4 == 1)
    {
     LCD_msg="Turn Off Toggle     ";
    }
   }
   EEPROM.write(29,EPCOT4);
   EEPROM.write(30,ERWPb7);
   EEPROM.write(31,ERWPb8);
   Set_view4(3);
  }
 }
}


void Flag_36()
{
  if (EPCOT5 == 1) {
    PCOT5 = "ON";
  } else {
    PCOT5 = "OFF";
  }

  if (ERWPb9 == 1) {
    RWPb9 = "ON";
  } else {
    RWPb9 = "OFF";
  }

  if (ERWPb10 == 1) {
    RWPb10 = "ON";
  } else {
    RWPb10 = "OFF";
  }

  if (Flag == 36)
  {
    if (U == LOW)
    {
      ns--;
      if (ns < 1)
      {
        ns = 3;
      }
    }

    if (D == LOW)
    {
      ns++;
      if (ns > 3)
      {
        ns = 1;
      }
    }

    if (ns == 1)
    {
      C = 1;
      lcd.setCursor(0, 1);
      lcd.print(">");
      lcd.setCursor(10, 2);
      lcd.print(" ");
      lcd.setCursor(0, 2);
      lcd.print(" ");
    }
    else if (ns == 2)
    {
      C = 2;
      lcd.setCursor(0, 1);
      lcd.print(" ");
      lcd.setCursor(10, 2);
      lcd.print(" ");
      lcd.setCursor(0, 2);
      lcd.print(">");
    }
    else if (ns == 3)
    {
      C = 3;
      lcd.setCursor(0, 1);
      lcd.print(" ");
      lcd.setCursor(10, 2);
      lcd.print(">");
      lcd.setCursor(0, 2);
      lcd.print(" ");
    }

  if(S == LOW)
  {
   if(C == 1)
   {
    if(PCOT5 == "ON")
    {
     PCOT5="OFF";
     EPCOT5=0;
     LCD_msg=" [Select to Feed]  "; 
    }
    else
    {
     PCOT5="ON";
     EPCOT5=1;
     RWPb9="OFF";
     ERWPb9=0;
     RWPb10="OFF";
     ERWPb10=0; 
     LCD_msg="Toggle Mode Engaged";     
    }
   }
   else if(C == 2 && RWPb10 == "OFF")
   {
    if(EPCOT5 == 0)
    {
     if(RWPb9 == "ON" )
     {
      RWPb9="OFF";
      ERWPb9=0;
     }
     else
     {
      RWPb9="ON";
      ERWPb9=1;
      LCD_msg=" Pump One Active  ";
     }    
    }
    else if(EPCOT5 == 1)
    {
     LCD_msg="Turn Off Toggle      ";
    }
    
   }
   else if(C == 3 && RWPb9 == "OFF")
   {
    if(EPCOT5 == 0)
    {
     if(RWPb10 == "ON")
     {  
      RWPb10="OFF";
      ERWPb10=0;
     }
     else
     {
      RWPb10="ON";
      ERWPb10=1;
      LCD_msg="Pump Two Active   "; 
     }  
    }  
    else if(EPCOT5 == 1)
    {
     LCD_msg="Turn Off Toggle     ";
    }
   }
   EEPROM.write(32,EPCOT4);
   EEPROM.write(33,ERWPb7);
   EEPROM.write(34,ERWPb8);
   Set_view4(4);
  }
 }
}
