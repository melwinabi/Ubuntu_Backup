/*
 * BLE Tag - Optimized for 3D Positioning Accuracy
 *
 * Optimizations vs basic version:
 *  1. Multi-scan accumulation (collects RSSI for multiple seconds before computing)
 *  2. Outlier rejection (drops sudden RSSI spikes)
 *  3. Median filter (more robust than mean for noisy data)
 *  4. Position smoothing across consecutive estimates
 *  5. Z-axis clamping (prevents nonsense values)
 *  6. Per-anchor calibration support
 *
 * Board: ESP32-S3
 *
 * BEFORE FLASHING:
 *   1. Calibrate MEASURED_POWER per anchor (see CALIBRATION GUIDE in code below)
 *   2. Set anchor coordinates to match physical placement
 *   3. Tune N_PATH_LOSS for your room
 */

#include <Arduino.h>
#include <WiFi.h>
#include <PubSubClient.h>
#include <BLEDevice.h>
#include <BLEScan.h>
#include <BLEAdvertisedDevice.h>
#include <math.h>

// ========== CONFIG ==========
#define TAG_ID            "T1"

// Anchor coordinates (x, y, z) in METERS - measure carefully!
struct AnchorPos {
  const char* name;
  float x, y, z;
  float measuredPower;  // RSSI at 1m for THIS specific anchor (calibrate)
};

// CALIBRATION GUIDE:
//   1. Place tag exactly 1.0m from the anchor
//   2. Watch serial monitor for ~30 seconds
//   3. Average the RSSI values shown for that anchor
//   4. Update measuredPower below for that anchor
//   5. Repeat for each anchor (each can be slightly different)
AnchorPos anchors[4] = {
  {"TUNNEL-ANC-A1", 0.0, 0.0, 0.0,  -65.0},
  {"TUNNEL-ANC-A2", 1.0, 0.0, 0.0,  -65.0},   // 1m east
  {"TUNNEL-ANC-A3", 0.5, 1.0, 0.0,  -65.0},   // 1m north
  {"TUNNEL-ANC-A4", 0.5, 0.5, 1.0,  -65.0},   // 1m up
};

// Path loss exponent: tune for your room
//   2.0 = open space
//   2.5 = typical office
//   3.0 = walls/obstacles
//   3.5 = heavy obstruction
#define N_PATH_LOSS       2.5

// Z-axis clamping (set realistic floor/ceiling)
#define Z_MIN             0.0    // floor
#define Z_MAX             3.0    // ceiling

// Multi-scan accumulation: collect samples over multiple BLE scans
// More samples = smoother but slower response
#define SCAN_DURATION     1     // seconds per scan
#define SCANS_PER_PUBLISH 3     // accumulate 3 scans before publishing

// Smoothing across position estimates
#define POSITION_SMOOTH   0.4   // 0=no smoothing, 1=fully smooth

// WiFi
const char* WIFI_SSID     = "OnePlus 11 5G";
const char* WIFI_PASSWORD = "b736szq7";

// MQTT
const char* MQTT_BROKER   = "10.218.131.74";
const int   MQTT_PORT     = 1883;
const char* MQTT_TOPIC    = "uwb/distance";
const char* MQTT_CLIENT   = "ble_tag_01";
const char* MQTT_USERNAME = "GowthamS";
const char* MQTT_PASSWORD = "GowthamS4516";
// ============================

WiFiClient   wifiClient;
PubSubClient mqttClient(wifiClient);
BLEScan*     pBLEScan;

// RSSI samples collected across multiple scans, per anchor
#define MAX_SAMPLES 30
int rssiSamples[4][MAX_SAMPLES];
int sampleCount[4] = {0, 0, 0, 0};

// Smoothed position (kept between iterations for smoothing)
float smoothX = 0, smoothY = 0, smoothZ = 0;
bool firstPosition = true;

uint32_t scanCount = 0;

// Insert sample, drop oldest if full
void addSample(int anchorIdx, int rssi) {
  if (sampleCount[anchorIdx] < MAX_SAMPLES) {
    rssiSamples[anchorIdx][sampleCount[anchorIdx]++] = rssi;
  } else {
    // Shift left and add at end
    for (int i = 0; i < MAX_SAMPLES - 1; i++) {
      rssiSamples[anchorIdx][i] = rssiSamples[anchorIdx][i + 1];
    }
    rssiSamples[anchorIdx][MAX_SAMPLES - 1] = rssi;
  }
}

