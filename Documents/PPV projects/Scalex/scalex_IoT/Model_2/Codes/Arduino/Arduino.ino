#include <Wire.h>
#include <EEPROM.h>
#include <SoftwareSerial.h>

#define INA260_ADDRESS 0x40
#define REG_CURRENT 0x01
#define REG_VOLTAGE 0x02
#define REG_POWER   0x03
#define Buzzer 11
#define Network 13
#define RS485CommControl 4
#define RS485Transmit    HIGH
#define RS485Receive     LOW

SoftwareSerial ESP32(6, 5);
SoftwareSerial rs485(2, 3); // RX, TX
//******************************************************************************************************
const int Device_ID = 105  ;
//******************************************************************************************************

uint8_t response[50];  // Buffer for Modbus response

const int PolarityRelay = 7;
const int OnOffRelay = 8;
unsigned long Current_millis;
unsigned long Previous_millis = 0;
unsigned long current_Millis_two;
unsigned long Previous_millis_two = 0;
unsigned long previousMillis_one = 0;
unsigned long lastBlink = 0;
unsigned long timerStart = 0;
unsigned long cooldownTimer = 0;

float current_mA = 0;
float voltage_mV = 0;
float power_mW = 0;
float Current_Flow = 0;
float Total_flow = 0;
float PT100 = 0;
float R1 = 10000;
float LM35 = 0;
float logR2, R2, T;
float c1 = 1.009249522e-03, c2 = 2.378405444e-04, c3 = 2.019202697e-07;
float totaliser_1, flow_1, totaliser_2, flow_2;

int index;
int Timer =  0;
int Vo;
int Counting_Y = 0;
int Counting_D = 0;
int Counting_H = 0;
int Counting_M = 0;
int Low_Salt_Led = 9;
int Temp_Led = 10;
int blynk = 0;
int minutes = 0;
int TimeLocation = 10;
int ChangeLocation = 20;
int CounterLocation_M = 30;
int CounterLocation_H = 40;
int CounterLocation_D = 50;
int CounterLocation_Y = 60;
int CountdownIndexLocation = 70;
int TimeCount;
int ChangeCount;
int passedminutes;
int PolRelayState;
int countdownIndex = 0;
int cooldownMinutesPassed = 0;

const int countdownSeconds = 179;//  179;

bool ledState = LOW;
bool inCountdown = false;
bool coolingActive = false;

String Polarity;
String Sender;
String Low_Salt_State = "False";


void setup() {
  Serial.begin(115200);
  ESP32.begin(115200);
  rs485.begin(9600);
  ESP32.setTimeout(100);
  Wire.begin();

  pinMode(OnOffRelay, OUTPUT);
  pinMode(Network, OUTPUT);
  pinMode(Buzzer, OUTPUT);
  pinMode(PolarityRelay, OUTPUT);
  pinMode(Low_Salt_Led, OUTPUT);
  pinMode(Temp_Led, OUTPUT);
  pinMode(RS485CommControl, OUTPUT);
  digitalWrite(RS485CommControl, RS485Receive);
  delay(100);
  EEPROM.update(ChangeLocation, 1);
  passedminutes = EEPROM.read(TimeLocation);
  PolRelayState = EEPROM.read(ChangeLocation);
  Counting_M = EEPROM.read(CounterLocation_M);
  Counting_H = EEPROM.read(CounterLocation_H);
  Counting_D = EEPROM.read(CounterLocation_D);
  Counting_Y = EEPROM.read(CounterLocation_Y);
  countdownIndex = EEPROM.read(CountdownIndexLocation);
  if (countdownIndex > countdownSeconds) countdownIndex = 0;
  sendFMRequest();
  if (PolRelayState == 1) {
    Polarity = "Forward";
    digitalWrite(PolarityRelay, LOW);
  } else {
    Polarity = "Forward";
    digitalWrite(PolarityRelay, LOW);
  }
  // Super beep sound pattern
  Buzer();
}

