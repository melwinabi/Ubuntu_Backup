

#include <Wire.h>
#define SLAVE_ADDR 0x02   // set to the address the scanner showed

volatile int got = 0;
char buf[32];
int idx = 0;

void receiveEvent(int howMany) {
  idx = 0;
  while (Wire.available() && idx < sizeof(buf)-1) {
    buf[idx++] = Wire.read();
  }
  buf[idx] = '\0';
  got = 1;
}

void setup() {
  Serial.begin(9600);
  Wire.begin(SLAVE_ADDR);
  Wire.onReceive(receiveEvent);
  Serial.print("Slave running at address 0x");
  Serial.println(SLAVE_ADDR, HEX);
}

void loop() {
  if (got) {
    Serial.print("Received: ");
    Serial.println(buf);
    got = 0;
  }
  delay(100);
}