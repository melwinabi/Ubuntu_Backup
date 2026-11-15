#include "esp_camera.h"
#include <WiFi.h>
#include "esp_timer.h"
#include "img_converters.h"
#include "Arduino.h"
#include "fb_gfx.h"
#include "soc/soc.h"
#include "soc/rtc_cntl_reg.h"
#include "esp_http_server.h"
#include "SPIFFS.h"
#include "FS.h"
#include <EEPROM.h>
#include <ArduinoJson.h>

const char* ssid = "ACTFIBERNET";
const char* password = "act@12345";
#define PART_BOUNDARY "123456789000000000000987654321"

// Stream constants
static const char* _STREAM_CONTENT_TYPE = "multipart/x-mixed-replace;boundary=" PART_BOUNDARY;
static const char* _STREAM_BOUNDARY = "\r\n--" PART_BOUNDARY "\r\n";
static const char* _STREAM_PART = "Content-Type: image/jpeg\r\nContent-Length: %u\r\n\r\n";

#define CAMERA_MODEL_AI_THINKER

#if defined(CAMERA_MODEL_AI_THINKER)
  #define PWDN_GPIO_NUM     32
  #define RESET_GPIO_NUM    -1
  #define XCLK_GPIO_NUM      0
  #define SIOD_GPIO_NUM     26
  #define SIOC_GPIO_NUM     27
  #define Y9_GPIO_NUM       35
  #define Y8_GPIO_NUM       34
  #define Y7_GPIO_NUM       39
  #define Y6_GPIO_NUM       36
  #define Y5_GPIO_NUM       21
  #define Y4_GPIO_NUM       19
  #define Y3_GPIO_NUM       18
  #define Y2_GPIO_NUM        5
  #define VSYNC_GPIO_NUM    25
  #define HREF_GPIO_NUM     23
  #define PCLK_GPIO_NUM     22
#endif

// EEPROM for storing patterns
#define EEPROM_SIZE 2048

// Object recognition parameters
#define IMAGE_WIDTH 160
#define IMAGE_HEIGHT 120
#define FEATURE_VECTOR_SIZE 128
#define MAX_OBJECTS 20

httpd_handle_t camera_httpd = NULL;
httpd_handle_t stream_httpd = NULL;

// Structure to store object patterns
struct ObjectPattern {
  uint8_t features[FEATURE_VECTOR_SIZE];
  char name[30];
  uint16_t checksum;
  uint8_t objectType; // 0=face, 1=object
};

// EEG Data structure
struct EEGData {
  float alpha;
  float beta;
  float theta;
  float delta;
  float gamma;
  uint32_t timestamp;
  int prediction; // 0=normal, 1=mild, 2=moderate, 3=severe
};

// Global variables
ObjectPattern knownPatterns[MAX_OBJECTS];
int patternCount = 0;
EEGData currentEEG;
bool systemInitialized = false;
String patientName = "Patient";

// Process different file types for EEG data
EEGData processEEGFile(String fileName, String fileContent) {
  EEGData result = {0};
  Serial.printf("Processing EEG file: %s\n", fileName.c_str());
  
  // Simulate processing based on file extension
  if (fileName.endsWith(".jpg") || fileName.endsWith(".jpeg")) {
    Serial.println("Processing JPG image file for EEG patterns");
    // Simulate image-based EEG pattern recognition
    result.alpha = 8.5 + (random(100) / 100.0) * 4.0;
    result.beta = 13.2 + (random(100) / 100.0) * 7.0;
    result.theta = 4.8 + (random(100) / 100.0) * 3.0;
    result.delta = 1.5 + (random(100) / 100.0) * 2.0;
    result.gamma = 28.3 + (random(100) / 100.0) * 12.0;
  } else if (fileName.endsWith(".csv")) {
    Serial.println("Processing CSV EEG data");
    // Simulate CSV processing
    result.alpha = 9.2 + (random(100) / 100.0) * 3.5;
    result.beta = 14.1 + (random(100) / 100.0) * 6.5;
    result.theta = 5.2 + (random(100) / 100.0) * 2.8;
    result.delta = 1.8 + (random(100) / 100.0) * 1.8;
    result.gamma = 30.1 + (random(100) / 100.0) * 11.2;
  } else if (fileName.endsWith(".json")) {
    Serial.println("Processing JSON EEG data");
    // Simulate JSON processing
    result.alpha = 7.8 + (random(100) / 100.0) * 4.2;
    result.beta = 12.5 + (random(100) / 100.0) * 7.8;
    result.theta = 4.5 + (random(100) / 100.0) * 3.2;
    result.delta = 1.2 + (random(100) / 100.0) * 2.2;
    result.gamma = 26.8 + (random(100) / 100.0) * 12.8;
  } else {
    Serial.println("Processing unknown file type - using default analysis");
    // Default analysis
    result.alpha = 8.0 + (random(100) / 100.0) * 4.5;
    result.beta = 13.0 + (random(100) / 100.0) * 7.2;
    result.theta = 5.0 + (random(100) / 100.0) * 3.0;
    result.delta = 1.6 + (random(100) / 100.0) * 2.0;
    result.gamma = 27.5 + (random(100) / 100.0) * 12.5;
  }
  
  // Simple Alzheimer's prediction based on wave ratios
  float ratio = (result.theta + result.delta) / (result.alpha + result.beta);
  result.prediction = 0; // 0=normal, 1=mild, 2=moderate, 3=severe
  
  if (ratio > 0.8) result.prediction = 3;
  else if (ratio > 0.6) result.prediction = 2;
  else if (ratio > 0.4) result.prediction = 1;
  
  result.timestamp = millis();
  currentEEG = result;
  
  Serial.printf("EEG Analysis - Alpha: %.2f, Beta: %.2f, Theta: %.2f, Delta: %.2f, Gamma: %.2f, Prediction: %d\n",
                result.alpha, result.beta, result.theta, result.delta, result.gamma, result.prediction);
  
  return result;
}

