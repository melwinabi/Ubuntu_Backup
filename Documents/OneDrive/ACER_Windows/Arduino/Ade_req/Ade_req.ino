/* ADE7758 - R/Y/B display with:
   - P-P voltage average
   - Current average (RMS)
   - Power Factor (per-phase and overall)
   - Running load in kWh (persisted to EEPROM)
   - Total kW (instantaneous)
   
   Assumes ADE7758 library exposes getAVRMS/getAIRMS/getWatt/getVar/getVa/getFreq/getStatus etc.
*/

#include <BluetoothSerial.h>
#include <SPI.h>
#include <ADE7758.h>
#include <EEPROM.h>

BluetoothSerial SerialBT;

#define EEPROM_SIZE 128
#define EEPROM_ENERGY_ADDR  0    // store double energy_kwh at addr 0
#define EEPROM_CAL_ADDR     32   // start of calibration floats area
#define ADE_IRQ_PIN 4

ADE7758 EnergyMeter;

// Raw registers and measurement variables
uint32_t rawVR, rawVY, rawVB;
uint32_t rawIR, rawIY, rawIB;

float Volt_R = 0, Volt_Y = 0, Volt_B = 0;  // Vrms
float Curr_R = 0, Curr_Y = 0, Curr_B = 0;  // Arms
float VA_R = 0, VA_Y = 0, VA_B = 0;        // Apparent VA (after scaling)
float Watt_R = 0, Watt_Y = 0, Watt_B = 0;  // Active W (after scaling)
float VAR_R = 0, VAR_Y = 0, VAR_B = 0;    // Reactive VAR (after scaling)
float PF_R = 0, PF_Y = 0, PF_B = 0;

float Freq = 0;
uint8_t PhaseSequence = 0;

// --- Calibration constants (can be tuned) ---
float V_SCALE   = 4682.0f;      // Vrms register -> V
float I_SCALE   = 142600.0f;    // Arms register -> A
float WATT_SCALE = 0.0155f;     // raw watt -> W
float VA_SCALE   = 0.37f;       // raw VA -> VA
float VAR_SCALE  = 0.05f;       // raw var -> VAR

// Energy accumulation
double energy_kwh = 0.0;      // running load in kWh (persisted)
unsigned long lastEnergySaveMs = 0;
unsigned long lastLoopMs = 0;

// constants
const double VPP_FROM_VRMS = 2.8284271247461903; // 2*sqrt(2)
const unsigned long ENERGY_SAVE_INTERVAL_MS = 60UL * 1000UL; // save every 60s

// ---------- EEPROM helpers ----------
void saveEnergyToEEPROM() {
  EEPROM.put(EEPROM_ENERGY_ADDR, energy_kwh);
  EEPROM.commit();
}

void loadEnergyFromEEPROM() {
  EEPROM.get(EEPROM_ENERGY_ADDR, energy_kwh);
  if (isnan(energy_kwh) || isinf(energy_kwh) || energy_kwh < 0.0) energy_kwh = 0.0;
}

// Save calibration floats (optional)
void saveCalibrationToEEPROM() {
  int addr = EEPROM_CAL_ADDR;
  EEPROM.put(addr, V_SCALE); addr += sizeof(V_SCALE);
  EEPROM.put(addr, I_SCALE); addr += sizeof(I_SCALE);
  EEPROM.put(addr, WATT_SCALE); addr += sizeof(WATT_SCALE);
  EEPROM.put(addr, VA_SCALE); addr += sizeof(VA_SCALE);
  EEPROM.put(addr, VAR_SCALE); addr += sizeof(VAR_SCALE);
  EEPROM.commit();
}

void loadCalibrationFromEEPROM() {
  int addr = EEPROM_CAL_ADDR;
  EEPROM.get(addr, V_SCALE); addr += sizeof(V_SCALE);
  EEPROM.get(addr, I_SCALE); addr += sizeof(I_SCALE);
  EEPROM.get(addr, WATT_SCALE); addr += sizeof(WATT_SCALE);
  EEPROM.get(addr, VA_SCALE); addr += sizeof(VA_SCALE);
  EEPROM.get(addr, VAR_SCALE); addr += sizeof(VAR_SCALE);

  // fallbacks if corrupted/invalid
  if (isnan(V_SCALE) || V_SCALE < 1.0) V_SCALE = 4682.0f;
  if (isnan(I_SCALE) || I_SCALE < 1.0) I_SCALE = 142600.0f;
  if (isnan(WATT_SCALE) || WATT_SCALE <= 0.0f) WATT_SCALE = 0.0155f;
  if (isnan(VA_SCALE) || VA_SCALE <= 0.0f) VA_SCALE = 0.37f;
  if (isnan(VAR_SCALE) || VAR_SCALE <= 0.0f) VAR_SCALE = 0.05f;
}

