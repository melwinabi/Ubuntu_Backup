#include <SPI.h>
#include <EEPROM.h>
#include "ADE7758.h"
#include "BluetoothSerial.h"

#define EEPROM_SIZE 64
#define ADE_IRQ_PIN 4

ADE7758 EnergyMeter;
BluetoothSerial bt;

// Scaling factors (your values)
float V_SCALE    = 4682.0f;
float I_SCALE    = 142600.0f;
float WATT_SCALE = 0.0155f;
float VAR_SCALE  = 0.05f;
float VA_SCALE   = 0.37f;

// Measurement variables
float Volt_R, Volt_Y, Volt_B;
float Curr_R, Curr_Y, Curr_B;
float Watt_R, Watt_Y, Watt_B;
float VAR_R,  VAR_Y,  VAR_B;
float VA_R,   VA_Y,   VA_B;

float PF_R, PF_Y, PF_B;
float Freq;
float PF_total, Vavg, Iavg;
int f = 0;
float Tot_kW, Tot_kVAR, Tot_kVA;
const char* System_ID = "100099";
// kWh accumulator
float Total_kWh = 0.0f;
unsigned long lastEnergyMillis = 0;
String Blt_Name = "";
String stored_data = "";
unsigned long previousMillis_TWO = 0;
const unsigned long interval_TWO = 60000; // 2 seconds

void setup() {
  Serial.begin(115200);
  EEPROM.begin(EEPROM_SIZE);
  Blt_Name = "Cenmark_";
  Blt_Name += System_ID; 
  delay(500);  
  bt.begin(Blt_Name);
  delay(1000);
  if (EnergyMeter.Init()) {
    Serial.println("ADE7758 Ready");
  } else {
    Serial.println("ADE7758 Init Failed!");
  }
  Calibrate_ADE7758();
}

void loop() {
  Read_ADE7758();
  unsigned long currentMillis_TWO = millis();
  if (currentMillis_TWO - previousMillis_TWO >= interval_TWO) {
    previousMillis_TWO = currentMillis_TWO;   
  }
  // ======= PHASE TO PHASE VOLTAGE =======
  float Vab = sqrt(3.0) * Volt_R;
  float Vbc = sqrt(3.0) * Volt_Y;
  float Vca = sqrt(3.0) * Volt_B;

  Vavg = (Vab + Vbc + Vca) / 3.0;
  Iavg = (Curr_R + Curr_Y + Curr_B) / 3.0;

  // ======= ENERGY ACCUMULATION =======
  unsigned long now = millis();
  if (now - lastEnergyMillis >= 1000) {  // every 1 second
    lastEnergyMillis = now;

    // kW to kWh: kWh += kW * (time hours)
    Total_kWh += Tot_kW * (1.0 / 3600.0);
    if (isnan(Total_kWh)) {
        Total_kWh = 0;       // reset to zero instead of showing -nan
    }
    // save to EEPROM every 10 sec
    static int cnt = 0;
    cnt++;
    if (cnt >= 10) {
      EEPROM.put(0, Total_kWh);
      EEPROM.commit();
      cnt = 0;
    }
  }

  // ======= PRINTING =======
bt.println("\n===== ADE7758 READINGS =====");

bt.print("Phase Voltage (V):   ");
bt.print("R="); bt.print(Volt_R, 2);
bt.print("  Y="); bt.print(Volt_Y, 2);
bt.print("  B="); bt.println(Volt_B, 2);

bt.print("Phase Current (A):   ");
bt.print("R="); bt.print(Curr_R, 3);
bt.print("  Y="); bt.print(Curr_Y, 3);
bt.print("  B="); bt.println(Curr_B, 3);

bt.print("Phase-Phase Voltage: ");
bt.print("Vab="); bt.print(Vab, 2);
bt.print("  Vbc="); bt.print(Vbc, 2);
bt.print("  Vca="); bt.println(Vca, 2);

bt.print("Average Voltage:     ");
bt.print(Vavg, 2);
bt.println(" V");

bt.print("Active Power (W):    ");
bt.print("R="); bt.print(Watt_R, 2);
bt.print("  Y="); bt.print(Watt_Y, 2);
bt.print("  B="); bt.println(Watt_B, 2);

bt.print("Reactive Power(VAR): ");
bt.print("R="); bt.print(VAR_R, 2);
bt.print("  Y="); bt.print(VAR_Y, 2);
bt.print("  B="); bt.println(VAR_B, 2);

bt.print("Apparent Power(VA):  ");
bt.print("R="); bt.print(VA_R, 2);
bt.print("  Y="); bt.print(VA_Y, 2);
bt.print("  B="); bt.println(VA_B, 2);

bt.print("PF per phase:        ");
bt.print("R="); bt.print(PF_R, 3);
bt.print("  Y="); bt.print(PF_Y, 3);
bt.print("  B="); bt.println(PF_B, 3);

bt.print("Total PF: ");
bt.println(PF_total, 3);

bt.print("Total kW: ");
bt.print(Tot_kW, 3);
bt.println(" kW");

bt.print("Total kVAR: ");
bt.print(Tot_kVAR, 3);
bt.println(" kVAR");

bt.print("Total kVA: ");
bt.print(Tot_kVA, 3);
bt.println(" kVA");

bt.print("Energy: ");
bt.print(Total_kWh, 4);
bt.println(" kWh");

bt.println("===============================\n");


  delay(2000);
}

