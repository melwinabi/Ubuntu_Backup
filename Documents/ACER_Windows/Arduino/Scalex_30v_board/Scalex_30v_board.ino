// Program Updated on 01/06/2025 for 12 Volt SMPS

#include <EEPROM.h>

const int OnOffRelay = 6;     // SMPS Output ON/OFF
const int PolarityRelay = 5;  // SMPS Polarity Changeover
const int TempSensor = A1;    // Read Temp Sensor
const int StatusLed = 2;      // SMPS Polarity Changeover
const int Fan = 7;            // SMPS Output ON/OFF

int minutes = 5; // Updated on 12/05/2024

int Vo;
float R1 = 10000;
float logR2, R2, T;
float c1 = 1.009249522e-03, c2 = 2.378405444e-04, c3 = 2.019202697e-07;

int TimeLocation = 10;
int ChangeLocation = 20;                                                                                                                                                                                                                                    
int CounterLocation_M = 30;
int CounterLocation_H = 40;
int CounterLocation_D = 50;
int CounterLocation_Y = 60;

int TimeCount;
int ChangeCount;
int Counting_M, Counting_H, Counting_D, Counting_Y;
int passedminutes;
int PolRelayState;

void setup() 
{
  Serial.begin(9600);
  delay(5000);
  pinMode(OnOffRelay, OUTPUT);
  pinMode(PolarityRelay, OUTPUT);
  pinMode(StatusLed, OUTPUT);
  pinMode(Fan, OUTPUT);

  passedminutes = EEPROM.read(TimeLocation);
  PolRelayState = EEPROM.read(ChangeLocation);
  Counting_M = EEPROM.read(CounterLocation_M);
  Counting_H = EEPROM.read(CounterLocation_H);
  Counting_D = EEPROM.read(CounterLocation_D);
  Counting_Y = EEPROM.read(CounterLocation_Y);

  if (PolRelayState == 1) {
    digitalWrite(PolarityRelay, HIGH);
  } else {
    digitalWrite(PolarityRelay, LOW);
  }
}

void loop() {
  Vo = analogRead(TempSensor);
  R2 = R1 * (1023.0 / (float)Vo - 1.0);
  logR2 = log(R2);
  T = (1.0 / (c1 + c2 * logR2 + c3 * logR2 * logR2 * logR2));
  T = T - 273.15;             
  T = (T * 9.0) / 5.0 + 32.0;

  Serial.print("Current Temperature: ");
  Serial.print(T);
  Serial.println(" °F");

  if (T > 130) 
  {
    digitalWrite(Fan, HIGH);
    Serial.println("Fan ON");
  }
  if (T < 120) 
  {
    digitalWrite(Fan, LOW);
  }
  if (T < 175) // 175
  {
    Serial.println("Temperature Okay");

    Serial.print("Passed Minutes : ");
    Serial.println(passedminutes - 1);

    Serial.print("Last State : ");
    Serial.println(PolRelayState);

    Serial.print("Life Counter : ");
    Serial.print(Counting_Y);
    Serial.print(" Y, ");
    Serial.print(Counting_D);
    Serial.print(" D, ");
    Serial.print(Counting_H);
    Serial.print(" H, ");
    Serial.print(Counting_M);
    Serial.println(" M;");

    digitalWrite(StatusLed, HIGH);
    digitalWrite(OnOffRelay, HIGH);


    for (int i = 0; i <= 60; i++) 
    {
      delay(1000);
      Serial.print("Count-down:");
      Serial.print(minutes - passedminutes);
      Serial.print(" Min, ");
      Serial.print(60 - i);
      Serial.println(" Sec");
    }

    if (passedminutes >= minutes) {
      passedminutes = 1;
      EEPROM.update(TimeLocation, passedminutes);
      LifeCount();
      ChangePolarity();
    } else {
      LifeCount();
      passedminutes++;
      EEPROM.update(TimeLocation, passedminutes);
    }

  } 
  else 
  {
    digitalWrite(Fan, HIGH);
    Serial.println("Fan ON");
    digitalWrite(OnOffRelay, LOW);
    Serial.print("System OFF Due to High Temperature : ");
    Serial.print(T);
    Serial.println(" °F");
    Serial.println("Please wait for 5 Min or Temperature Down");
    for (int i = 0; i < 5; i++) 
    {
      for (int j = 0; j < 30; j++) 
      {
        digitalWrite(StatusLed, HIGH);
        delay(1000);
        digitalWrite(StatusLed, LOW);
        delay(1000);
      }
      Vo = analogRead(TempSensor);
      R2 = R1 * (1023.0 / (float)Vo - 1.0);
      logR2 = log(R2);
      T = (1.0 / (c1 + c2 * logR2 + c3 * logR2 * logR2 * logR2));
      T = T - 273.15;
      T = (T * 9.0) / 5.0 + 32.0;
      if( T < 150)
      {
        Serial.print("Current Temperature : ");
        Serial.print(T);
        Serial.println(" °F.");
        Serial.println("System temperature reduced, back to running mode");
        i = 5;
      }
      else
      {
        Serial.print("Current Temperature : ");
        Serial.print(T);
        Serial.print(" °F. Not yet down, Please wait for : ");
        Serial.print(5-i);
        Serial.println(" Min");
      }
    }
  }
}

void ChangePolarity() {
  PolRelayState = EEPROM.read(ChangeLocation);

  if (PolRelayState == 0) 
  {
    PolRelayState = 1;
    EEPROM.write(ChangeLocation, PolRelayState);
    delay(500);
    //digitalWrite(OnOffRelay, LOW); 
    //delay(5000);
    digitalWrite(PolarityRelay, HIGH);
    delay(500);
    //digitalWrite(OnOffRelay, HIGH);
    //delay(5000);
  } else 
  {
    PolRelayState = 0;
    EEPROM.write(ChangeLocation, PolRelayState);
    delay(500);
    //digitalWrite(OnOffRelay, LOW);
    //delay(5000);
    digitalWrite(PolarityRelay, LOW);
    delay(500);
    //digitalWrite(OnOffRelay, HIGH);
    //delay(5000);
  }
  Serial.println("State Changed");
  delay(500);
}
void LifeCount() {
  Counting_M = EEPROM.read(CounterLocation_M);
  if (Counting_M >= 60) {
    EEPROM.update(CounterLocation_M, 0);
    Counting_H = EEPROM.read(CounterLocation_H);
    if (Counting_H >= 24) {
      EEPROM.update(CounterLocation_H, 0);
      Counting_D = EEPROM.read(CounterLocation_D);
      if (Counting_D >= 250) {
        EEPROM.update(CounterLocation_D, 0);
        Counting_Y = EEPROM.read(CounterLocation_Y);
        Counting_Y++;
        EEPROM.update(CounterLocation_Y, Counting_Y);
      } else {
        Counting_D++;
        EEPROM.update(CounterLocation_D, Counting_D);
      }
    } else {
      Counting_H++;
      EEPROM.update(CounterLocation_H, Counting_H);
    }
  } else {
    Counting_M++;
    EEPROM.update(CounterLocation_M, Counting_M);
  }
}
