#include<Servo.h>

Servo s1;
Servo s2;

int trigPinH =2;
int trigPinS =3;
int echoPinH =4;
int echoPinS =5;
int durationH;
int durationS;
int distanceH;
int distanceS;

void setup(){

  Serial.begin(9600);
  s1.attach(9);
  s2.attach(10);
  s1.write(93);
  s2.write(60);
  pinMode(A0,OUTPUT);
  pinMode(A1,OUTPUT);
  pinMode(A2,OUTPUT);
  pinMode(A3,OUTPUT);
  }

void loop (){

  pinMode(trigPinH, OUTPUT);  //wave trig
  digitalWrite(trigPinH, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPinH, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPinH, LOW); //...
  delayMicroseconds(10);
  pinMode(echoPinH, INPUT);  // formula
  durationH = pulseIn(echoPinH, HIGH);
  distanceH = durationH * 0.0344 / 2;  //...

  Serial.println( distanceH );
  delay(100);

  
  //.....................

 
  pinMode(trigPinS, OUTPUT);  //wave trig
  digitalWrite(trigPinS, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPinS, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPinS, LOW); //...
  delayMicroseconds(10);

  pinMode(echoPinS, INPUT);  // formula
  durationS = pulseIn(echoPinS, HIGH);
  distanceS = durationS * 0.0344 / 2;  //...

 Serial.println( distanceS );
 delay(100);
 Serial.println( "  " );
 
 if (distanceH>20){
  s1.write(93);
  s2.write(60);
  forward();
 }
 if (distanceH<20){
  backward();
  side();
  }
}
   
  



 void side(){
  if (distanceS>20) {
    s2.write(10);
  }
  if (distanceS<20){
    s1.write(10);
    delay(1000);
    s2.write(110);
  }
  
 }


 void forward(){
  digitalWrite(A0,LOW);
  digitalWrite(A1,HIGH);
  digitalWrite(A2,LOW);
  digitalWrite(A3,HIGH);
 
 }

void backward(){
  digitalWrite(A0,HIGH);
  digitalWrite(A1,LOW);
  digitalWrite(A2,HIGH);
  digitalWrite(A3,LOW);
  delay (500);
  digitalWrite(A0,LOW);
  digitalWrite(A2,LOW);
delay(2000);
}


 