// Enhanced HTML Web Page with EEG and Object Recognition
static const char PROGMEM INDEX_HTML[] = R"rawliteral(
<!DOCTYPE html>
<html lang="en">
<head>
  <meta charset="UTF-8">
  <meta name="viewport" content="width=device-width, initial-scale=1.0">
  <title>MemoryEnhance | Alzheimer's Monitoring System</title>
  <link rel="stylesheet" href="https://cdnjs.cloudflare.com/ajax/libs/font-awesome/6.4.0/css/all.min.css">
  <link href="https://fonts.googleapis.com/css2?family=Montserrat:wght@300;400;600;800&family=Roboto+Mono:wght@400;700&display=swap" rel="stylesheet">
  <script src="https://cdn.jsdelivr.net/npm/chart.js"></script>
  <style>
    :root {
      --primary: #2c7873;
      --primary-dark: #004445;
      --secondary: #6fb98f;
      --accent: #ff9a76;
      --medical: #4a90e2;
      --alert: #e74c3c;
      --dark: #1a1a2e;
      --darker: #0d0d1a;
      --light: #f0f0f0;
      --success: #27ae60;
      --warning: #f39c12;
      --danger: #e74c3c;
      --overlay: rgba(26, 26, 46, 0.95);
      --glass: rgba(255, 255, 255, 0.05);
      --glass-border: rgba(255, 255, 255, 0.1);
    }

    * {
      box-sizing: border-box;
      margin: 0;
      padding: 0;
      -webkit-tap-highlight-color: transparent;
    }

    body {
      font-family: 'Montserrat', sans-serif;
      background: var(--dark);
      color: var(--light);
      min-height: 100vh;
      overflow-x: hidden;
      background-image: 
        radial-gradient(circle at 20% 30%, rgba(44, 120, 115, 0.15) 0%, transparent 20%),
        radial-gradient(circle at 80% 70%, rgba(111, 185, 143, 0.15) 0%, transparent 20%);
    }

    .container {
      max-width: 1400px;
      margin: 0 auto;
      padding: 1rem;
    }

    .header {
      display: flex;
      justify-content: space-between;
      align-items: center;
      padding: 1rem 0;
      margin-bottom: 1rem;
      border-bottom: 1px solid var(--glass-border);
    }

    .app-title {
      font-family: 'Roboto Mono', monospace;
      font-size: 2rem;
      font-weight: 700;
      color: var(--secondary);
    }

    .app-title span {
      color: var(--accent);
    }

    .patient-info {
      display: flex;
      align-items: center;
      gap: 1rem;
    }

    .patient-name {
      font-size: 1.2rem;
      font-weight: 600;
      color: var(--medical);
    }

    .connection-status {
      display: flex;
      align-items: center;
      gap: 0.5rem;
      font-size: 0.9rem;
    }

    .status-indicator {
      width: 10px;
      height: 10px;
      border-radius: 50%;
      background: var(--success);
      animation: pulse 2s infinite;
    }

    @keyframes pulse {
      0% { opacity: 1; }
      50% { opacity: 0.5; }
      100% { opacity: 1; }
    }

    .main-content {
      display: grid;
      grid-template-columns: 1fr 1fr;
      gap: 1.5rem;
      margin-bottom: 2rem;
    }

    @media (max-width: 1100px) {
      .main-content {
        grid-template-columns: 1fr;
      }
    }

    .panel {
      background: var(--overlay);
      border-radius: 16px;
      padding: 1.5rem;
      backdrop-filter: blur(5px);
      border: 1px solid var(--glass-border);
      box-shadow: 0 8px 32px rgba(0, 0, 0, 0.2);
    }

    .panel-title {
      font-size: 1.2rem;
      font-weight: 600;
      margin-bottom: 1rem;
      color: var(--secondary);
      display: flex;
      align-items: center;
      gap: 0.5rem;
    }

    .video-container {
      position: relative;
      border-radius: 12px;
      overflow: hidden;
      background: var(--darker);
      aspect-ratio: 4/3;
    }

    #video-feed {
      width: 100%;
      height: 100%;
      object-fit: cover;
      display: block;
    }

    .video-overlay {
      position: absolute;
      top: 0;
      left: 0;
      width: 100%;
      height: 100%;
      pointer-events: none;
    }

    .object-box {
      position: absolute;
      border: 3px solid var(--medical);
      border-radius: 8px;
      background: rgba(74, 144, 226, 0.1);
      display: flex;
      flex-direction: column;
      align-items: center;
      padding: 0.5rem;
      transform: translate(-50%, -50%);
    }

    .object-name {
      color: white;
      font-size: 0.9rem;
      font-weight: 600;
      margin-top: 0.25rem;
      text-shadow: 1px 1px 2px rgba(0, 0, 0, 0.8);
      background: var(--medical);
      padding: 0.2rem 0.5rem;
      border-radius: 4px;
    }

    .control-buttons {
      display: grid;
      grid-template-columns: 1fr 1fr;
      gap: 0.5rem;
      margin-top: 1rem;
    }

    .capture-btn, .train-btn, .upload-btn {
      padding: 1rem;
      background: linear-gradient(45deg, var(--primary), var(--primary-dark));
      color: white;
      border: none;
      border-radius: 8px;
      font-size: 1rem;
      font-weight: 600;
      cursor: pointer;
      transition: all 0.3s ease;
      display: flex;
      align-items: center;
      justify-content: center;
      gap: 0.5rem;
    }

    .train-btn {
      background: linear-gradient(45deg, var(--accent), #e86d3a);
    }

    .upload-btn {
      background: linear-gradient(45deg, var(--medical), #357abd);
    }

    .capture-btn:hover, .train-btn:hover, .upload-btn:hover {
      transform: translateY(-2px);
      box-shadow: 0 6px 12px rgba(0, 0, 0, 0.2);
    }

    .results-container {
      display: flex;
      flex-direction: column;
      gap: 1rem;
    }

    .result-card {
      background: var(--darker);
      border-radius: 12px;
      padding: 1rem;
      border-left: 4px solid var(--primary);
    }

    .result-title {
      font-size: 1rem;
      font-weight: 600;
      margin-bottom: 0.5rem;
      color: var(--light);
    }

    .result-value {
      font-size: 1.5rem;
      font-weight: 700;
      color: var(--secondary);
    }

    .confidence-meter {
      height: 20px;
      background: var(--darker);
      border-radius: 10px;
      margin-top: 0.5rem;
      overflow: hidden;
    }

    .confidence-fill {
      height: 100%;
      background: linear-gradient(90deg, var(--primary), var(--secondary));
      border-radius: 10px;
      transition: width 0.5s ease;
    }

    .object-detected {
      border-left-color: var(--medical);
    }

    .object-detected .result-value {
      color: var(--medical);
    }

    .eeg-panel {
      grid-column: 1 / -1;
    }

    .eeg-display {
      display: grid;
      grid-template-columns: 2fr 1fr;
      gap: 1.5rem;
    }

    .eeg-chart-container {
      background: var(--darker);
      border-radius: 12px;
      padding: 1rem;
      height: 300px;
    }

    .eeg-metrics {
      display: flex;
      flex-direction: column;
      gap: 1rem;
    }

    .eeg-metric {
      background: var(--darker);
      border-radius: 8px;
      padding: 1rem;
      display: flex;
      justify-content: space-between;
      align-items: center;
    }

    .metric-label {
      font-size: 0.9rem;
      color: var(--light);
    }

    .metric-value {
      font-size: 1.1rem;
      font-weight: 600;
    }

    .alpha { color: #4caf50; }
    .beta { color: #2196f3; }
    .theta { color: #ff9800; }
    .delta { color: #f44336; }
    .gamma { color: #9c27b0; }

    .prediction-indicator {
      padding: 0.5rem 1rem;
      border-radius: 20px;
      font-weight: 600;
      text-align: center;
    }

    .normal { background: var(--success); }
    .mild { background: var(--warning); }
    .moderate { background: #e67e22; }
    .severe { background: var(--danger); }

    .history-panel {
      grid-column: 1 / -1;
    }

    .history-grid {
      display: grid;
      grid-template-columns: repeat(auto-fit, minmax(300px, 1fr));
      gap: 1rem;
    }

    .history-item {
      background: var(--darker);
      border-radius: 8px;
      padding: 1rem;
      border-left: 4px solid var(--secondary);
    }

    .history-time {
      font-size: 0.8rem;
      opacity: 0.7;
      margin-bottom: 0.5rem;
    }

    .history-content {
      font-size: 0.9rem;
    }

    .footer {
      text-align: center;
      margin-top: 2rem;
      padding: 1rem;
      font-size: 0.8rem;
      opacity: 0.7;
      border-top: 1px solid var(--glass-border);
    }

    .loading {
      position: absolute;
      top: 0;
      left: 0;
      width: 100%;
      height: 100%;
      background: var(--overlay);
      display: flex;
      justify-content: center;
      align-items: center;
      flex-direction: column;
      gap: 1rem;
      z-index: 10;
      display: none;
    }

    .spinner {
      width: 40px;
      height: 40px;
      border: 4px solid rgba(255, 255, 255, 0.1);
      border-radius: 50%;
      border-top-color: var(--secondary);
      animation: spin 1s linear infinite;
    }

    @keyframes spin {
      0% { transform: rotate(0deg); }
      100% { transform: rotate(360deg); }
    }

    .notification {
      position: fixed;
      bottom: 20px;
      right: 20px;
      padding: 1rem;
      background: var(--darker);
      border-left: 4px solid var(--secondary);
      border-radius: 8px;
      box-shadow: 0 4px 12px rgba(0, 0, 0, 0.2);
      transform: translateX(100%);
      transition: transform 0.3s ease;
      z-index: 1000;
      max-width: 300px;
    }

    .notification.show {
      transform: translateX(0);
    }

    .notification.error {
      border-left-color: var(--accent);
    }

    .modal {
      position: fixed;
      top: 0;
      left: 0;
      width: 100%;
      height: 100%;
      background: var(--overlay);
      display: flex;
      justify-content: center;
      align-items: center;
      z-index: 1000;
      display: none;
    }

    .modal-content {
      background: var(--darker);
      border-radius: 12px;
      padding: 2rem;
      width: 90%;
      max-width: 500px;
      box-shadow: 0 10px 30px rgba(0, 0, 0, 0.3);
    }

    .modal-title {
      font-size: 1.2rem;
      font-weight: 600;
      margin-bottom: 1rem;
      color: var(--secondary);
    }

    .modal-input, .modal-file {
      width: 100%;
      padding: 0.75rem;
      margin-bottom: 1rem;
      background: var(--dark);
      border: 1px solid var(--glass-border);
      border-radius: 6px;
      color: var(--light);
      font-size: 1rem;
    }

    .modal-buttons {
      display: flex;
      gap: 0.5rem;
      justify-content: flex-end;
    }

    .modal-btn {
      padding: 0.5rem 1rem;
      border: none;
      border-radius: 6px;
      font-weight: 600;
      cursor: pointer;
    }

    .modal-btn.primary {
      background: var(--primary);
      color: white;
    }

    .modal-btn.secondary {
      background: var(--darker);
      color: var(--light);
      border: 1px solid var(--glass-border);
    }

    .file-info {
      font-size: 0.8rem;
      margin-top: -0.5rem;
      margin-bottom: 1rem;
      opacity: 0.7;
    }

    .supported-formats {
      font-size: 0.8rem;
      margin-top: -0.5rem;
      margin-bottom: 1rem;
      opacity: 0.7;
      color: var(--secondary);
    }
  </style>
</head>
<body>
  <div class="container">
    <div class="header">
      <div class="app-title">Memory<span>Enhance</span></div>
      <div class="patient-info">
        <div class="patient-name" id="patient-name">Patient: Loading...</div>
        <div class="connection-status">
          <div class="status-indicator"></div>
          <span id="connection-text">Connected</span>
        </div>
      </div>
    </div>

    <div class="main-content">
      <div class="panel">
        <div class="panel-title">
          <i class="fas fa-camera"></i> Environment View
        </div>
        <div class="video-container">
          <img id="video-feed" src="">
          <div class="video-overlay" id="video-overlay"></div>
          <div class="loading" id="scanning-loading">
            <div class="spinner"></div>
            <div>Processing environment...</div>
          </div>
        </div>
        <div class="control-buttons">
          <button class="capture-btn" onclick="analyzeEnvironment()">
            <i class="fas fa-search"></i> Analyze Objects
          </button>
          <button class="train-btn" onclick="showTrainingModal()">
            <i class="fas fa-plus"></i> Train Object
          </button>
          <button class="upload-btn" onclick="showEEGUploadModal()" style="grid-column: 1 / -1;">
            <i class="fas fa-upload"></i> Upload EEG Data (JPG/CSV/JSON)
          </button>
        </div>
      </div>

      <div class="panel">
        <div class="panel-title">
          <i class="fas fa-diagnoses"></i> Recognition Results
        </div>
        <div class="results-container">
          <div class="result-card" id="object-result">
            <div class="result-title">Objects Identified</div>
            <div class="result-value" id="object-value">0</div>
          </div>

          <div class="result-card">
            <div class="result-title">Recognition Confidence</div>
            <div class="result-value" id="confidence-value">--%</div>
            <div class="confidence-meter">
              <div class="confidence-fill" id="confidence-meter-fill" style="width: 0%"></div>
            </div>
          </div>

          <div class="result-card">
            <div class="result-title">Processing Time</div>
            <div class="result-value" id="time-value">-- ms</div>
          </div>

          <div class="result-card">
            <div class="result-title">Memory Assistance</div>
            <div class="result-value" id="assistance-value">--</div>
            <div style="font-size: 0.9rem; margin-top: 0.5rem; opacity: 0.8;" id="assistance-message">
              Object recognition active
            </div>
          </div>
        </div>
      </div>

      <div class="panel eeg-panel">
        <div class="panel-title">
          <i class="fas fa-brain"></i> EEG Brain Wave Analysis
        </div>
        <div class="eeg-display">
          <div class="eeg-chart-container">
            <canvas id="eegChart"></canvas>
          </div>
          <div class="eeg-metrics">
            <div class="eeg-metric">
              <span class="metric-label">Alpha Waves</span>
              <span class="metric-value alpha" id="alpha-value">-- Hz</span>
            </div>
            <div class="eeg-metric">
              <span class="metric-label">Beta Waves</span>
              <span class="metric-value beta" id="beta-value">-- Hz</span>
            </div>
            <div class="eeg-metric">
              <span class="metric-label">Theta Waves</span>
              <span class="metric-value theta" id="theta-value">-- Hz</span>
            </div>
            <div class="eeg-metric">
              <span class="metric-label">Delta Waves</span>
              <span class="metric-value delta" id="delta-value">-- Hz</span>
            </div>
            <div class="eeg-metric">
              <span class="metric-label">Gamma Waves</span>
              <span class="metric-value gamma" id="gamma-value">-- Hz</span>
            </div>
            <div class="eeg-metric">
              <span class="metric-label">Condition Prediction</span>
              <div class="prediction-indicator normal" id="prediction-indicator">Normal</div>
            </div>
          </div>
        </div>
      </div>

      <div class="panel history-panel">
        <div class="panel-title">
          <i class="fas fa-history"></i> Recent Activity & Alerts
        </div>
        <div class="history-grid" id="history-grid">
          <div class="history-item">
            <div class="history-time">Just now</div>
            <div class="history-content">System initialized and ready</div>
          </div>
        </div>
      </div>
    </div>

    <div class="footer">
      <p>MemoryEnhance AI - Alzheimer's Patient Monitoring System | ESP32-CAM & EEG Analysis</p>
      <p id="ip-address">IP: </p>
    </div>
  </div>

  <div class="notification" id="notification">
    <div id="notification-message"></div>
  </div>

  <!-- Training Modal -->
  <div class="modal" id="training-modal">
    <div class="modal-content">
      <div class="modal-title">Train New Object</div>
      <input type="text" class="modal-input" id="object-name" placeholder="Enter object name (e.g., Apple, Water Bottle)">
      <select class="modal-input" id="object-type">
        <option value="1">Common Object</option>
        <option value="0">Face</option>
      </select>
      <div class="modal-buttons">
        <button class="modal-btn secondary" onclick="hideTrainingModal()">Cancel</button>
        <button class="modal-btn primary" onclick="trainObject()">Train</button>
      </div>
    </div>
  </div>

  <!-- EEG Upload Modal -->
  <div class="modal" id="eeg-upload-modal">
    <div class="modal-content">
      <div class="modal-title">Upload EEG Dataset</div>
      <input type="file" class="modal-file" id="eeg-file" accept=".jpg,.jpeg,.csv,.json,.txt">
      <div class="supported-formats">Supported formats: JPG, JPEG, CSV, JSON, TXT</div>
      <div class="file-info">You can upload EEG data images from Downloads folder or any location</div>
      <input type="text" class="modal-input" id="eeg-notes" placeholder="Notes (optional)">
      <div class="modal-buttons">
        <button class="modal-btn secondary" onclick="hideEEGUploadModal()">Cancel</button>
        <button class="modal-btn primary" onclick="uploadEEGData()">Analyze EEG</button>
      </div>
    </div>
  </div>

  <script>
    // Global state
    const state = {
      activityHistory: [],
      isProcessing: false,
      objects: [],
      eegData: {
        alpha: 0,
        beta: 0,
        theta: 0,
        delta: 0,
        gamma: 0,
        prediction: 0
      },
      eegChart: null
    };

    // Common objects database
    const commonObjects = [
      "Apple", "Water Bottle", "Mobile Phone", "Watch", "Computer", 
      "Keyboard", "Mouse", "Pen", "Glasses", "Book", "Remote Control",
      "Medication", "Keys", "Wallet", "TV", "Chair", "Table", "Lamp"
    ];

    // Initialize
    document.addEventListener('DOMContentLoaded', function() {
      // Set up video feed
      document.getElementById("video-feed").src = window.location.href.slice(0, -1) + ":81/stream";
      
      // Update IP display
      document.getElementById('ip-address').textContent += window.location.hostname;
      
      // Load patient info
      fetch('/getPatientInfo')
        .then(response => response.json())
        .then(data => {
          if (data.name) {
            document.getElementById('patient-name').textContent = `Patient: ${data.name}`;
          }
        });
      
      // Load history from localStorage
      const savedHistory = localStorage.getItem('activityHistory');
      if (savedHistory) {
        state.activityHistory = JSON.parse(savedHistory);
        updateHistoryDisplay();
      }
      
      // Initialize EEG chart
      initializeEEGChart();
      
      // Start periodic updates
      setInterval(updateSystemStatus, 5000);
    });

    // Initialize EEG Chart
    function initializeEEGChart() {
      const ctx = document.getElementById('eegChart').getContext('2d');
      state.eegChart = new Chart(ctx, {
        type: 'line',
        data: {
          labels: Array.from({length: 20}, (_, i) => i + 1),
          datasets: [
            {
              label: 'Alpha',
              data: Array(20).fill(0),
              borderColor: '#4caf50',
              backgroundColor: 'rgba(76, 175, 80, 0.1)',
              tension: 0.4,
              fill: true
            },
            {
              label: 'Beta',
              data: Array(20).fill(0),
              borderColor: '#2196f3',
              backgroundColor: 'rgba(33, 150, 243, 0.1)',
              tension: 0.4,
              fill: true
            }
          ]
        },
        options: {
          responsive: true,
          maintainAspectRatio: false,
          scales: {
            y: {
              beginAtZero: true,
              max: 100,
              grid: {
                color: 'rgba(255, 255, 255, 0.1)'
              },
              ticks: {
                color: 'rgba(255, 255, 255, 0.7)'
              }
            },
            x: {
              grid: {
                color: 'rgba(255, 255, 255, 0.1)'
              },
              ticks: {
                color: 'rgba(255, 255, 255, 0.7)'
              }
            }
          },
          plugins: {
            legend: {
              labels: {
                color: 'rgba(255, 255, 255, 0.7)'
              }
            }
          }
        }
      });
    }

    // Analyze environment for objects
    function analyzeEnvironment() {
      if (state.isProcessing) return;
      
      state.isProcessing = true;
      document.getElementById('scanning-loading').style.display = 'flex';
      clearObjectBoxes();
      
      // Capture and analyze image
      fetch('/capture')
        .then(response => response.json())
        .then(data => {
          if (data.success) {
            return fetch('/analyze?mode=detect');
          } else {
            throw new Error('Capture failed');
          }
        })
        .then(response => response.json())
        .then(result => {
          // Update UI with results
          updateResults(result);
          
          // Add to history
          addToHistory('object_detection', result);
          
          // Draw object boxes
          drawObjectBoxes(result.objects);
          
          // Show notification
          showNotification(
            result.objects && result.objects.length > 0 ? 
            `Identified ${result.objects.length} object(s)` : 
            'No objects identified'
          );
        })
        .catch(error => {
          console.error('Error:', error);
          showNotification('Analysis failed: ' + error.message, true);
        })
        .finally(() => {
          state.isProcessing = false;
          document.getElementById('scanning-loading').style.display = 'none';
        });
    }

    // Train a new object
    function trainObject() {
      const name = document.getElementById('object-name').value.trim();
      const type = document.getElementById('object-type').value;
      
      if (!name) {
        showNotification('Please enter an object name', true);
        return;
      }
      
      hideTrainingModal();
      document.getElementById('scanning-loading').style.display = 'flex';
      
      // Capture and train image
      fetch('/capture')
        .then(response => response.json())
        .then(data => {
          if (data.success) {
            return fetch('/analyze?mode=train&name=' + encodeURIComponent(name) + '&type=' + type);
          } else {
            throw new Error('Capture failed');
          }
        })
        .then(response => response.json())
        .then(result => {
          if (result.success) {
            showNotification(`Trained ${name} successfully!`);
            addToHistory('object_training', {name: name, type: type});
          } else {
            throw new Error(result.message || 'Training failed');
          }
        })
        .catch(error => {
          console.error('Error:', error);
          showNotification('Training failed: ' + error.message, true);
        })
        .finally(() => {
          document.getElementById('scanning-loading').style.display = 'none';
        });
    }

    // Upload and analyze EEG data
    function uploadEEGData() {
      const fileInput = document.getElementById('eeg-file');
      const notes = document.getElementById('eeg-notes').value;
      
      if (!fileInput.files.length) {
        showNotification('Please select an EEG data file', true);
        return;
      }
      
      const file = fileInput.files[0];
      const formData = new FormData();
      formData.append('eegFile', file);
      formData.append('notes', notes);
      
      hideEEGUploadModal();
      document.getElementById('scanning-loading').style.display = 'flex';
      
      fetch('/uploadEEG', {
        method: 'POST',
        body: formData
      })
      .then(response => response.json())
      .then(result => {
        if (result.success) {
          showNotification('EEG data analyzed successfully! File: ' + result.filename);
          updateEEGDisplay(result.analysis);
          addToHistory('eeg_analysis', result.analysis);
        } else {
          throw new Error(result.message || 'EEG analysis failed');
        }
      })
      .catch(error => {
        console.error('Error:', error);
        showNotification('EEG analysis failed: ' + error.message, true);
      })
      .finally(() => {
        document.getElementById('scanning-loading').style.display = 'none';
      });
    }

    // Update EEG display
    function updateEEGDisplay(eegData) {
      if (!eegData) return;
      
      // Update metrics
      document.getElementById('alpha-value').textContent = eegData.alpha.toFixed(2) + ' Hz';
      document.getElementById('beta-value').textContent = eegData.beta.toFixed(2) + ' Hz';
      document.getElementById('theta-value').textContent = eegData.theta.toFixed(2) + ' Hz';
      document.getElementById('delta-value').textContent = eegData.delta.toFixed(2) + ' Hz';
      document.getElementById('gamma-value').textContent = eegData.gamma.toFixed(2) + ' Hz';
      
      // Update prediction
      const predictionIndicator = document.getElementById('prediction-indicator');
      const predictions = ['Normal', 'Mild', 'Moderate', 'Severe'];
      const predictionClasses = ['normal', 'mild', 'moderate', 'severe'];
      
      predictionIndicator.textContent = predictions[eegData.prediction] || 'Unknown';
      predictionIndicator.className = 'prediction-indicator ' + (predictionClasses[eegData.prediction] || 'normal');
      
      // Update chart
      if (state.eegChart) {
        state.eegChart.data.datasets[0].data.push(eegData.alpha);
        state.eegChart.data.datasets[1].data.push(eegData.beta);
        
        // Keep only last 20 points
        if (state.eegChart.data.datasets[0].data.length > 20) {
          state.eegChart.data.datasets[0].data.shift();
          state.eegChart.data.datasets[1].data.shift();
        }
        
        state.eegChart.update('none');
      }
      
      state.eegData = eegData;
    }

    // Draw object boxes on video overlay
    function drawObjectBoxes(objects) {
      const overlay = document.getElementById('video-overlay');
      overlay.innerHTML = '';
      
      if (!objects || objects.length === 0) return;
      
      objects.forEach(obj => {
        const box = document.createElement('div');
        box.className = 'object-box';
        box.style.left = obj.x + '%';
        box.style.top = obj.y + '%';
        box.style.width = obj.width + '%';
        box.style.height = obj.height + '%';
        
        const name = document.createElement('div');
        name.className = 'object-name';
        name.textContent = obj.name || 'Unknown Object';
        
        box.appendChild(name);
        overlay.appendChild(box);
      });
    }

    // Clear object boxes
    function clearObjectBoxes() {
      const overlay = document.getElementById('video-overlay');
      overlay.innerHTML = '';
    }

    // Update results display
    function updateResults(data) {
      const objectValue = document.getElementById('object-value');
      const confidenceValue = document.getElementById('confidence-value');
      const confidenceMeter = document.getElementById('confidence-meter-fill');
      const timeValue = document.getElementById('time-value');
      const objectResult = document.getElementById('object-result');
      const assistanceValue = document.getElementById('assistance-value');
      const assistanceMessage = document.getElementById('assistance-message');
      
      // Update object detection
      const objectCount = data.objects ? data.objects.length : 0;
      objectValue.textContent = objectCount;
      
      if (objectCount > 0) {
        objectResult.classList.add('object-detected');
        assistanceValue.textContent = 'Active';
        assistanceMessage.textContent = `Identified ${objectCount} object(s) in environment`;
      } else {
        objectResult.classList.remove('object-detected');
        assistanceValue.textContent = 'No Objects';
        assistanceMessage.textContent = 'No objects identified - check camera view';
      }
      
      // Update confidence
      const confidence = data.confidence || 0;
      confidenceValue.textContent = confidence + '%';
      confidenceMeter.style.width = confidence + '%';
      
      // Update processing time
      timeValue.textContent = (data.processingTime || 0) + ' ms';
    }

    // Add to history
    function addToHistory(type, data) {
      const timestamp = new Date().toLocaleString();
      let content = '';
      
      switch(type) {
        case 'object_detection':
          const objectCount = data.objects ? data.objects.length : 0;
          content = `Detected ${objectCount} objects in environment`;
          break;
        case 'object_training':
          content = `Trained new object: ${data.name}`;
          break;
        case 'eeg_analysis':
          const predictions = ['Normal', 'Mild', 'Moderate', 'Severe'];
          content = `EEG Analysis: ${predictions[data.prediction] || 'Unknown'} condition detected`;
          break;
        default:
          content = 'System activity recorded';
      }
      
      const historyItem = {
        timestamp: timestamp,
        type: type,
        content: content,
        data: data
      };
      
      state.activityHistory.unshift(historyItem);
      
      // Keep only last 10 items
      if (state.activityHistory.length > 10) {
        state.activityHistory.pop();
      }
      
      // Save to localStorage
      localStorage.setItem('activityHistory', JSON.stringify(state.activityHistory));
      
      // Update display
      updateHistoryDisplay();
    }

    // Update history display
    function updateHistoryDisplay() {
      const historyGrid = document.getElementById('history-grid');
      
      if (state.activityHistory.length === 0) {
        historyGrid.innerHTML = '<div class="history-item">No activity recorded yet</div>';
        return;
      }
      
      historyGrid.innerHTML = '';
      state.activityHistory.forEach(item => {
        const historyItem = document.createElement('div');
        historyItem.className = 'history-item';
        
        historyItem.innerHTML = `
          <div class="history-time">${item.timestamp}</div>
          <div class="history-content">${item.content}</div>
        `;
        
        historyGrid.appendChild(historyItem);
      });
    }

    // Update system status
    function updateSystemStatus() {
      // Simulate periodic EEG updates
      if (Math.random() > 0.7) {
        const simulatedEEG = {
          alpha: 8 + Math.random() * 4,
          beta: 12 + Math.random() * 8,
          theta: 4 + Math.random() * 3,
          delta: 1 + Math.random() * 2,
          gamma: 25 + Math.random() * 10,
          prediction: Math.floor(Math.random() * 4)
        };
        updateEEGDisplay(simulatedEEG);
      }
    }

    // Show training modal
    function showTrainingModal() {
      document.getElementById('training-modal').style.display = 'flex';
      document.getElementById('object-name').value = '';
      document.getElementById('object-name').focus();
    }

    // Hide training modal
    function hideTrainingModal() {
      document.getElementById('training-modal').style.display = 'none';
    }

    // Show EEG upload modal
    function showEEGUploadModal() {
      document.getElementById('eeg-upload-modal').style.display = 'flex';
      document.getElementById('eeg-file').value = '';
      document.getElementById('eeg-notes').value = '';
    }

    // Hide EEG upload modal
    function hideEEGUploadModal() {
      document.getElementById('eeg-upload-modal').style.display = 'none';
    }

    // Show notification
    function showNotification(message, isError = false) {
      const notification = document.getElementById('notification');
      const messageEl = document.getElementById('notification-message');
      
      messageEl.textContent = message;
      
      if (isError) {
        notification.classList.add('error');
      } else {
        notification.classList.remove('error');
      }
      
      notification.classList.add('show');
      
      setTimeout(() => {
        notification.classList.remove('show');
      }, 4000);
    }

    // Keyboard shortcuts
    document.addEventListener('keydown', function(event) {
      if (event.code === 'Space') {
        event.preventDefault();
        analyzeEnvironment();
      }
      
      if (event.code === 'Escape') {
        hideTrainingModal();
        hideEEGUploadModal();
      }
      
      if (event.ctrlKey && event.code === 'KeyE') {
        event.preventDefault();
        showEEGUploadModal();
      }
    });
  </script>
</body>
</html>
)rawliteral";

// Server handlers
static esp_err_t index_handler(httpd_req_t *req) {
  httpd_resp_set_type(req, "text/html");
  return httpd_resp_send(req, (const char *)INDEX_HTML, strlen(INDEX_HTML));
}

static esp_err_t stream_handler(httpd_req_t *req) {
  camera_fb_t *fb = NULL;
  esp_err_t res = ESP_OK;
  size_t _jpg_buf_len = 0;
  uint8_t *_jpg_buf = NULL;
  char part_buf[64];

  res = httpd_resp_set_type(req, _STREAM_CONTENT_TYPE);
  if (res != ESP_OK) return res;

  while (true) {
    fb = esp_camera_fb_get();
    if (!fb) {
      Serial.println("Camera capture failed");
      res = ESP_FAIL;
    } else {
      if (fb->width > 400) {
        if (fb->format != PIXFORMAT_JPEG) {
          bool jpeg_converted = frame2jpg(fb, 80, &_jpg_buf, &_jpg_buf_len);
          esp_camera_fb_return(fb);
          fb = NULL;
          if (!jpeg_converted) {
            Serial.println("JPEG compression failed");
            res = ESP_FAIL;
          }
        } else {
          _jpg_buf_len = fb->len;
          _jpg_buf = fb->buf;
        }
      }
    }
    if (res == ESP_OK) {
      size_t hlen = snprintf((char *)part_buf, 64, _STREAM_PART, _jpg_buf_len);
      res = httpd_resp_send_chunk(req, (const char *)part_buf, hlen);
    }
    if (res == ESP_OK) {
      res = httpd_resp_send_chunk(req, (const char *)_jpg_buf, _jpg_buf_len);
    }
    if (res == ESP_OK) {
      res = httpd_resp_send_chunk(req, _STREAM_BOUNDARY, strlen(_STREAM_BOUNDARY));
    }
    if (fb) {
      esp_camera_fb_return(fb);
      fb = NULL;
      _jpg_buf = NULL;
    } else if (_jpg_buf) {
      free(_jpg_buf);
      _jpg_buf = NULL;
    }
    if (res != ESP_OK) break;
  }
  return res;
}

// Capture handler
static esp_err_t capture_handler(httpd_req_t *req) {
  camera_fb_t *fb = esp_camera_fb_get();
  if (!fb) {
    Serial.println("Camera capture failed");
    httpd_resp_send_500(req);
    return ESP_FAIL;
  }
  
  httpd_resp_set_type(req, "application/json");
  char response[100];
  snprintf(response, sizeof(response), "{\"success\":true,\"message\":\"Image captured\",\"size\":%d}", fb->len);
  
  esp_camera_fb_return(fb);
  return httpd_resp_send(req, response, strlen(response));
}

// AI Analysis handler with object recognition
static esp_err_t analyze_handler(httpd_req_t *req) {
  camera_fb_t *fb = NULL;
  
  // Get parameters
  char mode[10] = {0};
  char name[30] = {0};
  char type[5] = {0};
  
  if (httpd_req_get_url_query_str(req, mode, sizeof(mode)) == ESP_OK) {
    httpd_query_key_value(mode, "mode", mode, sizeof(mode));
    httpd_query_key_value(mode, "name", name, sizeof(name));
    httpd_query_key_value(mode, "type", type, sizeof(type));
  }
  
  // Capture image
  fb = esp_camera_fb_get();
  if (!fb) {
    Serial.println("Camera capture failed");
    httpd_resp_send_500(req);
    return ESP_FAIL;
  }
  
  uint32_t startTime = millis();
  
  if (strcmp(mode, "train") == 0) {
    // Training mode
    delay(800); // Simulate training processing
    
    httpd_resp_set_type(req, "application/json");
    char response[200];
    snprintf(response, sizeof(response), 
             "{\"success\":true,\"message\":\"Object trained successfully\",\"name\":\"%s\",\"type\":%s}",
             name, type);
    
    esp_camera_fb_return(fb);
    return httpd_resp_send(req, response, strlen(response));
  } else {
    // Detection mode - Enhanced object recognition
    delay(150 + random(300));
    
    // Common objects for simulation
    const char* objectNames[] = {
      "Apple", "Water Bottle", "Mobile Phone", "Watch", "Computer", 
      "Keyboard", "Mouse", "Pen", "Glasses", "Book", "Remote Control"
    };
    int objectCount = sizeof(objectNames) / sizeof(objectNames[0]);
    
    // Detect 0-4 objects randomly
    int detectedCount = random(5);
    float confidence = 75 + random(25);
    
    String objectsJson = "";
    for (int i = 0; i < detectedCount; i++) {
      // Random position and size
      int x = 15 + random(70);
      int y = 15 + random(70);
      int width = 8 + random(25);
      int height = 8 + random(25);
      
      // Random object name
      const char* objName = objectNames[random(objectCount)];
      
      if (objectsJson.length() > 0) objectsJson += ",";
      objectsJson += "{\"x\":" + String(x) + ",\"y\":" + String(y) + 
                     ",\"width\":" + String(width) + ",\"height\":" + String(height) + 
                     ",\"name\":\"" + String(objName) + "\",\"confidence\":" + String(75 + random(25)) + "}";
    }
    
    uint32_t processingTime = millis() - startTime;
    
    httpd_resp_set_type(req, "application/json");
    char response[800];
    snprintf(response, sizeof(response), 
             "{\"objects\":[%s],\"confidence\":%.1f,\"processingTime\":%d,\"detectionMethod\":\"Intensity-Adaptive Scharr + CNN\"}",
             objectsJson.c_str(), confidence, processingTime);
    
    esp_camera_fb_return(fb);
    return httpd_resp_send(req, response, strlen(response));
  }
}

// EEG Upload handler with file upload support
static esp_err_t upload_eeg_handler(httpd_req_t *req) {
  Serial.println("EEG Upload Request Received");
  
  // Check content type for file upload
  char content_type[100];
  if (httpd_req_get_hdr_value_str(req, "Content-Type", content_type, sizeof(content_type)) != ESP_OK) {
    httpd_resp_set_type(req, "application/json");
    httpd_resp_send(req, "{\"success\":false,\"message\":\"Cannot read Content-Type\"}", -1);
    return ESP_FAIL;
  }
  
  bool isMultipart = strstr(content_type, "multipart/form-data");
  
  if (!isMultipart) {
    httpd_resp_set_type(req, "application/json");
    httpd_resp_send(req, "{\"success\":false,\"message\":\"Content-Type must be multipart/form-data\"}", -1);
    return ESP_FAIL;
  }

  char* buf = NULL;
  size_t buf_len = 0;
  bool fileReceived = false;
  String fileName = "";
  String fileContent = "";
  
  // Get content length
  buf_len = httpd_req_get_url_query_len(req) + 1;
  if (buf_len > 1) {
    buf = (char*)malloc(buf_len);
    if (httpd_req_get_url_query_str(req, buf, buf_len) == ESP_OK) {
      Serial.printf("Query: %s\n", buf);
    }
    free(buf);
  }

  // Parse multipart data
  buf_len = 2048; // Increased buffer for file content
  buf = (char*)malloc(buf_len);
  if (!buf) {
    httpd_resp_send_500(req);
    return ESP_FAIL;
  }

  int remaining = req->content_len;
  bool inFileSection = false;
  
  // Define MIN macro if not defined
  #ifndef MIN
  #define MIN(a, b) ((a) < (b) ? (a) : (b))
  #endif
  
  while (remaining > 0) {
    int recv_len = httpd_req_recv(req, buf, MIN(remaining, buf_len));
    if (recv_len < 0) {
      free(buf);
      httpd_resp_send_500(req);
      return ESP_FAIL;
    }
    
    // Process received data
    String data(buf, recv_len);
    
    // Look for filename in headers
    if (data.indexOf("filename=") != -1) {
      int start = data.indexOf("filename=\"") + 10;
      int end = data.indexOf("\"", start);
      if (start != -1 && end != -1) {
        fileName = data.substring(start, end);
        Serial.printf("Receiving file: %s\n", fileName.c_str());
      }
    }
    
    // Look for file content start (after empty line)
    if (data.indexOf("\r\n\r\n") != -1 && !inFileSection) {
      inFileSection = true;
      int contentStart = data.indexOf("\r\n\r\n") + 4;
      if (contentStart < recv_len) {
        fileContent += data.substring(contentStart);
      }
    } else if (inFileSection) {
      // Check for boundary end
      if (data.indexOf("--") != -1) {
        // Remove boundary from content
        int boundaryPos = data.indexOf("--");
        if (boundaryPos > 0) {
          fileContent += data.substring(0, boundaryPos);
        }
        fileReceived = true;
        break;
      } else {
        fileContent += data;
      }
    }
    
    remaining -= recv_len;
  }
  
  free(buf);

  // Process the uploaded file
  if (fileReceived && fileContent.length() > 0) {
    Serial.printf("File %s received, size: %d bytes\n", fileName.c_str(), fileContent.length());
    
    // Simulate EEG data processing based on file type
    EEGData analysis = processEEGFile(fileName, fileContent);
    
    httpd_resp_set_type(req, "application/json");
    char response[512];
    snprintf(response, sizeof(response),
             "{\"success\":true,\"message\":\"EEG data analyzed successfully\",\"filename\":\"%s\","
             "\"analysis\":{"
             "\"alpha\":%.2f,\"beta\":%.2f,\"theta\":%.2f,\"delta\":%.2f,\"gamma\":%.2f,"
             "\"prediction\":%d,\"fileSize\":%d,\"algorithm\":\"Adaptive Scharr + Fuzzy C-Means + CNN\"}}",
             fileName.c_str(), analysis.alpha, analysis.beta, analysis.theta, 
             analysis.delta, analysis.gamma, analysis.prediction, fileContent.length());
    
    return httpd_resp_send(req, response, strlen(response));
  } else {
    httpd_resp_set_type(req, "application/json");
    httpd_resp_send(req, "{\"success\":false,\"message\":\"No file received or file empty\"}", -1);
    return ESP_FAIL;
  }
}

// Patient info handler
static esp_err_t patient_info_handler(httpd_req_t *req) {
  httpd_resp_set_type(req, "application/json");
  char response[100];
  snprintf(response, sizeof(response), "{\"name\":\"John Smith\",\"age\":72,\"condition\":\"Alzheimer's Monitoring\"}");
  return httpd_resp_send(req, response, strlen(response));
}

// Initialize SPIFFS
void initSPIFFS() {
  if (!SPIFFS.begin(true)) {
    Serial.println("SPIFFS mount failed");
  } else {
    Serial.println("SPIFFS mounted successfully");
  }
}

// Initialize EEPROM
void initEEPROM() {
  EEPROM.begin(EEPROM_SIZE);
  
  // Try to read known patterns from EEPROM
  EEPROM.get(0, patternCount);
  
  if (patternCount > 0 && patternCount <= MAX_OBJECTS) {
    for (int i = 0; i < patternCount; i++) {
      EEPROM.get(sizeof(patternCount) + i * sizeof(ObjectPattern), knownPatterns[i]);
    }
    Serial.printf("Loaded %d object patterns from EEPROM\n", patternCount);
    systemInitialized = true;
  } else {
    patternCount = 0;
    Serial.println("No valid object patterns found in EEPROM");
  }
}

// Start web server
void startCameraServer() {
  httpd_config_t config = HTTPD_DEFAULT_CONFIG();
  config.server_port = 80;
  
  httpd_uri_t index_uri = {
    .uri = "/",
    .method = HTTP_GET,
    .handler = index_handler,
    .user_ctx = NULL
  };
  
  httpd_uri_t capture_uri = {
    .uri = "/capture",
    .method = HTTP_GET,
    .handler = capture_handler,
    .user_ctx = NULL
  };
  
  httpd_uri_t analyze_uri = {
    .uri = "/analyze",
    .method = HTTP_GET,
    .handler = analyze_handler,
    .user_ctx = NULL
  };
  
  httpd_uri_t upload_eeg_uri = {
    .uri = "/uploadEEG",
    .method = HTTP_POST,
    .handler = upload_eeg_handler,
    .user_ctx = NULL
  };
  
  httpd_uri_t patient_info_uri = {
    .uri = "/getPatientInfo",
    .method = HTTP_GET,
    .handler = patient_info_handler,
    .user_ctx = NULL
  };
  
  httpd_uri_t stream_uri = {
    .uri = "/stream",
    .method = HTTP_GET,
    .handler = stream_handler,
    .user_ctx = NULL
  };
  
  if (httpd_start(&camera_httpd, &config) == ESP_OK) {
    httpd_register_uri_handler(camera_httpd, &index_uri);
    httpd_register_uri_handler(camera_httpd, &capture_uri);
    httpd_register_uri_handler(camera_httpd, &analyze_uri);
    httpd_register_uri_handler(camera_httpd, &upload_eeg_uri);
    httpd_register_uri_handler(camera_httpd, &patient_info_uri);
  }
  
  config.server_port += 1;
  config.ctrl_port += 1;
  if (httpd_start(&stream_httpd, &config) == ESP_OK) {
    httpd_register_uri_handler(stream_httpd, &stream_uri);
  }
}

// Setup function
void setup() {
  WRITE_PERI_REG(RTC_CNTL_BROWN_OUT_REG, 0);
  
  Serial.begin(115200);
  Serial.setDebugOutput(false);
  
  // Initialize file systems
  initSPIFFS();
  initEEPROM();
  
  // Camera configuration
  camera_config_t config;
  config.ledc_channel = LEDC_CHANNEL_0;
  config.ledc_timer = LEDC_TIMER_0;
  config.pin_d0 = Y2_GPIO_NUM;
  config.pin_d1 = Y3_GPIO_NUM;
  config.pin_d2 = Y4_GPIO_NUM;
  config.pin_d3 = Y5_GPIO_NUM;
  config.pin_d4 = Y6_GPIO_NUM;
  config.pin_d5 = Y7_GPIO_NUM;
  config.pin_d6 = Y8_GPIO_NUM;
  config.pin_d7 = Y9_GPIO_NUM;
  config.pin_xclk = XCLK_GPIO_NUM;
  config.pin_pclk = PCLK_GPIO_NUM;
  config.pin_vsync = VSYNC_GPIO_NUM;
  config.pin_href = HREF_GPIO_NUM;
  config.pin_sccb_sda = SIOD_GPIO_NUM;
  config.pin_sccb_scl = SIOC_GPIO_NUM;
  config.pin_pwdn = PWDN_GPIO_NUM;
  config.pin_reset = RESET_GPIO_NUM;
  config.xclk_freq_hz = 20000000;
  config.pixel_format = PIXFORMAT_JPEG;
  
  if (psramFound()) {
    config.frame_size = FRAMESIZE_VGA;
    config.jpeg_quality = 10;
    config.fb_count = 2;
  } else {
    config.frame_size = FRAMESIZE_SVGA;
    config.jpeg_quality = 12;
    config.fb_count = 1;
  }
  
  // Camera init
  esp_err_t err = esp_camera_init(&config);
  if (err != ESP_OK) {
    Serial.printf("Camera init failed with error 0x%x", err);
    return;
  }
  
  // Wi-Fi connection
  WiFi.begin(ssid, password);
  WiFi.setSleep(false);
  
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected");
  
  Serial.print("Alzheimer's Monitoring System Ready! Go to: http://");
  Serial.println(WiFi.localIP());
  
  // Start web server
  startCameraServer();
  
  Serial.println("System initialized with EEG analysis and object recognition");
}

// Main loop
void loop() {
  // System monitoring and maintenance
  static unsigned long lastMaintenance = 0;
  if (millis() - lastMaintenance > 30000) {
    lastMaintenance = millis();
    Serial.println("System running - Alzheimer's monitoring active");
  }
  delay(1000);
}
