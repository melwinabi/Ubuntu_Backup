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
    Serial.println("Date & Time");
   }
  }
 }
} 
