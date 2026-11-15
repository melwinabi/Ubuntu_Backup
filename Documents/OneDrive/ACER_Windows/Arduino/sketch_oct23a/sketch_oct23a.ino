
unsigned long prMil1 = 0;
unsigned long prMil2 = 0;
unsigned long prMil3 = 0;
unsigned long prMil4 = 0;
const long interval = 60000;


void setup() {
  Serial.begin(9600);
  Serial.print("Start");
}

void loop() {
  unsigned long currentMillis = millis();
  if (currentMillis - prMil1 >= interval) 
  {
      prMil1 = currentMillis;
      Serial.println("ALLERT1");
  }
  if (currentMillis - prMil2 >= interval) 
  {
      prMil2 = currentMillis;
      Serial.println("ALLERT2");
  }
}
