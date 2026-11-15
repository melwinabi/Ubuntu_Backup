
#include <Servo.h>

Servo s1;  // create servo object to control a servo
// twelve servo objects can be created on most boards

int pos = 0;    // variable to store the servo position

void setup() {
  s1.attach(10);  // attaches the servo on pin 9 to the servo object
}

void loop() {
  s1.write(40);
    
}
