#define LDR_PIN 34   // Connect LDR to GPIO34 (analog input)
#define buzzer 2


void setup() {
  Serial.begin(115200);
  pinMode(LDR_PIN, INPUT);
  pinMode(buzzer, OUTPUT);
}

void loop() {
  int ldrValue = analogRead(LDR_PIN);  // Read analog value (0-4095)
 if(ldrValue < 1000)
 {
  digitalWrite(buzzer,HIGH);
  Serial.println("CAMERA DETECTED");
 }
 else 
 {
  digitalWrite(buzzer,LOW);
  Serial.println("NO CAMERA");
 }
 
}