// Median filter: more robust than mean against outliers
int medianRssi(int anchorIdx) {
  int n = sampleCount[anchorIdx];
  if (n == 0) return -999;
  int copy[MAX_SAMPLES];
  for (int i = 0; i < n; i++) copy[i] = rssiSamples[anchorIdx][i];
  // Simple bubble sort (n is small)
  for (int i = 0; i < n - 1; i++) {
    for (int j = 0; j < n - i - 1; j++) {
      if (copy[j] > copy[j + 1]) {
        int t = copy[j]; copy[j] = copy[j + 1]; copy[j + 1] = t;
      }
    }
  }
  return copy[n / 2];
}

// Average of middle samples after sorting (trim outliers)
float trimmedMeanRssi(int anchorIdx) {
  int n = sampleCount[anchorIdx];
  if (n == 0) return -999;
  if (n < 4) return medianRssi(anchorIdx);
  
  int copy[MAX_SAMPLES];
  for (int i = 0; i < n; i++) copy[i] = rssiSamples[anchorIdx][i];
  for (int i = 0; i < n - 1; i++) {
    for (int j = 0; j < n - i - 1; j++) {
      if (copy[j] > copy[j + 1]) {
        int t = copy[j]; copy[j] = copy[j + 1]; copy[j + 1] = t;
      }
    }
  }
  // Drop top 25% and bottom 25%, average the middle 50%
  int start = n / 4;
  int end = n - n / 4;
  float sum = 0;
  int cnt = 0;
  for (int i = start; i < end; i++) { sum += copy[i]; cnt++; }
  return sum / cnt;
}

void connectWiFi() {
  Serial.print("Connecting to WiFi: ");
  Serial.println(WIFI_SSID);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  int attempts = 0;
  while (WiFi.status() != WL_CONNECTED && attempts < 20) {
    delay(500); Serial.print("."); attempts++;
  }
  if (WiFi.status() == WL_CONNECTED) {
    Serial.println("\nWiFi connected!");
    Serial.print("IP: "); Serial.println(WiFi.localIP());
  } else {
    Serial.println("\nWiFi failed");
  }
}

void connectMQTT() {
  if (WiFi.status() != WL_CONNECTED) return;
  mqttClient.setServer(MQTT_BROKER, MQTT_PORT);
  int attempts = 0;
  while (!mqttClient.connected() && attempts < 5) {
    Serial.print("Connecting to MQTT...");
    if (mqttClient.connect(MQTT_CLIENT, MQTT_USERNAME, MQTT_PASSWORD)) {
      Serial.println("connected!");
    } else {
      Serial.print("failed rc="); Serial.println(mqttClient.state());
      delay(2000); attempts++;
    }
  }
}

float rssiToDistance(float rssi, float measuredPower) {
  float ratio = (measuredPower - rssi) / (10.0 * N_PATH_LOSS);
  return pow(10.0, ratio);
}

bool trilaterate3D(float d[4], float &outX, float &outY, float &outZ) {
  float x0 = anchors[0].x, y0 = anchors[0].y, z0 = anchors[0].z;
  float A[3][3], b[3];
  
  for (int i = 0; i < 3; i++) {
    int idx = i + 1;
    A[i][0] = 2 * (anchors[idx].x - x0);
    A[i][1] = 2 * (anchors[idx].y - y0);
    A[i][2] = 2 * (anchors[idx].z - z0);
    b[i] = (d[0]*d[0] - d[idx]*d[idx])
         + (anchors[idx].x*anchors[idx].x + anchors[idx].y*anchors[idx].y + anchors[idx].z*anchors[idx].z)
         - (x0*x0 + y0*y0 + z0*z0);
  }
  
  float det = A[0][0]*(A[1][1]*A[2][2] - A[1][2]*A[2][1])
            - A[0][1]*(A[1][0]*A[2][2] - A[1][2]*A[2][0])
            + A[0][2]*(A[1][0]*A[2][1] - A[1][1]*A[2][0]);
  
  if (fabs(det) < 0.0001) return false;
  
  float detX = b[0]*(A[1][1]*A[2][2] - A[1][2]*A[2][1])
             - A[0][1]*(b[1]*A[2][2] - A[1][2]*b[2])
             + A[0][2]*(b[1]*A[2][1] - A[1][1]*b[2]);
  
  float detY = A[0][0]*(b[1]*A[2][2] - A[1][2]*b[2])
             - b[0]*(A[1][0]*A[2][2] - A[1][2]*A[2][0])
             + A[0][2]*(A[1][0]*b[2] - b[1]*A[2][0]);
  
  float detZ = A[0][0]*(A[1][1]*b[2] - b[1]*A[2][1])
             - A[0][1]*(A[1][0]*b[2] - b[1]*A[2][0])
             + b[0]*(A[1][0]*A[2][1] - A[1][1]*A[2][0]);
  
  outX = detX / det;
  outY = detY / det;
  outZ = detZ / det;
  return true;
}

