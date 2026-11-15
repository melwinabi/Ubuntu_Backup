#include <Servo.h>
Servo s1;
Servo s2;
int servoPin1 = 5;
int servoPin2 = 6;
void setup() {
  s1.attach(servoPin1);
  s2.attach(servoPin2);
  s1.write(0);
  s2.write(0);
}
