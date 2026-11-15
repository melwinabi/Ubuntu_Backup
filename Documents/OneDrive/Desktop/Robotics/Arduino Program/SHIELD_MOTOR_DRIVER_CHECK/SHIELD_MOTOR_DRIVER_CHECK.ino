#include <AFMotor.h>
#define MOTOR3 3
#define MOTOR4 4

AF_DCMotor motor3(MOTOR3);
AF_DCMotor motor4(MOTOR4);
  
void setup() {
  
  motor3.setSpeed(255);
  motor4.setSpeed(255);


}

void loop() {
   motor3.run(FORWARD);
      motor4.run(FORWARD);

}
