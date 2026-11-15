#include <Arduino.h>

const int muxs0 = 7;
const int muxs1 = 8;
const int muxs2 = 9;
const int muxs3 = 10;
const int muxen = 11; // MUX ENABLE
const int CTmonitor = A0;
const int samples = 200;
const int ADC_RESOLUTION = 1023; // use 1023 for Arduino UNO ADC
const int Treshold = 3;
// Calibration coefficients derived from your measurements:
// amps = slope * ADC + intercept
const float CAL_SLOPE = 0.02496488;     // slope
const float CAL_INTERCEPT = 0.37211691; // intercept

// Channel mapping with equipment names
const char* channelLabels[16] = {
  "Air Blower 1 - Phase R",   // CH0
  "Air Blower 1 - Phase Y",   // CH1
  "Air Blower 1 - Phase B",   // CH2
  "Air Blower 2 - Phase R",   // CH3
  "Air Blower 2 - Phase Y",   // CH4
  "Air Blower 2 - Phase B",   // CH5
  "Raw Water Pump 1",         // CH6
  "Raw Water Pump 2",         // CH7
  "Filter Feed Pump 1",       // CH8
  "Filter Feed Pump 2",       // CH9
  "Recirculation Pump",       // CH10
  "Unused",                   // CH11
  "Unused",                   // CH12
  "Unused",                   // CH13
  "Unused",                   // CH14
  "Unused"                    // CH15
};

// Separate string variables for each channel status
String status0, status1, status2, status3, status4, status5, status6, status7;
String status8, status9, status10, status11, status12, status13, status14, status15;

void setup() {
  Serial.begin(9600);

  pinMode(muxs0, OUTPUT);
  pinMode(muxs1, OUTPUT);
  pinMode(muxs2, OUTPUT);
  pinMode(muxs3, OUTPUT);
  pinMode(muxen, OUTPUT);

  digitalWrite(muxen, LOW); // Enable MUX (active LOW)
  delay(100);
}

// set channel using bitmask
void setChannel(int channel) {
  digitalWrite(muxs0, (channel & 0x01) ? HIGH : LOW);
  digitalWrite(muxs1, (channel & 0x02) ? HIGH : LOW);
  digitalWrite(muxs2, (channel & 0x04) ? HIGH : LOW);
  digitalWrite(muxs3, (channel & 0x08) ? HIGH : LOW);
  delay(5); // allow signal to settle
}

void loop() {
  for (int ch = 0; ch < 16; ch++) {
    setChannel(ch);

    long sum = 0;
    for (int j = 0; j < samples; j++) {
      int rawADC = analogRead(CTmonitor);
      sum += rawADC;
      delayMicroseconds(200);
    }

    float avgADC = (float)sum / samples;
    float amps = CAL_SLOPE * avgADC + CAL_INTERCEPT;

    // Separate if conditions and separate string storage
    if (ch == 0) {
      if (amps > Treshold) status0 = "ON"; else status0 = "OFF";
      Serial.print(channelLabels[ch]); Serial.print(" | Current: "); Serial.print(amps,3); Serial.print(" | Status: "); Serial.println(status0);
    }
    else if (ch == 1) {
      if (amps > Treshold) status1 = "ON"; else status1 = "OFF";
      Serial.print(channelLabels[ch]); Serial.print(" | Current: "); Serial.print(amps,3); Serial.print(" | Status: "); Serial.println(status1);
    }
    else if (ch == 2) {
      if (amps > Treshold) status2 = "ON"; else status2 = "OFF";
      Serial.print(channelLabels[ch]); Serial.print(" | Current: "); Serial.print(amps,3); Serial.print(" | Status: "); Serial.println(status2);
    }
    else if (ch == 3) {
      if (amps > Treshold) status3 = "ON"; else status3 = "OFF";
      Serial.print(channelLabels[ch]); Serial.print(" | Current: "); Serial.print(amps,3); Serial.print(" | Status: "); Serial.println(status3);
    }
    else if (ch == 4) {
      if (amps > Treshold) status4 = "ON"; else status4 = "OFF";
      Serial.print(channelLabels[ch]); Serial.print(" | Current: "); Serial.print(amps,3); Serial.print(" | Status: "); Serial.println(status4);
    }
    else if (ch == 5) {
      if (amps > Treshold) status5 = "ON"; else status5 = "OFF";
      Serial.print(channelLabels[ch]); Serial.print(" | Current: "); Serial.print(amps,3); Serial.print(" | Status: "); Serial.println(status5);
    }
    else if (ch == 6) {
      if (amps > Treshold) status6 = "ON"; else status6 = "OFF";
      Serial.print(channelLabels[ch]); Serial.print(" | Current: "); Serial.print(amps,3); Serial.print(" | Status: "); Serial.println(status6);
    }
    else if (ch == 7) {
      if (amps > Treshold) status7 = "ON"; else status7 = "OFF";
      Serial.print(channelLabels[ch]); Serial.print(" | Current: "); Serial.print(amps,3); Serial.print(" | Status: "); Serial.println(status7);// end
    }
    else if (ch == 8) {
      if (amps > Treshold) status8 = "ON"; else status8 = "OFF";
      Serial.print(channelLabels[ch]); Serial.print(" | Current: "); Serial.print(amps,3); Serial.print(" | Status: "); Serial.println(status8);
    }
    else if (ch == 9) {
      if (amps > Treshold) status9 = "ON"; else status9 = "OFF";
      Serial.print(channelLabels[ch]); Serial.print(" | Current: "); Serial.print(amps,3); Serial.print(" | Status: "); Serial.println(status9);
    }
    else if (ch == 10) {
      if (amps > Treshold) status10 = "ON"; else status10 = "OFF";
      Serial.print(channelLabels[ch]); Serial.print(" | Current: "); Serial.print(amps,3); Serial.print(" | Status: "); Serial.println(status10);
    }
    else if (ch == 11) {
      if (amps > Treshold) status11 = "ON"; else status11 = "OFF";
      Serial.print(channelLabels[ch]); Serial.print(" | Current: "); Serial.print(amps,3); Serial.print(" | Status: "); Serial.println(status11);
    }
    else if (ch == 12) {
      if (amps > Treshold) status12 = "ON"; else status12 = "OFF";
      Serial.print(channelLabels[ch]); Serial.print(" | Current: "); Serial.print(amps,3); Serial.print(" | Status: "); Serial.println(status12);
    }
    else if (ch == 13) {
      if (amps > Treshold) status13 = "ON"; else status13 = "OFF";
      Serial.print(channelLabels[ch]); Serial.print(" | Current: "); Serial.print(amps,3); Serial.print(" | Status: "); Serial.println(status13);
    }
    else if (ch == 14) {
      if (amps > Treshold) status14 = "ON"; else status14 = "OFF";
      Serial.print(channelLabels[ch]); Serial.print(" | Current: "); Serial.print(amps,3); Serial.print(" | Status: "); Serial.println(status14);
    }
    else if (ch == 15) {
      if (amps > Treshold) status15 = "ON"; else status15 = "OFF";
      Serial.print(channelLabels[ch]); Serial.print(" | Current: "); Serial.print(amps,3); Serial.print(" | Status: "); Serial.println(status15);
    }
  }

  Serial.println("-----------------------------");
  delay(1000);
}
