const int trigPin = 11;//ache  
const int echoPin =10 ;//blue  
volatile unsigned long startTime; 
volatile unsigned long endTime;   
volatile boolean pulseDetected = false; 

void setup() {
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);
  attachInterrupt(digitalPinToInterrupt(echoPin), echoInterrupt, CHANGE);
  Serial.begin(9600);
}

void loop() {
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);

  if (pulseDetected) {
  unsigned duration = pulseIn(echoPin, HIGH);
  unsigned distance= duration * 0.0344 / 2; 
}
}
void echoInterrupt() {
  int j=0;
  if (digitalRead(echoPin) == HIGH) {
    
    Serial.print("OPEN");
  } else {
    
    Serial.print("CLOSE");
     j++;
  }
}