// ---------- ADE7758 setup ----------
void Calibrate_ADE7758() {
  EnergyMeter.setOpMode(0x04); // continuous conversion mode
  EnergyMeter.gainSetup(INTEGRATOR_OFF, FULLSCALESELECT_0_5V, GAIN_1, GAIN_4);
  EnergyMeter.setupDivs(0x800, 0x800, 0x800);
  EnergyMeter.setLcycMode(0x0F);
  EnergyMeter.setLineCyc(20);
  EnergyMeter.setMaskInterrupts(ZXTOA | ZXTOB | ZXTOC | LENERGY | ZXA | ZXB | ZXC);
  EnergyMeter.setCompMode(0x24);
  EnergyMeter.setAVoltageOffset(-210);
  EnergyMeter.setACurrentOffset(-26);
  EnergyMeter.setAWattOffset(-10);
}

// ---------- Read & compute ----------
void Read_ADE7758_andCompute() {
  unsigned long now = millis();
  double dt_seconds = (lastLoopMs == 0) ? 0.0 : (now - lastLoopMs) / 1000.0;
  lastLoopMs = now;

  uint32_t status = EnergyMeter.getStatus();

  // Raw registers
  rawVR = EnergyMeter.getAVRMS();
  rawIR = EnergyMeter.getAIRMS();
  rawVY = EnergyMeter.getBVRMS();
  rawIY = EnergyMeter.getBIRMS();
  rawVB = EnergyMeter.getCVRMS();
  rawIB = EnergyMeter.getCIRMS();

  // Raw power registers
  float rawW_R = EnergyMeter.getWatt(PHASE_A);
  float rawW_Y = EnergyMeter.getWatt(PHASE_B);
  float rawW_B = EnergyMeter.getWatt(PHASE_C);

  float rawVar_R = EnergyMeter.getVar(PHASE_A);
  float rawVar_Y = EnergyMeter.getVar(PHASE_B);
  float rawVar_B = EnergyMeter.getVar(PHASE_C);

  float rawVa_R = EnergyMeter.getVa(PHASE_A);
  float rawVa_Y = EnergyMeter.getVa(PHASE_B);
  float rawVa_B = EnergyMeter.getVa(PHASE_C);

  // Frequency
  Freq = EnergyMeter.getFreq() / 16.0f; // library/raw -> Hz approx

  // Phase sequence (fallback)
  PhaseSequence = (status & 0x01) ? 1 : 0;

  // Apply scaling
  Volt_R = (float)rawVR / V_SCALE;
  Volt_Y = (float)rawVY / V_SCALE;
  Volt_B = (float)rawVB / V_SCALE;

  Curr_R = (float)rawIR / I_SCALE;
  Curr_Y = (float)rawIY / I_SCALE;
  Curr_B = (float)rawIB / I_SCALE;

  Watt_R = rawW_R * WATT_SCALE;
  Watt_Y = rawW_Y * WATT_SCALE;
  Watt_B = rawW_B * WATT_SCALE;

  VAR_R = rawVar_R * VAR_SCALE;
  VAR_Y = rawVar_Y * VAR_SCALE;
  VAR_B = rawVar_B * VAR_SCALE;

  VA_R = rawVa_R * VA_SCALE;
  VA_Y = rawVa_Y * VA_SCALE;
  VA_B = rawVa_B * VA_SCALE;

  // Per-phase PF safe calc
  PF_R = (VA_R > 0.0001f) ? (Watt_R / VA_R) : 0.0f;
  PF_Y = (VA_Y > 0.0001f) ? (Watt_Y / VA_Y) : 0.0f;
  PF_B = (VA_B > 0.0001f) ? (Watt_B / VA_B) : 0.0f;

  // Instant totals
  float totalWatt = Watt_R + Watt_Y + Watt_B;        // W
  float totalVA = VA_R + VA_Y + VA_B;                // VA (apparent)
  float totalVAR = VAR_R + VAR_Y + VAR_B;            // VAR

  // Overall PF (weighted by apparent power)
  float overallPF = (totalVA > 0.0001f) ? (totalWatt / totalVA) : 0.0f;

  // Total kW instantaneous
  float totalKW = totalWatt / 1000.0f; // kW

  // Energy integration (kWh)
  if (dt_seconds > 0.0) {
    // kWh increment = totalWatt (W) * dt_seconds / 3600 / 1000 = totalWatt * dt_seconds / 3,600,000
    double increment_kwh = (double)totalWatt * (double)dt_seconds / 3600000.0;
    energy_kwh += increment_kwh;
  }

  // Periodically persist energy to EEPROM
  if (millis() - lastEnergySaveMs >= ENERGY_SAVE_INTERVAL_MS) {
    lastEnergySaveMs = millis();
    saveEnergyToEEPROM();
  }

  // --- Compute requested derived values to print ---
  // P-P voltage per phase (estimate assuming sinusoid)
  float Vpp_R = Volt_R * (float)VPP_FROM_VRMS;
  float Vpp_Y = Volt_Y * (float)VPP_FROM_VRMS;
  float Vpp_B = Volt_B * (float)VPP_FROM_VRMS;

  // P-P voltage average across three phases
  float Vpp_avg = (Vpp_R + Vpp_Y + Vpp_B) / 3.0f;

  // Current average (RMS average)
  float Iavg = (Curr_R + Curr_Y + Curr_B) / 3.0f;

  // Print results (Serial + Bluetooth)
  char seqStr[16];
  if (PhaseSequence == 0) snprintf(seqStr, sizeof(seqStr), "R-Y-B");
  else snprintf(seqStr, sizeof(seqStr), "R-B-Y");

  // Print to Serial and Bluetooth
  Serial.printf("=== ADE7758 Readings ===\n");
  Serial.printf("Vrms: R=%.2f V  Y=%.2f V  B=%.2f V\n", Volt_R, Volt_Y, Volt_B);
  Serial.printf("Vpp (est): R=%.2f V  Y=%.2f V  B=%.2f V  | Vpp_avg=%.2f V\n", Vpp_R, Vpp_Y, Vpp_B, Vpp_avg);
  Serial.printf("Irms: R=%.2f A  Y=%.2f A  B=%.2f A  | Iavg=%.2f A\n", Curr_R, Curr_Y, Curr_B, Iavg);
  Serial.printf("W:   R=%.2f W  Y=%.2f W  B=%.2f W  | Total W=%.2f W\n", Watt_R, Watt_Y, Watt_B, (Watt_R+Watt_Y+Watt_B));
  Serial.printf("VAR: R=%.2f VAR Y=%.2f VAR B=%.2f VAR  | Total VAR=%.2f\n", VAR_R, VAR_Y, VAR_B, (VAR_R+VAR_Y+VAR_B));
  Serial.printf("VA:  R=%.2f VA  Y=%.2f VA  B=%.2f VA  | Total VA=%.2f\n", VA_R, VA_Y, VA_B, (VA_R+VA_Y+VA_B));
  Serial.printf("PF:  R=%.2f  Y=%.2f  B=%.2f  | Overall PF(weighted)=%.3f\n", PF_R, PF_Y, PF_B, overallPF);
  Serial.printf("Total kW (instant) = %.3f kW\n", totalKW);
  Serial.printf("Running energy = %.6f kWh\n", energy_kwh);
  Serial.printf("Frequency: %.2f Hz  | Phase sequence: %s\n", Freq, seqStr);
  Serial.printf("-------------------------------\n\n");

  SerialBT.printf("Vpp_avg=%.2f V, Iavg=%.2f A, PF=%.3f, Total_kW=%.3f kW, Energy=%.6f kWh\n",
                  Vpp_avg, Iavg, overallPF, totalKW, energy_kwh);
}

