int irValue;
const int BUZZER_PIN = A5;
int State = 0;
int sensorValue = 0;
unsigned long previousMillis = 0;   
const unsigned long interval = 50;  // Buzzer toggle interval in ms

void setup() {
  Serial.begin(115200);
  
  pinMode(3, INPUT);       // D3 input
  pinMode(10, OUTPUT);     // D10 output
  pinMode(11, OUTPUT);     // D11 output
  pinMode(12, OUTPUT);     // D12 output
  pinMode(5, INPUT);       // IR sensor input
  pinMode(BUZZER_PIN, OUTPUT);
}

void loop() {
  int stateInput = digitalRead(3);  // Read D3
  irValue = digitalRead(5);         // Read IR sensor
  sensorValue = analogRead(A6);     // Read LDR sensor

  if (stateInput == HIGH) {
    // IR MODE
    digitalWrite(10, LOW);
    digitalWrite(11, HIGH);

    Serial.print("IR = ");
    Serial.println(irValue);

    if (irValue == LOW) {   // Object detected
      Buzz();               // Buzzer active
    } else {
      StopBuzz();           // Stop buzzer
    }
  } 
  else {
    // LDR MODE
    digitalWrite(10, HIGH);
    digitalWrite(11, LOW);

    Serial.print("LDR = ");
    Serial.println(sensorValue);

    if (sensorValue <= 90) {   // Dark condition
      Buzz();                  // Buzzer active
    } else {
      StopBuzz();              // Stop buzzer
    }
  }
}

// Non-blocking buzzer toggle function
void Buzz() {
  unsigned long currentMillis = millis();
  digitalWrite(11, LOW);
  digitalWrite(10, LOW);
  if (currentMillis - previousMillis >= interval) {
    previousMillis = currentMillis;
    State = !State;  // Toggle ON/OFF
    digitalWrite(BUZZER_PIN, State);
    digitalWrite(12, State);
  }
}

// Cleanly stop buzzer
void StopBuzz() {
  digitalWrite(BUZZER_PIN, LOW);
  digitalWrite(12, LOW);
  State = 0;
}
