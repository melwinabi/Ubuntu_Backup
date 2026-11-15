#include <SPI.h>
#include "ADE7758_AT.h"
#include <Wire.h>

#define SLAVE_ADDR 0x08
#define I2C_CHUNK 30 // < 32
volatile int rxIndex = 0;
volatile bool messageReady = false;

ADE7758 EnergyMeter;

float VR, VY, VB;
float IR, IY, IB;
float PR, PY, PB;
float QR, QY, QB;
float SR, SY, SB;
float PFR, PFY, PFB;

float TotW, TotVA, TotPF;
float kWh_total = 0.0f;

float freq;
uint8_t PhaseSeq;

float V_SCALE   = 4682.0f;
float I_SCALE   = 130000.0f;    // Adjust depending on CT/burden
float W_SCALE   = 0.0155f;
float VA_SCALE  = 0.37f;
float VAR_SCALE = 0.05f;

unsigned long lastKwhUpdate = 0;
unsigned long previousMillis = 0;

volatile uint16_t txIndex = 0; 

String txData = "";

int CT_data;

void setup() {
  Serial.begin(115200);
  Wire.begin(SLAVE_ADDR);     // I2C SLAVE
  Wire.onRequest(requestEvent);
  if(EnergyMeter.Init()){
    Serial.println("ADE7758 READY");
  } else {
    Serial.println("ADE7758 INIT FAILED!");
    while(1);
  }

  CalibrateADE();
}


void loop() {
  readADE();
  I2c();
  // Serial.println("\n=== ADE7758 Real-Time Measurements ===");
  // print3("Voltage (V):  ", VR, VY, VB);
  // print3("Current (A):  ", IR, IY, IB);
  // print3("Active (W):   ", PR, PY, PB);
  // print3("Reactive(VAR):", QR, QY, QB);
  // print3("Apparent(VA): ", SR, SY, SB);
  // print3("PF:           ", PFR, PFY, PFB);

  // Serial.print("Frequency: "); Serial.println(freq);

  // // Phase sequence display
  // Serial.print("Phase Sequence: ");
  // Serial.println(PhaseSeq ? "R-Y-B" : "R-B-Y");

  // Serial.println("--- Total Values ---");
  // Serial.print("Total kW: "); Serial.println(TotW / 1000.0f, 3);
  // Serial.print("Total kVA: "); Serial.println(TotVA / 1000.0f, 3);
  // Serial.print("Total PF:  "); Serial.println(TotPF, 3);
  // Serial.print("Energy kWh: "); Serial.println(kWh_total, 4);

  // Serial.println("--------------------------------------");
  if (IR > 1 || IY > 1 || IB > 1)
  {
    CT_data = 1;
  }
  else
  {
    CT_data = 0;
  }
  delay(1000);
}

void CalibrateADE(){
  EnergyMeter.setOpMode(0x04);
  EnergyMeter.gainSetup(INTEGRATOR_OFF, FULLSCALESELECT_0_5V, GAIN_1, GAIN_4);

  EnergyMeter.setLcycMode(0x0F);
  EnergyMeter.setLineCyc(20);

  EnergyMeter.setMaskInterrupts(ZXTOA | ZXTOB | ZXTOC | LENERGY | ZXA | ZXB | ZXC);

  EnergyMeter.setCompMode(0x24);
}

static inline float clampPF(float x){
  if(x > 1.0f) return 1.0f;
  if(x < -1.0f) return -1.0f;
  return x;
}

void readADE(){
  uint32_t status = EnergyMeter.getStatus();

  VR = EnergyMeter.getAVRMS() / V_SCALE;
  VY = EnergyMeter.getBVRMS() / V_SCALE;
  VB = EnergyMeter.getCVRMS() / V_SCALE;

  IR = EnergyMeter.getAIRMS() / I_SCALE;
  IY = EnergyMeter.getBIRMS() / I_SCALE;
  IB = EnergyMeter.getCIRMS() / I_SCALE;

  PR = EnergyMeter.getWatt(PHASE_A) * W_SCALE;
  PY = EnergyMeter.getWatt(PHASE_B) * W_SCALE;
  PB = EnergyMeter.getWatt(PHASE_C) * W_SCALE;

  QR = EnergyMeter.getVar(PHASE_A) * VAR_SCALE;
  QY = EnergyMeter.getVar(PHASE_B) * VAR_SCALE;
  QB = EnergyMeter.getVar(PHASE_C) * VAR_SCALE;

  SR = EnergyMeter.getVa(PHASE_A) * VA_SCALE;
  SY = EnergyMeter.getVa(PHASE_B) * VA_SCALE;
  SB = EnergyMeter.getVa(PHASE_C) * VA_SCALE;

  PFR = (SR != 0) ? clampPF(PR / SR) : 0;
  PFY = (SY != 0) ? clampPF(PY / SY) : 0;
  PFB = (SB != 0) ? clampPF(PB / SB) : 0;

  freq = EnergyMeter.getFreq() / 16.0f;

  // reverse logic since your hardware is swapped
  PhaseSeq = (status & SEQERR) ? 0 : 1;

  TotW  = PR + PY + PB;
  TotVA = SR + SY + SB;
  TotPF = (TotVA != 0) ? clampPF(TotW / TotVA) : 0;

  if(millis() - lastKwhUpdate > 1000){
    kWh_total += (TotW / 1000.0f) / 3600.0f;
    lastKwhUpdate = millis();
  }
}

void print3(const char *label, float r, float y, float b){
  Serial.print(label);
  Serial.print(" R="); Serial.print(r, 2);
  Serial.print("  Y="); Serial.print(y, 2);
  Serial.print("  B="); Serial.println(b, 2);
}

void I2c()
{
  unsigned long currentMillis = millis();
  if (currentMillis - previousMillis >= 3000) {
    previousMillis = currentMillis;
    buildTxString();          // update string
    Serial.println(txData);
  }
}


void buildTxString()
{
  txData = "";

  txData += String(VR, 2) + "*";
  txData += String(VY, 2) + "*";
  txData += String(VB, 2) + "*";

  txData += String(PR, 2) + "*";
  txData += String(PY, 2) + "*";
  txData += String(PB, 2) + "*";

  txData += String(IR, 2) + "*";
  txData += String(IY, 2) + "*";
  txData += String(IB, 2) + "*%";
  txData += String(CT_data) + "$\n";
}

void requestEvent()
{
  uint8_t count = 0;

  while (txIndex < txData.length() && count < I2C_CHUNK)
  {
    Wire.write(txData[txIndex++]);
    count++;
  }

  if (txIndex >= txData.length())
  {
    txIndex = 0;   // reset for next request
  }
}


