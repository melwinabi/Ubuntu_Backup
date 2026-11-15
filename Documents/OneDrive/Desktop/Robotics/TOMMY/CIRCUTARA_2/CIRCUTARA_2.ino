#include<Servo.h>  //global decleration
#include<LiquidCrystal.h>
LiquidCrystal lcd(0,1,2,3,4,5);
const int trigPinH = 6; // Trigger us  h
const int trigPinS = 8; // Trigger us s
const int echoPinH = 7; //Echo h
const int echoPinS = 9; // Echo s
Servo s1; // head servo 7
Servo s2; // steer servo 8
long durationS,distanceS,durationH,distanceH;
int pir = 12;
int PIRread;
  

void setup() {    // working setup
  
   lcd.begin(16,2);
   pinMode(6,OUTPUT);
   pinMode(8,OUTPUT);
   pinMode(7,INPUT);
   pinMode(9,INPUT);
    pinMode(A0,OUTPUT);
   pinMode(A1,OUTPUT);
    pinMode(A2,OUTPUT);
   pinMode(A3,OUTPUT);
   s1.attach(10); // head servo
   s2.attach(11); // steer servo
}

void loop (){
  
   long durationS,distanceS,durationH,distanceH;  //ultra sonic program start
   pinMode(trigPinH, OUTPUT);  //wave trig
   digitalWrite(trigPinH, LOW);
   delayMicroseconds(2);
   digitalWrite(trigPinH, HIGH);
   delayMicroseconds(10);
   digitalWrite(trigPinH, LOW); //..
  
   
   pinMode(trigPinS, OUTPUT);  //wave trig
   digitalWrite(trigPinS, LOW);
   delayMicroseconds(2);
   digitalWrite(trigPinS, HIGH);
   delayMicroseconds(10);
   digitalWrite(trigPinS, LOW); //..
   
   pinMode(echoPinS, INPUT);  // formula
   durationS = pulseIn(echoPinS, HIGH);
   distanceS = durationS * 0.0344;  //...


   pinMode(echoPinH, INPUT);  // formula
   durationH = pulseIn(echoPinH, HIGH);
   distanceH = durationH * 0.0344;  //...
  
   
   lcd.setCursor(0,0);
   lcd.print("cmS:");
   lcd.setCursor(4,0);
   lcd.print(distanceS);
   lcd.setCursor(0,1);
   lcd.print("cmH:");
   lcd.setCursor(4,1);
   lcd.print(distanceH);
      
   if (distanceH>30){

    digitalWrite(A0,HIGH);
    digitalWrite(A2,HIGH);
   
    
    }
   
   if (distanceH<30){
    lcd.setCursor(0,2);
  

     digitalWrite(A0,LOW);
     digitalWrite(A2,LOW);
     digitalWrite(A0,HIGH);
     digitalWrite(A2,HIGH);
     delay(500);
     digitalWrite(A0,LOW);
     digitalWrite(A2,LOW);
     distanceside();
   }

   int PIRread = digitalRead(12) ;
   if(PIRread==HIGH)
    {
      lcd.setCursor(0,3);
      lcd.print("ALERT");
    }

}
void distanceside(){
     if (distanceS>30){
      s2.write(60);
      forward();
     }
     else{
      turnhead();
     }

   
}

void  turnhead(){
  s1.write(60);
  if (distanceH>30){
    s2.write(90);
    forward();
  }

  
}




void forward(){
  digitalWrite(A0,HIGH);
    digitalWrite(A2,HIGH);
    digitalWrite(A1,LOW);
     digitalWrite(A3,LOW);
}
