#include<Servo.h>
Servo s;
void bltcnt() {
if (Serial.available()) {
String command = Serial.readStringUntil('\n');
if (command == "HAII") {
Serial.println("HAII SHAKE HAND"); 
for (int angle = 20; angle <= 120; angle += 1) {
    s.write(angle);
    delay(4000);

  }
  
}
} 
}