void loop() {
  External_Processes();

//**********************************************************************************************************
//RS485
current_Millis_two = millis();
if (current_Millis_two - Previous_millis_two >= 3000) 
{
    Previous_millis_two = current_Millis_two; // reset timer
    sendFMRequest();
    delay(10);
    //Serial.println("5 seconds passed!");
}
if (rs485.available()) {
  index = 0;
  while (rs485.available() && index < 50) {
    response[index] = rs485.read();
    //Serial.print(response[index], HEX); Serial.print(" ");
    index++;
  }
 // Serial.println("\nINDEX: " + String(index));
  delay(50);
  decodeFT650();
}


//**********************************************************************************************************

  Vo = analogRead(A1);
  R2 = R1 * (1023.0 / (float)Vo - 1.0);
  logR2 = log(R2);
  T = (1.0 / (c1 + c2 * logR2 + c3 * logR2 * logR2 * logR2)) - 273.15;
  T = (T * 9.0) / 5.0 + 32.0;
  PT100 = T;
  if (current_mA >= 0.5 && current_mA <= 2)
  {
    digitalWrite(Low_Salt_Led, HIGH);
    Low_Salt_State = "True";
  }
  else
  {
    Low_Salt_State = "False";
    digitalWrite(Low_Salt_Led, LOW);
  }
  if (T > 130 || LM35 > 75) {
    digitalWrite(Temp_Led, HIGH);
  } else if (T < 120 || LM35 < 65) {
    digitalWrite(Temp_Led, LOW);
  }

  if ((T < 175 && LM35 < 80) && !inCountdown && !coolingActive) {
    digitalWrite(OnOffRelay, HIGH);
    inCountdown = true;
    timerStart = millis();
  }

  if (inCountdown && millis() - timerStart >= 1000) {
    timerStart = millis();
    int remaining = countdownSeconds - countdownIndex;
    Serial.print("Count-down: ");
    Serial.print(remaining / 60 < 10 ? "0" : ""); Serial.print(remaining / 60);
    Serial.print(":");
    Serial.print(remaining % 60 < 10 ? "0" : ""); Serial.println(remaining % 60);

    countdownIndex++;

    if (countdownIndex % 60 == 0 && countdownIndex != 0) {
      LifeCount();
    }

    EEPROM.update(CountdownIndexLocation, countdownIndex);

    if (countdownIndex > countdownSeconds) {
      countdownIndex = 0;
      EEPROM.update(CountdownIndexLocation, 0);
      if (passedminutes >= minutes) {
        passedminutes = 1;
        EEPROM.update(TimeLocation, passedminutes);
        //ChangePolarity();
      } else {
        passedminutes++;
        EEPROM.update(TimeLocation, passedminutes);
      }
    }
  }
  if ((T > 180 || LM35 > 85)) 
  {
    digitalWrite(Buzzer, HIGH);
    delay(10);
    digitalWrite(OnOffRelay, LOW);
    digitalWrite(Temp_Led, HIGH);
    Serial.println("System OFF Due to High Temperature. Entering cooldown...");
    coolingActive = true;
    inCountdown = false;
    cooldownTimer = millis();
    cooldownMinutesPassed = 0;
  }

  if (coolingActive && millis() - cooldownTimer >= 60000) {
    cooldownTimer = millis();
    cooldownMinutesPassed++;

    Vo = analogRead(A1);
    R2 = R1 * (1023.0 / (float)Vo - 1.0);
    logR2 = log(R2);
    T = (1.0 / (c1 + c2 * logR2 + c3 * logR2 * logR2 * logR2)) - 273.15;
    T = (T * 9.0) / 5.0 + 32.0;
    PT100 = T;

    Serial.print("Cooldown minute "); Serial.print(cooldownMinutesPassed);
    Serial.print(": Current Temp = "); Serial.println(T);

    if (T < 150 || cooldownMinutesPassed >= 2) {
      Serial.println("Cooldown complete. Resuming system.");
      digitalWrite(Buzzer, LOW);
      digitalWrite(Temp_Led, LOW);
      delay(10);
      digitalWrite(OnOffRelay, HIGH);
      inCountdown = true;
      coolingActive = false;
    }
  }
}

void ChangePolarity() {
  PolRelayState = EEPROM.read(ChangeLocation);
  if (PolRelayState == 0) {
    PolRelayState = 1;
    EEPROM.update(ChangeLocation, PolRelayState);
    Polarity = "Reverse";
    Serial.println("Reverse");
    digitalWrite(OnOffRelay, LOW);
    delay(3000);
    digitalWrite(PolarityRelay, HIGH);
    digitalWrite(OnOffRelay, HIGH);
    delay(3000);
  } else {
    PolRelayState = 0;
    EEPROM.update(ChangeLocation, PolRelayState);
    Polarity = "Forward";
    Serial.println("Forward");
    digitalWrite(OnOffRelay, LOW);
    delay(3000);
    digitalWrite(PolarityRelay, LOW);
    digitalWrite(OnOffRelay, HIGH);
    delay(3000);
  }
  Serial.println("State Changed");
}

void LifeCount() {
  Counting_M = EEPROM.read(CounterLocation_M);
  Counting_M++;
  if (Counting_M >= 60) {
    Counting_M = 0;
    Counting_H = EEPROM.read(CounterLocation_H);
    Counting_H++;
    EEPROM.update(CounterLocation_H, Counting_H);

    if (Counting_H >= 24) {
      Counting_H = 0;
      Counting_D = EEPROM.read(CounterLocation_D);
      Counting_D++;
      EEPROM.update(CounterLocation_D, Counting_D);

      if (Counting_D >= 365) {
        Counting_D = 0;
        Counting_Y = EEPROM.read(CounterLocation_Y);
        Counting_Y++;
        EEPROM.update(CounterLocation_Y, Counting_Y);
      }
      EEPROM.update(CounterLocation_D, Counting_D);
    }
    EEPROM.update(CounterLocation_H, Counting_H);
  }
  EEPROM.update(CounterLocation_M, Counting_M);
}

