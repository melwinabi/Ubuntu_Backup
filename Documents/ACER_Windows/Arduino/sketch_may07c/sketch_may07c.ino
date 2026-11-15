#include <Servo.h>
Servo s;
void setup() {
 s.attach(11);
  s.write(0); 
}
void loop() {
   s.write(0);//195 
}
