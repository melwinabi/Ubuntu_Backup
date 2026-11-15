void Aut_Man()
{
 if(EMode1 == 1)
 {
  Mode1 = "AUT";
 }
 else
 {
  Mode1 = "MAN";
 }
 if(EMode2 == 1)
 {
  Mode2 = "AUT";
 }
 else
 {
  Mode2 = "MAN";
 }
 if(EMode3 == 1)
 {
  Mode3 = "AUT";
 }
 else
 {
  Mode3 = "MAN";
 }
 if(EMode4 == 1)
 {
  Mode4 = "AUT";
 }
 else
 {
  Mode4 = "MAN";
 }
 if(EMode5 == 1)
 {
  Mode5 = "AUT";
 }
 else
 {
  Mode5 = "MAN";
 }
 if(EMode6 == 1)
 {
  Mode6 = "AUT";
 }
 else
 {
  Mode6 = "MAN";
 }
 if(A == LOW)
 {
  Run_Flag=0;
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
    D=HIGH;
    A_M2(); 
   }
   curs(C);
  }
  if(S == LOW)
  {   
   if(C == 0)
   {
    if(Mode1 == "MAN")
    {
     Mode1="AUT"; 
     EMode1=1;
    }
    else
    {
     Mode1="MAN"; 
     EMode1=0;
    }
    A_M1();
    EEPROM.write(55, EMode1);
   }

   if(C == 1)
   {
    if(Mode2 == "MAN")
    {
     Mode2="AUT";
     EMode2=1;  
    }
    else
    {
     Mode2="MAN";
     EMode2=0; 
    }
    A_M1();
    EEPROM.write(56, EMode2);
   }

   if(C == 2)
   {
    if(Mode3 == "MAN")
    {
     Mode3="AUT";
     EMode3=1;  
    }
    else
    {
     Mode3="MAN";
     EMode3=0; 
    }
    A_M1();
    EEPROM.write(57, EMode3);
   }

   if(C == 3)
   {
    if(Mode4 == "MAN")
    {
     Mode4="AUT"; 
     EMode4=1; 
    }
    else
    {
     Mode4="MAN";
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
    C--;
    if(C < 0)
    {
     C=3;
     curs(C);
     A_M1(); 
    }
    curs(C);
  }
  if(D == LOW)
  {
   C++;
   if(C > 1)
   {
    C=0; 
   }
   curs(C);
  }
  if(S == LOW)
  {
   if(C == 0)
   {
    if(Mode5 == "MAN")
    {
      Mode5="AUT";
      EMode5=1;
    }
    else
    {
     Mode5="MAN";
     EMode5=0;  
    }
    EEPROM.write(59, EMode5);
   }
   else if(C == 1)
   {
    if(Mode6 == "MAN")
    {
     Mode6="AUT";
     EMode6=1; 
    }
    else
    {
     Mode6="MAN";
     EMode6=0; 
    }
    EEPROM.write(60, EMode6);
   }
   A_M2();
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
   lcd.setCursor(1, 1);
   lcd.print("D.W.P : ");
   lcd.setCursor(8, 1);
   lcd.print(Mode6);
}
//dp dossing pump
