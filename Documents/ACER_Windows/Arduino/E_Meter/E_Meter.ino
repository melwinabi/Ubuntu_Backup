#include <SPI.h>
#include "ADE7758.h"

ADE7758 EnergyMeter;

// Chip Select Pin
#define CS_PIN 10

// --- Calibration Constants ---
// Adjust these based on your circuit calibration
float V_SCALE    = 4682.0f;     // Convert AVRMS counts → Volts
float I_SCALE    = 142600.0f;   // Convert AIRMS counts → Amps
float WATT_SCALE = 0.0155f;     // Convert Power register → Watts
float VA_SCALE   = 0.37f;
float VAR_SCALE  = 0.05f;

void setup() {
  Serial.begin(115200);
  Serial.println("ADE7758 Initialization...");

  SPI.begin();
  if (!EnergyMeter.Init()) {
    Serial.println("ADE7758 Init Failed!");
    while (1);
  }
  Serial.println("ADE7758 Ready!");
  delay(1000);
}

void loop() {
  // Raw RMS values
  long raw_VA = EnergyMeter.getAVRMS();
  long raw_VB = EnergyMeter.getBVRMS();
  long raw_VC = EnergyMeter.getCVRMS();

  long raw_IA = EnergyMeter.getAIRMS();
  long raw_IB = EnergyMeter.getBIRMS();
  long raw_IC = EnergyMeter.getCIRMS();

  // --- Convert using scaling constants ---
  float V_A = raw_VA / V_SCALE;
  float V_B = raw_VB / V_SCALE;
  float V_C = raw_VC / V_SCALE;

  float I_A = raw_IA / I_SCALE;
  float I_B = raw_IB / I_SCALE;
  float I_C = raw_IC / I_SCALE;

  // Print scaled results
  Serial.println("-------------------------------");
  Serial.println("---- Instantaneous Values ----");
  Serial.print("V_A: "); Serial.print(V_A, 2); Serial.print(" V   ");
  Serial.print("I_A: "); Serial.print(I_A, 3); Serial.println(" A");

  Serial.print("V_B: "); Serial.print(V_B, 2); Serial.print(" V   ");
  Serial.print("I_B: "); Serial.print(I_B, 3); Serial.println(" A");

  Serial.print("V_C: "); Serial.print(V_C, 2); Serial.print(" V   ");
  Serial.print("I_C: "); Serial.print(I_C, 3); Serial.println(" A");

  Serial.println("-------------------------------");
  delay(1000);
}