void publishData(float d[4], float rssi[4], float x, float y, float z) {
  if (WiFi.status() != WL_CONNECTED) connectWiFi();
  if (!mqttClient.connected()) connectMQTT();
  if (mqttClient.connected()) {
    char payload[400];
    float magnitude = sqrt(x*x + y*y + z*z);
    snprintf(payload, sizeof(payload),
             "{\"distance\": %.2f, \"x\": %.2f, \"y\": %.2f, \"z\": %.2f, "
             "\"d1\": %.2f, \"d2\": %.2f, \"d3\": %.2f, \"d4\": %.2f, "
             "\"rssi1\": %.0f, \"rssi2\": %.0f, \"rssi3\": %.0f, \"rssi4\": %.0f, "
             "\"tag\": \"%s\"}",
             magnitude, x, y, z,
             d[0], d[1], d[2], d[3],
             rssi[0], rssi[1], rssi[2], rssi[3],
             TAG_ID);
    mqttClient.publish(MQTT_TOPIC, payload);
    Serial.print("MQTT: ");
    Serial.println(payload);
  }
  mqttClient.loop();
}

void setup() {
  Serial.begin(115200);
  delay(500);

  Serial.println("========================================");
  Serial.print("  BLE Tag (3D Optimized): "); Serial.println(TAG_ID);
  Serial.println("  Anchor positions:");
  for (int i = 0; i < 4; i++) {
    Serial.print("    "); Serial.print(anchors[i].name);
    Serial.print(" at ("); Serial.print(anchors[i].x);
    Serial.print(", "); Serial.print(anchors[i].y);
    Serial.print(", "); Serial.print(anchors[i].z);
    Serial.print(") | 1m RSSI: "); Serial.println(anchors[i].measuredPower);
  }
  Serial.println("========================================");

  connectWiFi();
  connectMQTT();

  BLEDevice::init("");
  pBLEScan = BLEDevice::getScan();
  pBLEScan->setActiveScan(true);
  pBLEScan->setInterval(100);
  pBLEScan->setWindow(99);

  Serial.println("Scanning...");
}

void loop() {
  // One scan cycle
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

  // Compute and publish every SCANS_PER_PUBLISH scans
  if (scanCount >= SCANS_PER_PUBLISH) {
    scanCount = 0;

    // Need samples for all 4 anchors
    bool allHaveSamples = true;
    for (int a = 0; a < 4; a++) if (sampleCount[a] == 0) allHaveSamples = false;

    if (allHaveSamples) {
      // Compute filtered RSSI and distance
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
        Serial.print(rawZ); Serial.print(") smoothed=(");
        Serial.print(smoothX); Serial.print(", ");
        Serial.print(smoothY); Serial.print(", ");
        Serial.print(smoothZ); Serial.println(")");

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

    // Slowly forget old samples - keep last 50% for next iteration
    for (int a = 0; a < 4; a++) {
      int keep = sampleCount[a] / 2;
      if (keep > 0) {
        for (int i = 0; i < keep; i++) {
          rssiSamples[a][i] = rssiSamples[a][sampleCount[a] - keep + i];
        }
      }
      sampleCount[a] = keep;
    }
  }

  delay(50);
}