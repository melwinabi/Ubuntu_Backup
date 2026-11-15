void AUT_MAN_Setting()
{


  if(Run_Flag == 1)
  {
   if(SET_FLAG == 1)
   {
     SEC1=Sec1;
     SEC2=Sec2;
     SEC3=Sec3;
     SEC32=Sec32;
     SEC4=Sec4;
     SEC5=Sec5;
     SET_FLAG=0;
   }
   //..............................................................AUTO MANUAL WORK DISTRIBUTION!
   
   if(EMode1 == 1)
   {    
    if(EmodeF1 == 1)
    {
        digitalWrite(LED11, HIGH);
        digitalWrite(LED12, HIGH); 
    }
    if(EmodeT1 == 1)
    {
      if(ERWPb1 == 1 || ERWPb2 == 1 ||EPCOT1 == 1)
      {
        Timer_RWP();
      }  
    } 
   } 
     
   if(EMode2 == 1)
   {    
    if(EmodeF2 == 1)
    {
        digitalWrite(LED21, HIGH);
        digitalWrite(LED22, HIGH); 
    }
    if(EmodeT2 == 1)
    {
      if(ERWPb3 == 1 || ERWPb4 == 1 ||EPCOT2 == 1)
      {
        Timer_AB();
      }  
    } 
   }

   if(EMode3 == 1)
   {    
    if(EmodeT3 == 1)
    {
      Timer_RCP();
    } 
   }
   
   if(EMode4 == 1)
   { 
    if(EmodeF4 == 1)
    {
        digitalWrite(LED41, HIGH);
        digitalWrite(LED42, HIGH); 
    }
    if(EmodeT4 == 1)
    {
      if(ERWPb7 == 1 || ERWPb8 == 1 ||EPCOT4 == 1)
      {
        Timer_FFP();
      }  
    } 
   }
   
   if(EMode5 == 1)
   {   
    if(EmodeF5 == 1)
    {
        digitalWrite(LED51, HIGH);
        digitalWrite(LED52, HIGH); 
    }
    if(EmodeT5 == 1)
    {
      if(ERWPb9 == 1 || ERWPb10 == 1 ||EPCOT5 == 1)
      {
        Timer_TWP();
      }  
    } 
   }   
  if(sec_flag1 == 0)
  {
    Timer_RWP_REV();
  }
  if(sec_flag2 == 0)
  {
    Timer_AB_REV();
  }

  if(sec_flag3 == 0)
  {
    Timer_RCP_REV();
  }

  if(sec_flag4 == 0)
  {
    Timer_FFP_REV();
  }

  if(sec_flag5 == 0)
  {
    Timer_TWP_REV();
  }  
 }
}
