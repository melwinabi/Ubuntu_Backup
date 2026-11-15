#include "blink_led.h"
void setup() {
  
  pinMode(LED_BUILTIN, OUTPUT);  
}


void loop() {
  led();                      
}
