#include <Servo.h>

Servo s;  // create servo object to control a servo
// twelve servo objects can be created on most boards

int pos = 0;    // variable to store the servo position

void setup() {
  s.attach(3);  // attaches the servo on pin 9 to the servo object
 s.write(0);
}

void loop() {

}
