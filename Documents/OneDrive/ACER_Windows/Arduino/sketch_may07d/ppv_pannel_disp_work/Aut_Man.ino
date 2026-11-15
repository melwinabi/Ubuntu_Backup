void Aut_Man()
{
 if(EMode1 == 1)
{
  Mode1 = "ON";
}
else
{
  Mode1 = "OFF";
}

if(EMode2 == 1)
{
  Mode2 = "ON";
}
else
{
  Mode2 = "OFF";
}

if(EMode3 == 1)
{
  Mode3 = "ON";
}
else
{
  Mode3 = "OFF";
}

if(EMode4 == 1)
{
  Mode4 = "ON";
}
else
{
  Mode4 = "OFF";
}

if(EMode5 == 1)
{
  Mode5 = "ON";
}
else
{
  Mode5 = "OFF";
}
  
 if(A == LOW)
 {
  lcd.clear();
  A_M1();
  S_f = 1;
 }
 if(Flag == 14)
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
    lcd.clear();
    A_M2(); 
   }
   curs(C);
  }
  if(S == LOW)
  {   
   if(C == 0)
   {
    if(Mode1 == "OFF")
    {
     Mode1="ON "; 
     EMode1=1;
    }
    else
    {
     Mode1="OFF"; 
     EMode1=0;
    }
    A_M1();
    EEPROM.write(55, EMode1);
   }

   if(C == 1)
   {
    if(Mode2 == "OFF")
    {
     Mode2="ON ";
     EMode2=1;  
    }
    else
    {
     Mode2="OFF";
     EMode2=0; 
    }
    A_M1();
    EEPROM.write(56, EMode2);
   }

   if(C == 2)
   {
    if(Mode3 == "OFF")
    {
     Mode3="ON ";
     EMode3=1;  
    }
    else
    {
     Mode3="OFF";
     EMode3=0; 
    }
    A_M1();
    EEPROM.write(57, EMode3);
   }

   if(C == 3)
   {
    if(Mode4 == "OFF")
    {
     Mode4="ON "; 
     EMode4=1; 
    }
    else
    {
     Mode4="OFF";
     EMode4=0; 
    }
    A_M1();
    EEPROM.write(58, EMode4);
   }
  }
 }
 
 if(Flag == 15)
 {
  if(U == LOW)
  { 
   C=3;
   A_M1();
  }
 
 if(S == LOW)
 {
   if(C == 0)
   {
    if(Mode5 == "OFF")
    {
     Mode5="ON ";
     EMode5=1;  
    }
    else
    {
     Mode5="OFF"; 
     EMode5=0;
    }
    EEPROM.write(59, EMode5);
    A_M2();
   }
 }
}
}


void A_M1(){
   Flag = 14;
   curs(C);
   lcd.setCursor(1, 0);
   lcd.print("R.W.P : ");
   lcd.setCursor(8, 0);
   lcd.print(Mode1);
   lcd.setCursor(1, 1);
   lcd.print("AR.BL : ");
   lcd.setCursor(8, 1);
   lcd.print(Mode2);
   lcd.setCursor(1, 2);
   lcd.print("R.C.P : ");
   lcd.setCursor(8, 2);
   lcd.print(Mode3);
   lcd.setCursor(1, 3);
   lcd.print("F.F.P : ");
   lcd.setCursor(8, 3);
   lcd.print(Mode4);

}
void A_M2(){
   Flag = 15;
   curs(C);
   lcd.setCursor(1, 0);
   lcd.print("T.W.P : ");
   lcd.setCursor(8, 0);
   lcd.print(Mode5);
}
