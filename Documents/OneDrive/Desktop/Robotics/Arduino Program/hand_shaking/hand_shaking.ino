#include"bluetoothcnt.h"
void setup() {
Serial.begin(9600);
Serial.println(F("Arduino here, command me!"));
s.attach(3);
s.write(0);
}
void loop(){
bltcnt();
}
