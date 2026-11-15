/*
 * BLE Anchor (high-power, optimized for accuracy)
 *
 * Set ANCHOR_ID and DEVICE_NAME below for each board:
 *   Anchor 1 -> A1 / TUNNEL-ANC-A1
 *   Anchor 2 -> A2 / TUNNEL-ANC-A2
 *   Anchor 3 -> A3 / TUNNEL-ANC-A3
 *   Anchor 4 -> A4 / TUNNEL-ANC-A4
 */

#include <Arduino.h>
#include <BLEDevice.h>
#include <BLEServer.h>
#include <BLEUtils.h>
#include <BLEAdvertising.h>

// CHANGE THESE per anchor before flashing each board
#define ANCHOR_ID   "A2"
#define DEVICE_NAME "TUNNEL-ANC-A2"

BLEAdvertising *pAdvertising;

void setup() {
  Serial.begin(115200);
  delay(500);

  Serial.println("========================================");
  Serial.print("  BLE Anchor: ");
  Serial.println(DEVICE_NAME);
  Serial.println("========================================");

  BLEDevice::init(DEVICE_NAME);

  // Maximum TX power for best range and stable RSSI
  BLEDevice::setPower(ESP_PWR_LVL_P9);  // +9 dBm

  BLEServer *pServer = BLEDevice::createServer();
  pAdvertising = BLEDevice::getAdvertising();

  BLEAdvertisementData advData;
  advData.setName(DEVICE_NAME);
  advData.setFlags(0x04);

  pAdvertising->setAdvertisementData(advData);
  pAdvertising->setScanResponseData(advData);

  // Fast advertising for higher tag scan rate
  pAdvertising->setMinInterval(0x20);   // 20ms
  pAdvertising->setMaxInterval(0x30);   // 30ms

  pAdvertising->start();

  Serial.println("Advertising at max power. Tags can detect this anchor.");
}

void loop() {
  static uint32_t lastPrint = 0;
  if (millis() - lastPrint > 5000) {
    Serial.print("[");
    Serial.print(ANCHOR_ID);
    Serial.println("] Broadcasting at +9 dBm...");
    lastPrint = millis();
  }
  delay(100);
}