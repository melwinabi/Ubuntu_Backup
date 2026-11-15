#include <Servo.h>
Servo s;
void setup(){
  
  s.attach(11);
  s.write(0);
}
void loop (){
  front_ultra();
 if(distance<25){
   s.write(195);
  }
  else{
   s.write(0)
  }

  void front_ultra(){
   digitalWrite(trigerpin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigerpin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigerpin, LOW);
  duration = pulseIn(echo, HIGH);
  distance = duration * .034 / 2;
  // Serial.print("\t");
   Serial.println(distance);
  //delay(200);
}