void Sensing() {
  int16_t raw_current = readRegister(REG_CURRENT);
  uint16_t raw_voltage = readRegister(REG_VOLTAGE);
  uint16_t raw_power = readRegister(REG_POWER);

  current_mA = raw_current * 1.25 / 1000.0;
  voltage_mV = raw_voltage * 1.25 / 1000.0;
  power_mW = raw_power * 10.0 / 1000.0;
}

int16_t readRegister(uint8_t reg) {
  Wire.beginTransmission(INA260_ADDRESS);
  Wire.write(reg);
  Wire.endTransmission(false);
  Wire.requestFrom(INA260_ADDRESS, 2);

  unsigned long timeout = millis();
  while (Wire.available() < 2) {
    if (millis() - timeout > 100) return 0;
  }
  uint16_t val = Wire.read() << 8 | Wire.read();
  return (int16_t)val;
}

void External_Processes() {
  Current_millis = millis();
  if (Current_millis - previousMillis_one >= 5000) {
    previousMillis_one = Current_millis;

    int A0_read = analogRead(A0);
    int A1_read = analogRead(A1);

    if (A0_read >= 0) {
      float voltage = (A0_read / 1023.0) * 5.0;
      LM35 = voltage / 0.01;
    }
  }

  Sensing();
  String url = "http://sili.ppvserver.com/api/Siligreen/Data?";
  url += "deviceid=" + String(Device_ID);
  url += "&v1=" + String(voltage_mV, 2);
  url += "&v2=" + String(current_mA, 2);
  url += "&v3=" + String(power_mW, 2);
  url += "&v4=" + String(Current_Flow, 2);
  url += "&v5=" + String(Total_flow, 2); 
  url += "&v6=" + Polarity;
  url += "&v7=" + String(Counting_Y) + ":" + String(Counting_D) + ":" + String(Counting_H) + ":" + String(Counting_M);
  url += "&v8=" + String(LM35);
  url += "&v9=" + String(PT100, 2); 
  url += "&v10=" + Low_Salt_State;

  Sender = url;
  if (Current_millis - Previous_millis >= 60000) {
    Previous_millis = Current_millis;
    ESP32.println(Sender);
    Serial.println(Sender);
  }

  if (ESP32.available()) {
    String Read = ESP32.readStringUntil('\n');
    Read.trim();
    Serial.println(Read);
    if (Read.startsWith("S")) {
      blynk = 0;
    } else if (Read.startsWith("F")) {
      blynk = 1;
    }
  }

  if (blynk == 1) {
    handleBlink(50);
  } else {
    handleBlink(500);
  }
}

void handleBlink(unsigned long interval) {
  if (millis() - lastBlink >= interval) {
    lastBlink = millis();
    ledState = !ledState;
    digitalWrite(Network, ledState);
  }
}

void Buzer()
{
  for (int i = 0; i < 3; i++) {
    digitalWrite(Buzzer, HIGH);
    delay(100);  // short ON
    digitalWrite(Buzzer, LOW);
    delay(100);  // short OFF
  }

  delay(200);

  for (int i = 0; i < 5; i++) {
    digitalWrite(Buzzer, HIGH);
    delay(50);
    digitalWrite(Buzzer, LOW);
    delay(50);
  }

  digitalWrite(Buzzer, LOW); // Ensure buzzer is off  
}

void sendFMRequest() {
  //Serial.println("FM Request Sent");
  digitalWrite(RS485CommControl, RS485Transmit);
  delay(10);
  uint8_t request[] = { 0x01, 0x03, 0x00, 0x00, 0x00, 0x14, 0x45, 0xC5 }; // Read 20 registers
  rs485.write(request, sizeof(request));
  delay(10);
  digitalWrite(RS485CommControl, RS485Receive);
  delay(10);
}

uint32_t combineRegisters(uint16_t msw, uint16_t lsw) {
  return ((uint32_t)msw << 16) | lsw;
}

void decodeFT650() {
  if (index >= 45 && response[1] == 0x03) {
    // Read registers from index 3 onward (3 = byte count start)
    uint16_t total2_lsw   = (response[3+28] << 8) | response[3+29]; // 40017
    uint16_t total2_msw   = (response[3+30] << 8) | response[3+31]; // 40018
    uint16_t flow1_lsw    = (response[3+24] << 8) | response[3+25]; // 40015
    uint16_t flow1_msw    = (response[3+26] << 8) | response[3+27]; // 40016

    flow_1       = combineRegisters(flow1_msw, flow1_lsw) * 0.01;
    totaliser_2  = combineRegisters(total2_msw, total2_lsw) * 0.01;
    Current_Flow = flow_1;
    Total_flow   = totaliser_2;
    // Serial.print("Totaliser : "); Serial.println(totaliser_2, 2);
    // Serial.print("Flow :      "); Serial.println(flow_1, 2);
    // Serial.println("");
  } else {
    flow_1      = 0;
    totaliser_2 = 0;
  }
}
//final

//error - network
//sts - temp
//eer - salt