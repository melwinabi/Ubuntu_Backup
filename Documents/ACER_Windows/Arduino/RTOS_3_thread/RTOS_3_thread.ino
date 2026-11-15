#include <Arduino_FreeRTOS.h>
#include <semphr.h> 
#include <LiquidCrystal.h>
SemaphoreHandle_t xSerialSemaphore;
LiquidCrystal lcd(12,13,5,4,3,2);
int pin1 = 8;
int pin2 = 9;
int pin3 = 10;
int Read1;
int Read2;
int Read3;
int flag;
int x,y;
String State = "";
void button();
void Timer();

void setup() {
  Serial.begin(9600);
  lcd.begin(20,4);
  lcd.setCursor(0,0);
  lcd.print("Working ON RTOS");
  delay(2000);
  lcd.clear();
  if ( xSerialSemaphore == NULL )  
  {
    xSerialSemaphore = xSemaphoreCreateMutex();  
    if ( ( xSerialSemaphore ) != NULL )
      xSemaphoreGive( ( xSerialSemaphore ) );  
  }
  xTaskCreate(button,"RUN TIME",128, NULL ,1 ,NULL); 
  xTaskCreate(Timer,"READ BUTTON",128,NULL ,2 ,NULL); 
}

void loop(){
  
}

void button(){
  for (;;)
  {
  lcd.setCursor(0,0);
  lcd.print(millis()/1000);
  delay(2000);
  }
}
void Timer(){
  for (;;)
  {
  }
}
