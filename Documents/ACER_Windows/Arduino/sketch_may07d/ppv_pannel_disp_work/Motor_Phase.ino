void Flag_3(){
   if(Flag == 3)
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
    set_A2();
   }
   curs(C);
  }
  if(S == LOW)
  {
   if(C == 0)
   {
    C=0;
    S=HIGH;
    Set_view1(1);
   }
   else if(C == 1)
   {
    C=0;
    S=HIGH;
    Set_view1(2);
   }
   else if(C == 2)
   {
    C=0;
    S=HIGH;
    Set_view1(3);
   }
   else if(C == 3)
   {
    C=0;
    S=HIGH;
    Set_view1(4);
   }
  }
 }
}

void Flag_4(){
 if(Flag == 4)
 {
  if(U == LOW){
    C=3;
    set_A1();
  }
  if(S == LOW)
  {
   if(C == 0)
   {
    Set_view1(5); 
   }
  }
 }
}

void Flag_16()
{
 if(Flag == 16)
 {
  if(S == LOW)
  {
   if(Ph1 == 1)
   {
    Ph1=3;
   }
   else
   {
    Ph1=1;
   }
   case1=Ph1;
   EEPROM.write(5,case1);
  }
  case1=EEPROM.read(5);
  Set_view1(1);
 }
}

void Flag_17()
{
if(Flag == 17)
 {
  if(S == LOW)
  {
   if(Ph2 == 1)
   {
    Ph2=3;
   }
   else
   {
    Ph2=1;
   }
   case2=Ph2;
   EEPROM.write(6,case2);
  }
  case2=EEPROM.read(6);
  Set_view1(2);
 }
}

void Flag_18()
{
if(Flag == 18)
 {
  if(S == LOW)
  {
   if(Ph3 == 1)
   {
    Ph3=3;
   }
   else
   {
    Ph3=1;
   }
   case3=Ph3;
   EEPROM.write(7,case3);
  }
  case3=EEPROM.read(7);
  Set_view1(3);
 }
}
void Flag_19()
{
if(Flag == 19)
 {
  if(S == LOW)
  {
   if(Ph4 == 1)
   {
    Ph4=3;
   }
   else
   {
    Ph4=1;
   }
   case4=Ph4;
   EEPROM.write(8,case4);
  }
  case4=EEPROM.read(8);
  Set_view1(4);
 }
}
 
void Flag_20()
{
if(Flag == 20)
 {
  if(S == LOW)
  {
   if(Ph5 == 1)
   {
    Ph5=3;
   }
   else
   {
    Ph5=1;
   }
   case5=Ph5;
   EEPROM.write(9,case5);
  }
  case5=EEPROM.read(9);
  Set_view1(5);
 }
}
