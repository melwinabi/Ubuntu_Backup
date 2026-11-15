#include <Servo.h>
Servo myservo;  
int pos = 0; 
int trig=2;
int echo=3; 
int dis;
int dur;  
void setup() {
  Serial.begin(9600);
  digitalWrite (3,LOW);
  delayMicroseconds(2);
  digitalWrite(3,HIGH);
  delayMicroseconds(2);
  digitalWrite(3,LOW);
  delayMicroseconds(10);
  dur = pulseIn(3, HIGH);
  dis = dur*.034/2;
  Serial.print(dis);
  pinMode(8,OUTPUT);
myservo.attach(8);  
}
void loop(){
  if (dis<10){
    motor (); 
  }
  else{
    digitalWrite(8,LOW);
  }
}
void motor()
{
for (pos = 0; pos <= 130; pos += 1) {
myservo.write(pos);             
delay(10);                       
}
digitalWrite(8,LOW);                
delay(100);
}
