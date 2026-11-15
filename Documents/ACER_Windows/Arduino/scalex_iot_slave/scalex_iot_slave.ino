// Program Updated on 01/06/2025 for 30 Volt SMPS Slave Control

const int OnOffRelay = 3;         // SMPS Output ON/OFF
const int PolarityRelay = 2;      // SMPS Polarity Changeover
const int OnOffRelay_in = 11;     // SMPS Output ON/OFF input
const int PolarityRelay_in = 12;  // SMPS Polarity Changeover input
const int StatusLed = 5;          // Over-temperature indicator
const int TempSensor = A0;        // Temp Sensor

int Vo;
float R1 = 10000;
float logR2, R2, T;
float c1 = 1.009249522e-03, c2 = 2.378405444e-04, c3 = 2.019202697e-07;

int OnOffRelayState;
int PolRelayState;

void setup() 
{
  pinMode(OnOffRelay_in, INPUT);
  pinMode(PolarityRelay_in, INPUT);

  pinMode(OnOffRelay, OUTPUT);
  pinMode(PolarityRelay, OUTPUT);
  pinMode(StatusLed, OUTPUT);

  digitalWrite(PolarityRelay, LOW);
  digitalWrite(OnOffRelay, LOW);
  delay(10000);
}

void loop() 
{
  Vo = analogRead(TempSensor);
  R2 = R1 * (1023.0 / (float)Vo - 1.0);
  logR2 = log(R2);
  T = (1.0 / (c1 + c2 * logR2 + c3 * logR2 * logR2 * logR2));
  T = T - 273.15;             
  T = (T * 9.0) / 5.0 + 32.0; // Convert to Fahrenheit

  if (T < 175) 
  {
    OnOffRelayState = digitalRead(OnOffRelay_in);
    PolRelayState = digitalRead(PolarityRelay_in);

    digitalWrite(OnOffRelay, OnOffRelayState);
    digitalWrite(PolarityRelay, PolRelayState);
    digitalWrite(StatusLed, LOW); // LED off when temp is normal
  }
  else
  {
    digitalWrite(OnOffRelay, LOW);   // Force off
    digitalWrite(StatusLed, HIGH);   // Indicate over-temp
  }
}
