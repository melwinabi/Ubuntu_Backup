void Flag_1()
{
 if(Flag == 1)
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
    Menu2();
   }
   curs(C);
  }
  if(S == LOW)
  {
   if(C == 0)
   {
    C=0;
    S=HIGH;
    set_A1(); 
   }
   else if(C == 1)
   {
    C=0;
    S=HIGH;
    set_B1();
   }
   else if(C == 2)
   {
    C=0;
    S=HIGH;
    set_C1();
   }
   else if(C == 3)
   {
    C=0;
    S=HIGH;
    set_D1();
   }
  }
 }
} 

void Flag_2()
{  
 if(Flag == 2)
 {
  if(U == LOW) 
  {
   C--;
   if(C < 0)
   {
    C=3; 
    Menu(); 
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
    C=0;
    S=HIGH;
    set_E1(); 
   }
   else if(C == 1)
   {
    lcd.clear();
    C=1;
    curs(C);
    S=HIGH;
    Set_view6();
    //........Serial.println("Date & Time");
   }
  }
 }
} 

void Flag_48()
{
 if(Flag == 48)
 {
  if(S == LOW)
  {
   Run_Flag=1;
   //SET_FLAG=1; 
   S_f = 0; 
   setup();  
  }
  if(B == LOW )
  {
   Menu();
  } 
 }
} 

void Exit_Screen()
{
  Flag = 48;
  S=HIGH;
  for (int col = 0; col < 20; col++) {
    lcd.setCursor(col, 0);  
    lcd.print("*");  
  }
  for (int col = 0; col < 20; col++) {
    lcd.setCursor(col, 3);  
    lcd.print("*");  
  }
  for (int row = 0; row < 4; row++) {
    lcd.setCursor(0, row);  
    lcd.print("*");  
    lcd.setCursor(19, row);  
    lcd.print("*");  
  }

    lcd.setCursor(1, 1);  
    lcd.print(" ENTER THE SELECT"); 
    lcd.setCursor(6, 2);  
    lcd.print("TO EXIT"); 
}
  
  
