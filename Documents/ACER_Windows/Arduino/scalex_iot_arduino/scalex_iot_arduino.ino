#include <EEPROM.h>

const char* System_ID = "100";

const int muxs0 = 5;
const int muxs1 = 6;
const int muxs2 = 8;
const int muxs3 = 7;
const int Enable = 9;
const int TempSensor = A2;
const int OnOffRelay = 11;
const int PolarityRelay = 12;
const int StatusRead = 10;

const float sensitivity = 0.066;  // For ACS712 30A sensor
const float Vref = 2.5;           // Midpoint for ACS712

float voltage_1, current_1;
float voltage_2, current_2;
float voltage_3, current_3;
float voltage_4, current_4;
float voltage_5, current_5;
float average_current = 0.0;
float sum_current = 0.0;

float R1 = 10000;
float logR2, R2, T;
float c1 = 1.009249522e-03, c2 = 2.378405444e-04, c3 = 2.019202697e-07;

int LM35;
int Buzzer = 13;
int minutes = 1 ;
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
int A2_read;
unsigned long previous_time = 0;
float Acutual_Volt;

String sender = "";

void setup() {
  Serial.begin(115200);
  pinMode(muxs0, OUTPUT);
  pinMode(muxs1, OUTPUT);
  pinMode(muxs2, OUTPUT);
  pinMode(muxs3, OUTPUT);
  pinMode(Enable, OUTPUT);
  pinMode(Buzzer, OUTPUT);
  pinMode(OnOffRelay, OUTPUT);
  pinMode(PolarityRelay, OUTPUT);
  pinMode(StatusRead, INPUT);
  delay(300);
  digitalWrite(Enable, LOW);
  digitalWrite(OnOffRelay, LOW);
  digitalWrite(PolarityRelay, LOW);
  delay(15000);
  passedminutes = EEPROM.read(TimeLocation);
  PolRelayState = EEPROM.read(ChangeLocation);
  Counting_M = EEPROM.read(CounterLocation_M);
  Counting_H = EEPROM.read(CounterLocation_H);
  Counting_D = EEPROM.read(CounterLocation_D);
  Counting_Y = EEPROM.read(CounterLocation_Y);
  delay(10);
  digitalWrite(PolarityRelay, PolRelayState == 1 ? HIGH : LOW);
  
  BuzzerPattern();
}

