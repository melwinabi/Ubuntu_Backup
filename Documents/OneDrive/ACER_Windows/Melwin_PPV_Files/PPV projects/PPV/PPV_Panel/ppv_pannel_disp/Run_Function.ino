void Timer_RWP()
{
  if(sec_flag1 == 1)
  {
  currentMillis1=millis();
  if (currentMillis1 - prMill_1 >= interval) 
  {
    prMill_1 = currentMillis1;
    SEC1--;
    //Serial.println(SEC1);
    if(SEC1 < 1)
    {
      REV1=Sec1;
      sec_flag1=0;
      if(EPCOT1 == 1)
      {
         digitalWrite(LED11, LOW);
         digitalWrite(LED12, LOW); 
      } 
    }
    else
    {
      sec_flag1 = 1;
      if(ERWPb1 == 1)
      {
        digitalWrite(LED11, HIGH);  
      }
      if(ERWPb2 == 1)
      {
        digitalWrite(LED12, HIGH);  
      }
      if(EPCOT1 == 1)
      {
        digitalWrite(LED11, HIGH);
        digitalWrite(LED12, LOW); 
      }               
     }   
   }
 }      
}


void Timer_RWP_REV()
{
     currentMillis1=millis(); 
     if (currentMillis1 - prMill_1 >= interval) 
    {
      prMill_1 = currentMillis1;
      REV1--;
     // Serial.println(REV1);
      if(REV1 < 1)
      {
        SEC1=Sec1;
        sec_flag1=1;  
        if(EPCOT1 == 1)
        {
          digitalWrite(LED11, LOW);
          digitalWrite(LED12, LOW);
        }        
      }
      else
      {
        sec_flag1=0;
        if(ERWPb1 == 1)
        {     
          digitalWrite(LED11, LOW);  //11=>1set-pumb1
        }
        if(ERWPb2 == 1)
        {
          digitalWrite(LED12, LOW);  
        }
        if(EPCOT1 == 1)
        {
          digitalWrite(LED11, LOW);
          digitalWrite(LED12, HIGH); 
       }    
      }
   } 
}


void Timer_AB()
{
 
  if(sec_flag2 == 1)
  {
    DateTime now = DS3231M.now();
  if (now.minute() != prMill_2) 
  {
    prMill_2 = now.minute();
    SEC2--;
    Serial.println(SEC2);
    if(SEC2 < 1)
    {
      if(EPCOT2 == 1)
      {
         digitalWrite(LED21, LOW);
         digitalWrite(LED22, LOW);        
      }
       REV2=Sec2;
       sec_flag2=0; 
    }
    else
    {
      sec_flag2 = 1;
      if(ERWPb3 == 1)
      {
        digitalWrite(LED21, HIGH);  
      }
      if(ERWPb4 == 1)
      {
        digitalWrite(LED22, HIGH);  
      }
      if(EPCOT2 == 1)
      {
        digitalWrite(LED21, HIGH);
        digitalWrite(LED22, LOW); 
      }               
     }   
   }
 }
}

void Timer_AB_REV()
{
     currentMillis2=millis();
     DateTime now = DS3231M.now();
   if (now.minute() != prMill_2) 
   {
     prMill_2 = now.minute();
     REV2--;
     Serial.println(REV2);
      if(REV2 < 1)
      {  
        if(EPCOT2 == 1)
        {
          digitalWrite(LED21, LOW);
          digitalWrite(LED22, LOW); 
        }  
        SEC2=Sec2;
        sec_flag2=1;      
      }
      else
      {
        sec_flag2=0;
        if(ERWPb3 == 1)
        {     
          digitalWrite(LED21, LOW);  //11=>1set-pumb1
        }
        if(ERWPb4 == 1)
        {
          digitalWrite(LED22, LOW);  
        }
        if(EPCOT2 == 1)
        {
          digitalWrite(LED21, LOW);
          digitalWrite(LED22, HIGH); 
       }    
      }
   } 
}


