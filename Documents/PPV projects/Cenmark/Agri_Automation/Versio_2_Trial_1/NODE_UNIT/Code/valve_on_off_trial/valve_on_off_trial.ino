const int EN_PIN  = 17;   // connect to module EN pad
const int CUT_PIN = 16;   // connect to module CUT/power-control pad
#define PIN_25 21
#define PIN_26 22

void setup() {
  Serial.begin(115200);
  pinMode(EN_PIN, OUTPUT);
  pinMode(CUT_PIN, OUTPUT);
  pinMode(PIN_25, OUTPUT);
  pinMode(PIN_26, OUTPUT);
  digitalWrite(PIN_25, LOW);   // GPIO 25 HIGH
  digitalWrite(PIN_26, LOW);   // GPIO 26 HIGH
  delay(1000);
  Serial.println("Test: EN=HIGH, CUT=LOW");
}

void loop() 
{
  digitalWrite(CUT_PIN, HIGH);
  delay(500);// must dealy for 12v charge
  digitalWrite(CUT_PIN, LOW);
  Serial.println("Boosted");
  delay(500);
  Serial.println("FD");
  digitalWrite(PIN_25, HIGH);   // GPIO 25 HIGH
  digitalWrite(PIN_26, LOW);   // GPIO 26 HIGH
  delay(1000);
  Serial.println("Released");
  digitalWrite(PIN_25, LOW);   // GPIO 25 HIGH
  digitalWrite(PIN_26, LOW);   // GPIO 26 HIGH
  delay(3000);
  digitalWrite(PIN_26, LOW);   // GPIO 25 HIGH
  digitalWrite(PIN_25, LOW);   // GPIO 26 HIGH
  delay(1000);
  digitalWrite(CUT_PIN, HIGH);
  delay(500);
  digitalWrite(CUT_PIN, LOW);
  Serial.println("Boosted");
  delay(500);
  Serial.println("BD");
  digitalWrite(PIN_26, HIGH);   // GPIO 25 HIGH
  digitalWrite(PIN_25, LOW);   // GPIO 26 HIGH
  delay(1000);
  Serial.println("Released");
  digitalWrite(PIN_26, LOW);   // GPIO 25 HIGH
  digitalWrite(PIN_25, LOW);   // GPIO 26 HIGH
  delay(3000);  
}

  // digitalWrite(CUT_PIN, HIGH);
  // delay(50);
  // digitalWrite(CUT_PIN, LOW);
  // delay(50);
  // digitalWrite(PIN_25, LOW);   // GPIO 25 HIGH
  // digitalWrite(PIN_26, LOW);   // GPIO 26 HIGH
  // delay(5000);
  // digitalWrite(PIN_26, HIGH);   // GPIO 25 HIGH
  // digitalWrite(PIN_25, LOW);   // GPIO 26 HIGH
  // delay(1000);
  // digitalWrite(CUT_PIN, HIGH);
  // delay(50);
  // digitalWrite(CUT_PIN, LOW);
  // delay(50);
  // digitalWrite(PIN_26, LOW);   // GPIO 25 HIGH
  // digitalWrite(PIN_25, LOW);   // GPIO 26 HIGH
  // delay(5000); 
  // Serial.println("HIGH");
  // digitalWrite(CUT_PIN, HIGH);
  // delay(2000);
  // Serial.println("LOW");
  // digitalWrite(CUT_PIN, LOW);
  // delay(2000);