#include <Servo.h>
#define trigPin 10 //ache
#define echoPin    12  //blue                   
Servo s;
long duration; 

int distance;


void setup()
{
pinMode(trigPin,OUTPUT); 
pinMode(echoPin, INPUT);
s.attach(11);//servo pin
s.write(40);//servo degree
Serial.begin(9600);
}
//void loop()
//{
//  digitalWrite(trigPin, LOW);
//  delayMicroseconds(2);
//  digitalWrite(trigPin,HIGH); 
//  delayMicroseconds(10);
//  digitalWrite(trigPin,LOW);
//  duration = pulseIn(echoPin, HIGH);
//  distance= duration * 0.0344 / 2;
//  Serial.print("Distance: ");
//  Serial.print(distance);
//  Serial.println(" cm");
//  if(distance<10){
//    s.write(150);
//  } else{
//     s.write(0); 
//    }
//  
//}