void loop() {
  for (int channel = 0; channel < 10; channel += 2) {
    int index = channel / 2 + 1;
    selectMuxChannel(channel);
    delay(50);
    int adcReading = analogRead(A0);
    float actualVoltage = (adcReading * 0.0526) - 4.165;
    Acutual_Volt = actualVoltage;  // Assign to global for printing if needed
    swap(index, actualVoltage);
    // Serial.print("Voltage Ch ");
    // Serial.print(channel);
    // Serial.print(" (V");
    // Serial.print(index);
    // Serial.print("): ");
    // Serial.print(Acutual_Volt);
    // Serial.println(" Volt");
    delay(50);
    selectMuxChannel(channel + 1);
    delay(50);
    sum_current = 0; // Reset sum before averaging
    for (int j = 0; j < 10; j++)  // j < 10 for 10 samples
    {
      int currentRaw = analogRead(A0);
      delay(10);  // Small delay to get stable readings
      float voltage = (currentRaw * 5.0) / 1023.0; // Use currentRaw, not average_current
      float current = (voltage - Vref) / sensitivity;
      sum_current += current;
    }
    average_current = sum_current / 10.0;
    // Assign current to proper variable
    switch (index) {
      case 1: current_1 = average_current; break;
      case 2: current_2 = average_current; break;
      case 3: current_3 = average_current; break;
      case 4: current_4 = average_current; break;
      case 5: current_5 = average_current; break;
    }
    // Serial.print("Current Ch ");
    // Serial.print(channel + 1);
    // Serial.print(" (C");
    // Serial.print(index);
    // Serial.print("): ");
    // Serial.print(average_current);
    // Serial.println(" Amps");
  }

  A2_read = analogRead(A2);
  if (A2_read > 0) {
    float voltage = (A2_read / 1023.0) * 5.0;
    float temperatureC = voltage / 0.01;
    LM35 = temperatureC;
    if(LM35 > 75)
    {
      digitalWrite(Buzzer,HIGH);
      delay(100);
    }
    else 
    {
      digitalWrite(Buzzer,LOW);
      delay(100);
    }    
  }

  if (digitalRead(StatusRead) == LOW) {
    if (LM35 < 75) {
      Serial.println("Temperature Okay");

      Serial.print("Passed Minutes : ");
      Serial.println(passedminutes - 1);

      Serial.print("Last State : ");
      Serial.println(PolRelayState);

      Serial.print("Life Counter : ");
      Serial.print(Counting_Y); Serial.print(" Y, ");
      Serial.print(Counting_D); Serial.print(" D, ");
      Serial.print(Counting_H); Serial.print(" H, ");
      Serial.print(Counting_M); Serial.println(" M;");

      String DOF = String(Counting_Y) + ":" +
                   String(Counting_D) + ":" +
                   String(Counting_H) + ":" +
                   String(Counting_M);

      sender = "http://sili.ppvserver.com/api/Siligreen/Data?deviceid=" + String(System_ID) +
               "&v1=" + String(voltage_1) + "&v2=" + String(current_1) +
               "&v3=" + String(voltage_2) + "&v4=" + String(current_2) +
               "&v5=" + String(voltage_3) + "&v6=" + String(current_3) +
               "&v7=" + String(voltage_4) + "&v8=" + String(current_4) +
               "&v9=" + String(voltage_5) + "&v10=" + String(current_5) +
               "&v11=" + String(LM35) + "&v12=" + String(DOF);
      Serial.println(sender);
      delay(100);
      digitalWrite(OnOffRelay, HIGH);

      for (int i = 0; i <= 60; i++) {
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

    } else {
      digitalWrite(OnOffRelay, LOW);
      for (int i = 0; i < 5; i++) {
        for (int j = 0; j < 30; j++) {
          digitalWrite(Buzzer, HIGH);
          delay(1000);
          digitalWrite(Buzzer, LOW);
          delay(1000);
        }

        A2_read = analogRead(A2);
        if (A2_read > 0) {
          float voltage = (A2_read / 1023.0) * 5.0;
          float temperatureC = voltage / 0.01;
          if (temperatureC < 75) {
            i = 5; // break outer loop
          }
        }
      }
    }
  }
}

void selectMuxChannel(int channel) {
  digitalWrite(muxs0, bitRead(channel, 0));
  digitalWrite(muxs1, bitRead(channel, 1));
  digitalWrite(muxs2, bitRead(channel, 2));
  digitalWrite(muxs3, bitRead(channel, 3));
}

void swap(int voltageIndex, float value) {
  switch (voltageIndex) {
    case 1: voltage_1 = value; break;
    case 2: voltage_2 = value; break;
    case 3: voltage_3 = value; break;
    case 4: voltage_4 = value; break;
    case 5: voltage_5 = value; break;
  }
}

void BuzzerPattern() {
  tone(Buzzer, 1000, 100); delay(150);
  tone(Buzzer, 1000, 100); delay(150);
  tone(Buzzer, 1000, 400); delay(450);
  tone(Buzzer, 1000, 100); delay(150);
  noTone(Buzzer);
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

void ChangePolarity() {
  PolRelayState = EEPROM.read(ChangeLocation);
  if (PolRelayState == 0) {
    PolRelayState = 1;
    EEPROM.write(ChangeLocation, PolRelayState);
    delay(500);
    digitalWrite(OnOffRelay, LOW);
    delay(10000);
    digitalWrite(PolarityRelay, HIGH);
  } else {
    PolRelayState = 0;
    EEPROM.write(ChangeLocation, PolRelayState);
    delay(500);
    digitalWrite(OnOffRelay, LOW);
    delay(10000);
    digitalWrite(PolarityRelay, LOW);
  }
}