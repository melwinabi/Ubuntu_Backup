void Flag_9()
{
 if(Flag == 9)
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
    set_D2(); 
   }
   curs(C);
  }
  if(S == LOW)
  {
  if(C == 0)
  {
   S=HIGH;
   C=0; 
   lcd.clear(); 
   Set_view5(1);  
  }

   else if(C == 1)
  {
   S=HIGH; 
   C=0;
   lcd.clear(); 
   Set_view5(2); 
  }

  else if(C == 2)
  {
   S=HIGH; 
   lcd.clear(); 
   Set_view5(3); 
  }

  else if(C == 3)
  {
   S=HIGH;
   lcd.clear(); 
   Set_view5(4); 
  }
 }
}
}
void Flag_10()
{
 if(Flag == 10)
 {
  if(U == LOW)
  {
    C=3;
    set_D1(); 
  }
  if(S == LOW)
  {
   if(C == 0)
  {
   S=HIGH;
   lcd.clear();
   Set_view5(5); 
  }
 }
}
}

void Flag_40()
{
    ESh1 = EEPROM.read(45);  
    ESm1 = EEPROM.read(46);  
    Sec1 = (ESh1 * 3600) + (ESm1 * 60); //= (hours * 60) + minutes;
   // Serial.println(Sec1);
    if (Flag == 40)
    {
        Sh1 = ESh1;
        Sm1 = ESm1;
        Set_view5(1);
        if (S == LOW)
        {
            C++;
            if (C > 1)
            {
                C = 0; 
            }
        }
        if (C == 0)
        {
            lcd.setCursor(4, 1); 
            lcd.print(">");
            lcd.setCursor(11, 1); 
            lcd.print(" "); 
        }
        if (C == 1)
        {
            lcd.setCursor(11, 1); 
            lcd.print(">");
            lcd.setCursor(4, 1); 
            lcd.print(" "); 
        }
        if (U == LOW)
        {
            if (C == 0)
            {
                Sh1--; 
                if (Sh1 < 0)
                {
                    Sh1 = 5; 
                }
            }
            if (C == 1)
            {
                Sm1--;
                if (Sm1 < 0)
                {
                    Sm1 = 60; 
                }
            }
            Set_view5(1);
            EEPROM.write(45, Sh1);
            EEPROM.write(46, Sm1); 
        }
        if (D == LOW)
        {
            if (C == 0)
            {
                Sh1++;
                if (Sh1 > 5)
                {
                    Sh1 = 0; 
                }
            }
            if (C == 1)
            {
                Sm1++;
                if (Sm1 > 60)
                {
                    Sm1 = 0; 
                }
            }
            Set_view5(1);
            EEPROM.write(45, Sh1);
            EEPROM.write(46, Sm1);  
        }
    }
}

void Flag_41()
{
    ESh2 = EEPROM.read(47);  
    ESm2 = EEPROM.read(48);  
    Sec2 = (ESh2 * 3600) + (ESm2 * 60); 
   // Serial.println(Sec2);
    if (Flag == 41)
    {
        Sh2 = ESh2;
        Sm2 = ESm2;
        Set_view5(2);
        if (S == LOW)
        {
            C++;
            if (C > 1)
            {
                C = 0; 
            }
        }
        if (C == 0)
        {
            lcd.setCursor(4, 1); 
            lcd.print(">");
            lcd.setCursor(11, 1); 
            lcd.print(" "); 
        }
        if (C == 1)
        {
            lcd.setCursor(11, 1); 
            lcd.print(">");
            lcd.setCursor(4, 1); 
            lcd.print(" "); 
        }
        if (U == LOW)
        {
            if (C == 0)
            {
                Sh2--; 
                if (Sh2 < 0)
                {
                    Sh2 = 5; 
                }
            }
            if (C == 1)
            {
                Sm2--;
                if (Sm2 < 0)
                {
                    Sm2 = 60; 
                }
            }
            Set_view5(2);
            EEPROM.write(47, Sh2);
            EEPROM.write(48, Sm2);  
        }
        if (D == LOW)
        {
            if (C == 0)
            {
                Sh2++;
                if (Sh2 > 5)
                {
                    Sh2 = 0; 
                }
            }
            if (C == 1)
            {
                Sm2++;
                if (Sm2 > 60)
                {
                    Sm2 = 0; 
                }
            }
            Set_view5(2);
            EEPROM.write(47, Sh2);
            EEPROM.write(48, Sm2); 
        }
    }
}

