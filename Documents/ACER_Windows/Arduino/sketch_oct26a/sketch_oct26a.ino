int pin;
void setup() {
  // Define pins 31 to 41 as outputs
  for (int pin = 31; pin < 41; pin++) {
    pinMode(pin, OUTPUT);
  }
}

void loop() {
    for (int pin = 31; pin < 41; pin++) {
    pinMode(pin, OUTPUT);
 digitalWrite(pin, HIGH);
 delay(725);
 digitalWrite(pin, LOW);

    }
 }
