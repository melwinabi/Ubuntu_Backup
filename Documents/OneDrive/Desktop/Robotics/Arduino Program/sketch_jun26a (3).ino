#include <LiquidCrystal.h>
LiquidCrystal lcd(11,12,10,9,8,7);
void setup()
{
int i,j,k;
lcd.begin(24,4);
}
void loop(){
int i,j,k;
for (k=0;k<=12;k++)
for (j=0;j<=60;j++)
for (i=0;i<=60;i++)
{
  lcd.setCursor(0,0);
  lcd.print(i);
  lcd.print(":");
  
  lcd.setCursor(3,0);
  lcd.print(j);
  lcd.print(":");
  
  lcd.setCursor(6,0);
   lcd.print(k);
   delay(1000);
  
  
  
 
}

    
{
  for (j=0;j<=60;j++)
 lcd.setCursor(0,2);
    lcd.print(j);
  {
   
    for (k=0;k<=12;k++);
    {
       lcd.setCursor(1,0);
    
     lcd.print(k);
     
     delay (100);
    lcd.clear();
      
    }
}
} 
} 
  
  
