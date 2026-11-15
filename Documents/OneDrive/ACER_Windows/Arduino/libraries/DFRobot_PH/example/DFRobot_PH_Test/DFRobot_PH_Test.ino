#include "DFRobot_PH.h"
#include <EEPROM.h>

#define PH_PIN 34

DFRobot_PH ph;

float voltage = 0;
float phValue = 0;
float temperature = 28.4;

// TIMING CONFIG
#define MAIN_INTERVAL     10000UL   // 10 seconds
#define SAMPLE_INTERVAL   5000UL    // 5 seconds
#define TOTAL_SAMPLES     6         // 30 seconds total

unsigned long mainTimer = 0;
unsigned long sampleTimer = 0;

bool samplingActive = false;
int sampleCount = 0;
float phSum = 0;

void setup()
{
    Serial.begin(115200);
    EEPROM.begin(64);
    ph.begin();
    Serial.println("System Ready. Waiting for measurement cycle...");
}

float readPHOnce()
{
    float totalADC = 0;

    for (int i = 0; i < 80; i++) {
        totalADC += analogRead(PH_PIN);
        delay(12);
    }

    voltage = (totalADC / 80.0) / 4095.0 * 3300.0;
    return ph.readPH(voltage, temperature);
}

void loop()
{
    unsigned long now = millis();

    /* ---------- START MEASUREMENT EVERY 10 SECONDS ---------- */
    if (!samplingActive && (now - mainTimer >= MAIN_INTERVAL)) {
        mainTimer = now;
        samplingActive = true;
        sampleCount = 0;
        phSum = 0;
        sampleTimer = now;

        Serial.println("\nStarting 30s pH sampling cycle...");
    }

    /* ---------- SAMPLE EVERY 5 SECONDS (6 TIMES) ---------- */
    if (samplingActive && (now - sampleTimer >= SAMPLE_INTERVAL)) {
        sampleTimer = now;

        phValue = readPHOnce();
        phSum += phValue;
        sampleCount++;

        Serial.print("Sample ");
        Serial.print(sampleCount);
        Serial.print(" | Voltage: ");
        Serial.print(voltage, 2);
        Serial.print(" mV | pH: ");
        Serial.println(phValue, 2);

        if (sampleCount >= TOTAL_SAMPLES) {
            float avgPH = phSum / TOTAL_SAMPLES;

            Serial.println("================================");
            Serial.print("AVERAGE pH (30s): ");
            Serial.println(avgPH, 2);
            Serial.println("================================");

            samplingActive = false;
        }
    }

    /* ---------- CALIBRATION SUPPORT ---------- */
    ph.calibration(voltage, temperature);
}
