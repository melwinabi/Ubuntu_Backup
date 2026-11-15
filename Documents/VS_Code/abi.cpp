void loop() {
  // 1 Second Scan
  BLEScanResults foundDevices = pBLEScan->start(SCAN_DURATION, false);

  // Track best RSSI per anchor seen in THIS scan
  int bestRssi[4] = {-999, -999, -999, -999};
  bool found[4] = {false, false, false, false};

  for (int i = 0; i < foundDevices.getCount(); i++) {
    BLEAdvertisedDevice device = foundDevices.getDevice(i);
    String name = device.getName().c_str();
    int rssi = device.getRSSI();

    for (int a = 0; a < 4; a++) {
      if (name == anchors[a].name) {
        if (rssi > bestRssi[a]) {
          bestRssi[a] = rssi;
          found[a] = true;
        }
      }
    }
  }

  // Add this scan's best RSSI to running sample buffer
  for (int a = 0; a < 4; a++) {
    if (found[a]) addSample(a, bestRssi[a]);
  }

  pBLEScan->clearResults();
  scanCount++;

  // Compute and publish every 5 Scans (5 Seconds)
  if (scanCount >= scanper) {
    scanCount = 0;

    // Need samples for all 4 anchors
    bool allHaveSamples = true;
    for (int a = 0; a < 4; a++) if (sampleCount[a] == 0) allHaveSamples = false;

    if (allHaveSamples) {
      // Compute filtered RSSI (Average) and distance
      float filteredRssi[4];
      float d[4];
      for (int a = 0; a < 4; a++) {
        filteredRssi[a] = trimmedMeanRssi(a);
        d[a] = rssiToDistance(filteredRssi[a], anchors[a].measuredPower);
      }

      float rawX, rawY, rawZ;
      if (trilaterate3D(d, rawX, rawY, rawZ)) {
        // Clamp z
        if (rawZ < Z_MIN) rawZ = Z_MIN;
        if (rawZ > Z_MAX) rawZ = Z_MAX;

        // Smooth position over time
        if (firstPosition) {
          smoothX = rawX; smoothY = rawY; smoothZ = rawZ;
          firstPosition = false;
        } else {
          smoothX = POSITION_SMOOTH * smoothX + (1 - POSITION_SMOOTH) * rawX;
          smoothY = POSITION_SMOOTH * smoothY + (1 - POSITION_SMOOTH) * rawY;
          smoothZ = POSITION_SMOOTH * smoothZ + (1 - POSITION_SMOOTH) * rawZ;
        }

        Serial.print("Filtered RSSI: ");
        for (int a = 0; a < 4; a++) {
          Serial.print(filteredRssi[a]); Serial.print(" ");
        }
        Serial.println();
        Serial.print("Distances: ");
        for (int a = 0; a < 4; a++) {
          Serial.print(d[a]); Serial.print("m ");
        }
        Serial.println();
        Serial.print("Position raw=(");
        Serial.print(rawX); Serial.print(", ");
        Serial.print(rawY); Serial.print(", ");
        Serial.print(rawZ); Serial.println(")");

        // Publish to MQTT
        publishData(d, filteredRssi, smoothX, smoothY, smoothZ);
      } else {
        Serial.println("Trilateration failed - check anchor placement");
      }
    } else {
      Serial.print("Missing samples for: ");
      for (int a = 0; a < 4; a++) {
        if (sampleCount[a] == 0) {
          Serial.print(anchors[a].name); Serial.print(" ");
        }
      }
      Serial.println();
    }

    // COMPLETELY CLEAR SAMPLES FOR THE NEXT 5 SECONDS BLOCK
    for (int a = 0; a < 4; a++) {
      sampleCount[a] = 0;
    }
  }

  delay(50);
}