// ==================== ADE CONFIG ====================
void Calibrate_ADE7758() {
  EnergyMeter.setOpMode(0x04);
  EnergyMeter.gainSetup(INTEGRATOR_OFF, FULLSCALESELECT_0_5V, GAIN_1, GAIN_4);
  EnergyMeter.setupDivs(0x800, 0x800, 0x800);
  EnergyMeter.setLcycMode(0x0F);
  EnergyMeter.setLineCyc(20);
  EnergyMeter.setMaskInterrupts(0xFFFFFFFF);
}

static inline float clampPF(float x) {
  if (x > 1.0f) return 1.0f;
  if (x < -1.0f) return -1.0f;
  return x;
}

// ==================== READ ADE ====================
void Read_ADE7758() {
  // Phase R
  Volt_R = EnergyMeter.getAVRMS() / V_SCALE;
  Curr_R = EnergyMeter.getAIRMS() / I_SCALE;
  Watt_R = EnergyMeter.getWatt(PHASE_A) * WATT_SCALE;
  VAR_R  = EnergyMeter.getVar(PHASE_A)  * VAR_SCALE;
  VA_R   = EnergyMeter.getVa(PHASE_A)   * VA_SCALE;

  // Phase Y
  Volt_Y = EnergyMeter.getBVRMS() / V_SCALE;
  Curr_Y = EnergyMeter.getBIRMS() / I_SCALE;
  Watt_Y = EnergyMeter.getWatt(PHASE_B) * WATT_SCALE;
  VAR_Y  = EnergyMeter.getVar(PHASE_B)  * VAR_SCALE;
  VA_Y   = EnergyMeter.getVa(PHASE_B)   * VA_SCALE;

  // Phase B
  Volt_B = EnergyMeter.getCVRMS() / V_SCALE;
  Curr_B = EnergyMeter.getCIRMS() / I_SCALE;
  Watt_B = EnergyMeter.getWatt(PHASE_C) * WATT_SCALE;
  VAR_B  = EnergyMeter.getVar(PHASE_C)  * VAR_SCALE;
  VA_B   = EnergyMeter.getVa(PHASE_C)   * VA_SCALE;

  // Frequency
  Freq = EnergyMeter.getFreq() / 16.0f;

  // PF per phase
  PF_R = (VA_R != 0) ? clampPF(Watt_R / VA_R) : 0;
  PF_Y = (VA_Y != 0) ? clampPF(Watt_Y / VA_Y) : 0;
  PF_B = (VA_B != 0) ? clampPF(Watt_B / VA_B) : 0;

  // Totals
  float Tot_W = Watt_R + Watt_Y + Watt_B;
  float Tot_VAR = VAR_R + VAR_Y + VAR_B;
  float Tot_VA = VA_R + VA_Y + VA_B;

  Tot_kW = Tot_W / 1000.0;
  Tot_kVAR = Tot_VAR / 1000.0;
  Tot_kVA = Tot_VA / 1000.0;

  PF_total = (Tot_VA != 0) ? clampPF(Tot_W / Tot_VA) : 0;
}

void btc_func()
{
  if (bt.available()) 
  {
    String Bt_data = bt.readStringUntil('\n');
    Bt_data.trim();

    if (Bt_data.startsWith("&&*"))
    {
      // Extract the string after "&&*"
      String S_PS = Bt_data.substring(3);
      S_PS.trim();

      int len = S_PS.length();
      if (len > 60) len = 60;

      // Store length at address 10
      EEPROM.write(10, len);

      // Store string starting at address 13
      for (int j = 0; j < len; j++) {
        EEPROM.write(13 + j, S_PS[j]);
      }

      // Null terminate
      EEPROM.write(13 + len, 0);

      EEPROM.commit();

      bt.println("SYSTEM RESTART");
      delay(1000);
      ESP.restart();
    }
  }
}