// ---------- setup & loop ----------
void setup() {
  Serial.begin(115200);
  SerialBT.begin("ESP32-METER");
  delay(200);
  EEPROM.begin(EEPROM_SIZE);
  loadEnergyFromEEPROM();      // load persisted energy
  loadCalibrationFromEEPROM(); // optional calibration load (if you want)
  if (EnergyMeter.Init()) {
    Serial.println("ADE7758 Initialized");
  } else {
    Serial.println("ADE7758 Init FAILED!");
  }
  Calibrate_ADE7758();
  lastLoopMs = millis();
  lastEnergySaveMs = millis();
}

void loop() {
  Read_ADE7758_andCompute();

  // Optionally accept simple serial commands: "ENERGYRESET" resets energy, "SAVECAL" saves calibration, etc.
  if (Serial.available()) {
    String s = Serial.readStringUntil('\n'); s.trim();
    if (s.equalsIgnoreCase("ENERGYRESET")) {
      energy_kwh = 0.0;
      saveEnergyToEEPROM();
      Serial.println("Energy reset to 0 and saved to EEPROM.");
    } else if (s.equalsIgnoreCase("SAVECAL")) {
      saveCalibrationToEEPROM();
      Serial.println("Calibration saved to EEPROM.");
    } else if (s.startsWith("CAL V=")) {
      float v = s.substring(6).toFloat();
      if (v > 0 && Volt_R > 1.0) {
        V_SCALE = rawVR / v;
        saveCalibrationToEEPROM();
        Serial.printf("V_SCALE updated=%.2f\n", V_SCALE);
      }
    } else if (s.startsWith("CAL I=")) {
      float i = s.substring(6).toFloat();
      if (i > 0 && Curr_R > 0.1) {
        I_SCALE = rawIR / i;
        saveCalibrationToEEPROM();
        Serial.printf("I_SCALE updated=%.2f\n", I_SCALE);
      }
    } else if (s.equalsIgnoreCase("SHOWCAL")) {
      Serial.printf("V_SCALE=%.2f I_SCALE=%.2f WATT_SCALE=%.6f VA_SCALE=%.3f VAR_SCALE=%.3f\n",
                    V_SCALE, I_SCALE, WATT_SCALE, VA_SCALE, VAR_SCALE);
    }
  }

  // small delay to avoid tight loop
  delay(500);
}
