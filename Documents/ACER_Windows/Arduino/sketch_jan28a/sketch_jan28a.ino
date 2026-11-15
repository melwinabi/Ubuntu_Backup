#include <OneWire.h>
#include <DallasTemperature.h>

#define TDS_PIN 34
#define ONE_WIRE_BUS 4

OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature sensors(&oneWire);

float temperature = 25.0;
float tdsValue = 0;

// CALIBRATION FACTOR (we will tune this)
float calibrationFactor = 1.0;

void setup() {
  Serial.begin(115200);
  sensors.begin();

  analogReadResolution(12);
  analogSetAttenuation(ADC_11db);

  Serial.println("Gravity TDS Sensor (ESP32) Ready");
}

void loop() {
  // --- Temperature ---
  sensors.requestTemperatures();
  temperature = sensors.getTempCByIndex(0);

  // --- Read ADC ---
  int rawADC = analogRead(TDS_PIN);
  float voltage = rawADC * (3.3 / 4095.0);

  // --- Temperature compensation ---
  float compensationCoefficient = 1.0 + 0.02 * (temperature - 25.0);
  float compensatedVoltage = voltage / compensationCoefficient;

  // --- DFRobot polynomial ---
  tdsValue = (133.42 * pow(compensatedVoltage, 3)
             -255.86 * pow(compensatedVoltage, 2)
             +857.39 * compensatedVoltage) * 0.5;

  // --- Apply calibration ---
  tdsValue *= calibrationFactor;

  Serial.print("Temp: "); Serial.print(temperature, 2); Serial.print(" °C  ");
  Serial.print("Voltage: "); Serial.print(voltage, 3); Serial.print(" V  ");
  Serial.print("TDS: "); Serial.print(tdsValue, 1); Serial.println(" ppm");

  delay(1000);
}