void Flag_42()
{
    ESh3 = EEPROM.read(49);  
    ESm3 = EEPROM.read(50);  
    ESh32 = EEPROM.read(61);  
    ESm32 = EEPROM.read(62);
    Sec3 = (ESh3 * 3600) + (ESm3 * 60); 
    Sec32 = (ESh32 * 3600) + (ESm32 * 60); 
   // Serial.println(Sec3);
    if (Flag == 42)
    {
        Sh3 = ESh3;
        Sm3 = ESm3;
        Sh32 = ESh32;
        Sm32 = ESm32;
        Set_view5(3);
        if (S == LOW)
        {
            C++;
            if (C > 3)
            {
                C = 0; 
            }
        }
        if (C == 0)
        {
          ///on tomr off time         
            lcd.setCursor(8, 1); 
            lcd.print(">");
            lcd.setCursor(13, 1); 
            lcd.print(" ");
            lcd.setCursor(8, 2); 
            lcd.print(" ");
            lcd.setCursor(13, 2); 
            lcd.print(" "); 
        }
        if (C == 1)
        {
            lcd.setCursor(8, 1); 
            lcd.print(" ");
            lcd.setCursor(13, 1); 
            lcd.print(">");
            lcd.setCursor(8, 2); 
            lcd.print(" ");
            lcd.setCursor(13, 2); 
            lcd.print(" "); 
        }
        if (C == 2)
        {
            lcd.setCursor(8, 1); 
            lcd.print(" ");
            lcd.setCursor(13, 1); 
            lcd.print(" ");
            lcd.setCursor(8, 2); 
            lcd.print(">");
            lcd.setCursor(13, 2); 
            lcd.print(" "); 
        }
        if (C == 3)
        {
            lcd.setCursor(8, 1); 
            lcd.print(" ");
            lcd.setCursor(13, 1); 
            lcd.print(" ");
            lcd.setCursor(8, 2); 
            lcd.print(" ");
            lcd.setCursor(13, 2); 
            lcd.print(">"); 
        }
        if (U == LOW)
        {
            if (C == 0)
            {
                Sh3--; 
                if (Sh3 < 0)
                {
                    Sh3 = 5; 
                }
            }
            else if (C == 1)
            {
                Sm3--;
                if (Sm3 < 0)
                {
                    Sm3 = 60; 
                }
            }
            else if (C == 2)
            {
                Sh32--;
                if (Sh32 < 0)
                {
                    Sh32 = 5; 
                }
            }
            else if (C == 3)
            {
                Sm32--;
                if (Sm32 < 0)
                {
                    Sm32 = 60; 
                }
            }            
            Set_view5(3);
            EEPROM.write(49, Sh3);
            EEPROM.write(50, Sm3);
            EEPROM.write(61, Sh32);
            EEPROM.write(62, Sm32);                
        }
        if (D == LOW)
        {
            if (C == 0)
            {
                Sh3++;
                if (Sh3 > 5)
                {
                    Sh3 = 0; 
                }
            }
            else if (C == 1)
            {
                Sm3++;
                if (Sm3 > 60)
                {
                    Sm3 = 0; 
                }
            }
            else if (C == 2)
            {
                Sh32++;
                if (Sh32 > 5)
                {
                    Sh32 = 0; 
                }
            }
            else if (C == 3)
            {
                Sm32++;
                if (Sm32 > 60)
                {
                    Sm32 = 0; 
                }
            }   
            Set_view5(3);
            EEPROM.write(49, Sh3);
            EEPROM.write(50, Sm3);
            EEPROM.write(61, Sh32);
            EEPROM.write(62, Sm32);              
        }
    }
}

void Flag_43()
{
    ESh4 = EEPROM.read(51);  
    ESm4 = EEPROM.read(52);  
    Sec4 = (ESh4 * 3600) + (ESm4 * 60); 
   // Serial.println(Sec4);
    if (Flag == 43)
    {
        Sh4 = ESh4;
        Sm4 = ESm4;
        Set_view5(4);
        if (S == LOW)
        {
            C++;
            if (C > 1)
            {
                C = 0; 
            }
        }
        if (C == 0)
        {
            lcd.setCursor(4, 1); 
            lcd.print(">");
            lcd.setCursor(11, 1); 
            lcd.print(" "); 
        }
        if (C == 1)
        {
            lcd.setCursor(11, 1); 
            lcd.print(">");
            lcd.setCursor(4, 1); 
            lcd.print(" "); 
        }
        if (U == LOW)
        {
            if (C == 0)
            {
                Sh4--; 
                if (Sh4 < 0)
                {
                    Sh4 = 5; 
                }
            }
            if (C == 1)
            {
                Sm4--;
                if (Sm4 < 0)
                {
                    Sm4 = 60; 
                }
            }
            Set_view5(4);
            EEPROM.write(51, Sh4);
            EEPROM.write(52, Sm4);  
        }
        if (D == LOW)
        {
            if (C == 0)
            {
                Sh4++;
                if (Sh4 > 5)
                {
                    Sh4 = 0; 
                }
            }
            if (C == 1)
            {
                Sm4++;
                if (Sm4 > 60)
                {
                    Sm4 = 0; 
                }
            }
            Set_view5(4);
            EEPROM.write(51, Sh4);
            EEPROM.write(52, Sm4); 
        }
    }
}

void Flag_44()
{
    ESh5 = EEPROM.read(53);  
    ESm5 = EEPROM.read(54); 
    Sec5 = (ESh5 * 3600) + (ESm5 * 60); 
   // Serial.println(Sec5); 
    if (Flag == 44)
    {
        Sh5 = ESh5;
        Sm5 = ESm5;
        Set_view5(5);
        if (S == LOW)
        {
            C++;
            if (C > 1)
            {
                C = 0; 
            }
        }
        if (C == 0)
        {
            lcd.setCursor(4, 1); 
            lcd.print(">");
            lcd.setCursor(11, 1); 
            lcd.print(" "); 
        }
        if (C == 1)
        {
            lcd.setCursor(11, 1); 
            lcd.print(">");
            lcd.setCursor(4, 1); 
            lcd.print(" "); 
        }
        if (U == LOW)
        {
            if (C == 0)
            {
                Sh5--; 
                if (Sh5 < 0)
                {
                    Sh5 = 5; 
                }
            }
            if (C == 1)
            {
                Sm5--;
                if (Sm5 < 0)
                {
                    Sm5 = 60; 
                }
            }
            Set_view5(5);
            EEPROM.write(53, Sh5);
            EEPROM.write(54, Sm5);  
        }
        if (D == LOW)
        {
            if (C == 0)
            {
                Sh5++;
                if (Sh5 > 5)
                {
                    Sh5 = 0; 
                }
            }
            if (C == 1)
            {
                Sm5++;
                if (Sm5 > 60)
                {
                    Sm5 = 0; 
                }
            }
            Set_view5(5);
            EEPROM.write(53, Sh5);
            EEPROM.write(54, Sm5);  
        }
    }
}