void Timer_FFP()
{
  if(sec_flag4 == 1)
  {
  currentMillis3=millis();
  if (currentMillis3 - prMill_3 >= interval) 
  {
    prMill_3 = currentMillis3;
    SEC4--;
    //Serial.println(SEC4);
    if(SEC4 < 1)
    {
      REV4=Sec4;
      sec_flag4=0;
      if(EPCOT4 == 1)
      {
         digitalWrite(LED41, LOW);
         digitalWrite(LED42, LOW); 
      } 
    }
    else
    {
      sec_flag4 = 1;
      if(ERWPb7 == 1)
      {
        digitalWrite(LED41, HIGH);  
      }
      if(ERWPb8 == 1)
      {
        digitalWrite(LED42, HIGH);  
      }
      if(EPCOT4 == 1)
      {
        digitalWrite(LED41, HIGH);
        digitalWrite(LED42, LOW); 
      }               
     }   
   }
 }
}

void Timer_FFP_REV()
{
     currentMillis3=millis();
     if (currentMillis3 - prMill_3 >= interval) 
    {
      prMill_3 = currentMillis3;
      REV4--;
      //Serial.println(REV4);
      if(REV4 < 1)
      {
        SEC4=Sec4;
        sec_flag4=1;  
        if(EPCOT4 == 1)
        {
          digitalWrite(LED41, LOW);
          digitalWrite(LED42, LOW);
        }        
      }
      else
      {
        sec_flag4=0;
        if(ERWPb7 == 1)
        {     
          digitalWrite(LED41, LOW);  //11=>1set-pumb1
        }
        if(ERWPb8 == 1)
        {
          digitalWrite(LED42, LOW);  
        }
        if(EPCOT4 == 1)
        {
          digitalWrite(LED41, LOW);
          digitalWrite(LED42, HIGH); 
       }    
      }
   } 
}

void Timer_TWP()
{
  if (sec_flag5 == 1)
  {
    currentMillis4 = millis();
    if (currentMillis4 - prMill_4 >= interval) 
    {
      prMill_4 = currentMillis4;
      SEC5--;
     // Serial.println("SEC5: " + String(SEC5));
      
      if (SEC5 < 1)
      {
        REV5 = Sec5;
        sec_flag5 = 0;
        if (EPCOT5 == 1)
        {
          digitalWrite(LED51, LOW);
          digitalWrite(LED52, LOW); 
        } 
      }
      else
      {
        if (ERWPb9 == 1)
        {
          digitalWrite(LED51, HIGH);  
        }
        if (ERWPb10 == 1)
        {
          digitalWrite(LED52, HIGH);  
        }
        if (EPCOT5 == 1)
        {
          digitalWrite(LED51, HIGH);
          digitalWrite(LED52, LOW); 
        }               
      }   
    }
  }
}

void Timer_TWP_REV()
{
  currentMillis4 = millis();
  if (currentMillis4 - prMill_4 >= interval) 
  {
    prMill_4 = currentMillis4;
    REV5--;
    //Serial.println("REV5: " + String(REV5));
    
    if (REV5 < 1)
    {
      SEC5 = Sec5;
      sec_flag5 = 1;
      if (EPCOT5 == 1)
      {
        digitalWrite(LED51, LOW);
        digitalWrite(LED52, LOW);
      }        
    }
    else
    {
      if (ERWPb9 == 1)
      {     
        digitalWrite(LED51, LOW);  
      }
      if (ERWPb10 == 1)
      {
        digitalWrite(LED52, LOW);  
      }
      if (EPCOT5 == 1)
      {
        digitalWrite(LED51, LOW);
        digitalWrite(LED52, HIGH); 
      }    
    }
  }
}



void Timer_RCP()
{
  if(sec_flag3 == 1)
  { 
  currentMillis5 = millis();  
  if (currentMillis5 - prMill_5 >= interval) 
  {
    prMill_5 = currentMillis5;
    SEC3--;
  //  Serial.println(SEC3);
    if(SEC3 < 1)
    {
      SEC32=Sec32;
      sec_flag3=0;
    }
    else
    {
      sec_flag3 = 1;
      digitalWrite(LED3, HIGH);                   
     }   
   }
  }
}


void Timer_RCP_REV()
{
  currentMillis5 = millis();
  if (currentMillis5 - prMill_5 >= interval) 
  {
    prMill_5= currentMillis5;
    SEC32--;
   // Serial.println(SEC32);
    if(SEC32 < 1)
    {
      SEC3=Sec3;
      sec_flag3=1;     
    }
    else
    {
      sec_flag3 = 0;
      digitalWrite(LED3, LOW);                   
     }   
   }
 }
