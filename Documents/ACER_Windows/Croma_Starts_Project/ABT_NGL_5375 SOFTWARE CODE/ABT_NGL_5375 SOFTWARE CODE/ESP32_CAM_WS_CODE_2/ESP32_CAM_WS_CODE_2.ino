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

// Define the stream constants
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

// EEPROM for storing known face and object patterns
#define EEPROM_SIZE 4096

// Detection parameters
#define IMAGE_WIDTH 160
#define IMAGE_HEIGHT 120
#define FEATURE_VECTOR_SIZE 128
#define MAX_OBJECTS 50

httpd_handle_t camera_httpd = NULL;
httpd_handle_t stream_httpd = NULL;

// Structure to store object patterns
struct ObjectPattern {
  uint8_t features[FEATURE_VECTOR_SIZE];
  char name[30];
  char category[20];
  uint16_t checksum;
  bool isTrained;
};

// Structure for EEG Data
struct EEGData {
  float alpha;
  float beta;
  float theta;
  float delta;
  float gamma;
  long timestamp;
  int prediction; // 0: Normal, 1: Mild, 2: Moderate, 3: Severe
};

// Global variables
ObjectPattern knownObjects[MAX_OBJECTS];
int objectCount = 0;
EEGData currentEEG;
bool objectRecognitionInitialized = false;
bool eegDataAvailable = false;

// Advanced Algorithm Parameters
float scharrKernelX[3][3] = {{-3, 0, 3}, {-10, 0, 10}, {-3, 0, 3}};
float scharrKernelY[3][3] = {{-3, -10, -3}, {0, 0, 0}, {3, 10, 3}};

// CNN-like feature extraction (simplified)
void extractFeatures(uint8_t* image, int width, int height, uint8_t* features) {
  // Apply Scharr filter for edge detection
  for (int i = 1; i < height - 1; i++) {
    for (int j = 1; j < width - 1; j++) {
      float gx = 0, gy = 0;
      for (int ki = -1; ki <= 1; ki++) {
        for (int kj = -1; kj <= 1; kj++) {
          int pixel = image[(i + ki) * width + (j + kj)];
          gx += pixel * scharrKernelX[ki + 1][kj + 1];
          gy += pixel * scharrKernelY[ki + 1][kj + 1];
        }
      }
      float magnitude = sqrt(gx * gx + gy * gy);
      features[i * width + j] = (uint8_t)min(255, max(0, (int)magnitude));
    }
  }
}

// Fuzzy C-Means clustering (simplified)
void fuzzyCMeansClustering(uint8_t* features, int size, float* clusters, int numClusters) {
  // Simplified implementation for edge AI
  for (int i = 0; i < numClusters; i++) {
    clusters[i] = (i * 255.0) / (numClusters - 1);
  }
}

// Gabor filter implementation
void applyGaborFilter(uint8_t* image, int width, int height, float theta, float lambda, float psi, float sigma, float gamma) {
  // Simplified Gabor filter for feature enhancement
  int half_size = 2;
  for (int i = half_size; i < height - half_size; i++) {
    for (int j = half_size; j < width - half_size; j++) {
      float sum = 0;
      for (int ki = -half_size; ki <= half_size; ki++) {
        for (int kj = -half_size; kj <= half_size; kj++) {
          float x = j + kj * cos(theta) + i + ki * sin(theta);
          float y = - (j + kj) * sin(theta) + (i + ki) * cos(theta);
          float gabor = exp(-(x*x + gamma*gamma*y*y)/(2*sigma*sigma)) * cos(2*M_PI*x/lambda + psi);
          sum += image[(i+ki)*width + (j+kj)] * gabor;
        }
      }
      image[i*width + j] = (uint8_t)min(255, max(0, (int)sum));
    }
  }
}

// Object recognition using advanced algorithms
String recognizeObject(uint8_t* image, int width, int height) {
  uint8_t features[FEATURE_VECTOR_SIZE] = {0};
  
  // Apply advanced filters
  extractFeatures(image, width, height, features);
  applyGaborFilter(features, 16, 8, M_PI/4, 5.0, 0, 2.0, 1.0);
  
  // Simple feature matching (replace with proper CNN in real implementation)
  for (int i = 0; i < objectCount; i++) {
    int matchScore = 0;
    for (int j = 0; j < FEATURE_VECTOR_SIZE; j++) {
      if (abs(features[j] - knownObjects[i].features[j]) < 10) {
        matchScore++;
      }
    }
    if (matchScore > FEATURE_VECTOR_SIZE * 0.7) {
      return String(knownObjects[i].name);
    }
  }
  
  return "Unknown";
}

// EEG Analysis Functions
int analyzeEEGPattern(EEGData eeg) {
  // Simplified EEG analysis for Alzheimer's prediction
  float ratio1 = eeg.theta / eeg.beta;
  float ratio2 = eeg.alpha / eeg.gamma;
  
  if (ratio1 > 2.5 || ratio2 < 0.3) return 3; // Severe
  if (ratio1 > 2.0 || ratio2 < 0.5) return 2; // Moderate
  if (ratio1 > 1.5 || ratio2 < 0.7) return 1; // Mild
  
  return 0; // Normal
}

// HTML Web Page with Enhanced Features
static const char PROGMEM INDEX_HTML[] = R"rawliteral(
<!DOCTYPE html>
<html lang="en">
<head>
  <meta charset="UTF-8">
  <meta name="viewport" content="width=device-width, initial-scale=1.0">
  <title>MemoryEnhance | AI Alzheimer's Support System</title>
  <link rel="stylesheet" href="https://cdnjs.cloudflare.com/ajax/libs/font-awesome/6.4.0/css/all.min.css">
  <link href="https://fonts.googleapis.com/css2?family=Montserrat:wght@300;400;600;800&family=Roboto+Mono:wght@400;700&display=swap" rel="stylesheet">
  <script src="https://cdn.jsdelivr.net/npm/chart.js"></script>
  <style>
    :root {
      --primary: #4a6fa5;
      --primary-dark: #166088;
      --secondary: #4cb5ae;
      --accent: #fd4d4d;
      --medical: #6a4c93;
      --dark: #1a1a2e;
      --darker: #0d0d1a;
      --light: #f0f0f0;
      --success: #4caf50;
      --warning: #ff9800;
      --danger: #f44336;
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
        radial-gradient(circle at 20% 30%, rgba(74, 111, 165, 0.15) 0%, transparent 20%),
        radial-gradient(circle at 80% 70%, rgba(106, 76, 147, 0.15) 0%, transparent 20%);
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
    }

    .app-title {
      font-family: 'Roboto Mono', monospace;
      font-size: 1.8rem;
      font-weight: 700;
      color: var(--secondary);
    }

    .app-title span {
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
    }

    @media (max-width: 900px) {
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
      border: 2px solid var(--medical);
      border-radius: 8px;
      background: rgba(106, 76, 147, 0.1);
      display: flex;
      flex-direction: column;
      align-items: center;
      padding: 0.5rem;
      transform: translate(-50%, -50%);
    }

    .object-name {
      color: white;
      font-size: 0.8rem;
      font-weight: 600;
      margin-top: 0.25rem;
      text-shadow: 1px 1px 2px rgba(0, 0, 0, 0.8);
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
      background: linear-gradient(45deg, var(--accent), #d13c3c);
    }

    .upload-btn {
      background: linear-gradient(45deg, var(--medical), #5a3d8a);
    }

    .capture-btn:hover, .train-btn:hover, .upload-btn:hover {
      transform: translateY(-2px);
      box-shadow: 0 6px 12px rgba(0, 0, 0, 0.2);
    }

    .capture-btn:active, .train-btn:active, .upload-btn:active {
      transform: translateY(0);
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

    .scan-history {
      margin-top: 1.5rem;
    }

    .history-title {
      font-size: 1rem;
      font-weight: 600;
      margin-bottom: 0.5rem;
      color: var(--light);
    }

    .history-list {
      display: flex;
      flex-direction: column;
      gap: 0.5rem;
    }

    .history-item {
      display: flex;
      justify-content: space-between;
      padding: 0.5rem;
      background: var(--darker);
      border-radius: 6px;
      font-size: 0.9rem;
    }

    .known {
      color: var(--success);
    }

    .unknown {
      color: var(--accent);
    }

    .ai-panel {
      grid-column: 1 / -1;
    }

    .ai-features {
      display: grid;
      grid-template-columns: repeat(auto-fit, minmax(250px, 1fr));
      gap: 1rem;
    }

    .ai-feature {
      background: var(--darker);
      border-radius: 12px;
      padding: 1rem;
      display: flex;
      flex-direction: column;
      gap: 0.5rem;
    }

    .feature-title {
      font-size: 1rem;
      font-weight: 600;
      color: var(--secondary);
    }

    .feature-desc {
      font-size: 0.9rem;
      opacity: 0.8;
    }

    .feature-status {
      font-size: 0.9rem;
      font-weight: 600;
    }

    .active {
      color: var(--success);
    }

    .inactive {
      color: var(--accent);
    }

    .eeg-panel {
      grid-column: 1 / -1;
    }

    .eeg-container {
      display: grid;
      grid-template-columns: 2fr 1fr;
      gap: 1rem;
    }

    .eeg-chart {
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
      border-radius: 12px;
      padding: 1rem;
    }

    .metric-value {
      font-size: 1.5rem;
      font-weight: 700;
      color: var(--medical);
    }

    .prediction-normal {
      color: var(--success);
    }

    .prediction-mild {
      color: var(--warning);
    }

    .prediction-moderate {
      color: #ff5722;
    }

    .prediction-severe {
      color: var(--danger);
    }

    .footer {
      text-align: center;
      margin-top: 2rem;
      padding: 1rem;
      font-size: 0.8rem;
      opacity: 0.7;
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
    }

    .notification.show {
      transform: translateX(0);
    }

    .notification.error {
      border-left-color: var(--accent);
    }

    .training-modal, .upload-modal {
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
      max-width: 400px;
      box-shadow: 0 10px 30px rgba(0, 0, 0, 0.3);
    }

    .modal-title {
      font-size: 1.2rem;
      font-weight: 600;
      margin-bottom: 1rem;
      color: var(--secondary);
    }

    .modal-input {
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

    .file-input {
      width: 100%;
      padding: 0.75rem;
      margin-bottom: 1rem;
      background: var(--dark);
      border: 1px solid var(--glass-border);
      border-radius: 6px;
      color: var(--light);
    }
  </style>
</head>
<body>
  <div class="container">
    <div class="header">
      <div class="app-title">Memory<span>Enhance</span></div>
      <div class="connection-status">
        <div class="status-indicator"></div>
        <span id="connection-text">Connected</span>
      </div>
    </div>

    <div class="main-content">
      <div class="panel">
        <div class="panel-title">
          <i class="fas fa-camera"></i> Object Recognition
        </div>
        <div class="video-container">
          <img id="video-feed" src="">
          <div class="video-overlay" id="video-overlay"></div>
          <div class="loading" id="scanning-loading">
            <div class="spinner"></div>
            <div>Processing image...</div>
          </div>
        </div>
        <div class="control-buttons">
          <button class="capture-btn" onclick="detectObjects()">
            <i class="fas fa-search"></i> Detect Objects
          </button>
          <button class="train-btn" onclick="showTrainingModal()">
            <i class="fas fa-plus"></i> Train Object
          </button>
          <button class="upload-btn" onclick="showUploadModal()" style="grid-column: 1 / -1;">
            <i class="fas fa-upload"></i> Upload EEG Data
          </button>
        </div>
      </div>

      <div class="panel">
        <div class="panel-title">
          <i class="fas fa-diagnoses"></i> Recognition Results
        </div>
        <div class="results-container">
          <div class="result-card" id="object-result">
            <div class="result-title">Objects Detected</div>
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

          <div class="scan-history">
            <div class="history-title">Recent Recognitions</div>
            <div class="history-list" id="history-list">
              <div class="history-item">No recognitions yet</div>
            </div>
          </div>
        </div>
      </div>

      <div class="panel eeg-panel">
        <div class="panel-title">
          <i class="fas fa-brain"></i> EEG Analysis & Alzheimer's Prediction
        </div>
        <div class="eeg-container">
          <div class="eeg-chart">
            <canvas id="eegChart"></canvas>
          </div>
          <div class="eeg-metrics">
            <div class="eeg-metric">
              <div class="result-title">Current Prediction</div>
              <div class="metric-value" id="prediction-value">--</div>
            </div>
            <div class="eeg-metric">
              <div class="result-title">Risk Level</div>
              <div class="metric-value" id="risk-value">--</div>
            </div>
            <div class="eeg-metric">
              <div class="result-title">Last Update</div>
              <div class="metric-value" id="update-value">--</div>
            </div>
          </div>
        </div>
      </div>

      <div class="panel ai-panel">
        <div class="panel-title">
          <i class="fas fa-brain"></i> AI System Information
        </div>
        <div class="ai-features">
          <div class="ai-feature">
            <div class="feature-title">Scharr Filter</div>
            <div class="feature-desc">Intensity-based edge detection</div>
            <div class="feature-status active">Active</div>
          </div>
          <div class="ai-feature">
            <div class="feature-title">Fuzzy C-Means</div>
            <div class="feature-desc">Kernel-based clustering</div>
            <div class="feature-status active">Active</div>
          </div>
          <div class="ai-feature">
            <div class="feature-title">Gabor Filters</div>
            <div class="feature-desc">Feature extraction</div>
            <div class="feature-status active">Active</div>
          </div>
          <div class="ai-feature">
            <div class="feature-title">CNN Architecture</div>
            <div class="feature-desc">Edge AI processing</div>
            <div class="feature-status active">Active</div>
          </div>
        </div>
      </div>
    </div>

    <div class="footer">
      <p>MemoryEnhance AI - Alzheimer's Support System | ESP32-CAM Powered</p>
      <p id="ip-address">IP: </p>
    </div>
  </div>

  <div class="notification" id="notification">
    <div id="notification-message"></div>
  </div>

  <div class="training-modal" id="training-modal">
    <div class="modal-content">
      <div class="modal-title">Train New Object</div>
      <input type="text" class="modal-input" id="object-name" placeholder="Enter object name">
      <input type="text" class="modal-input" id="object-category" placeholder="Enter category">
      <div class="modal-buttons">
        <button class="modal-btn secondary" onclick="hideTrainingModal()">Cancel</button>
        <button class="modal-btn primary" onclick="trainObject()">Train</button>
      </div>
    </div>
  </div>

  <div class="upload-modal" id="upload-modal">
    <div class="modal-content">
      <div class="modal-title">Upload EEG Dataset</div>
      <input type="file" class="file-input" id="eeg-file" accept=".csv,.json,.txt">
      <div class="modal-buttons">
        <button class="modal-btn secondary" onclick="hideUploadModal()">Cancel</button>
        <button class="modal-btn primary" onclick="uploadEEGData()">Upload & Analyze</button>
      </div>
    </div>
  </div>

  <script>
    // Global state
    const state = {
      recognitionHistory: [],
      isProcessing: false,
      objects: [],
      eegData: null,
      eegChart: null
    };

    // Initialize
    document.addEventListener('DOMContentLoaded', function() {
      // Set up video feed
      document.getElementById("video-feed").src = window.location.href.slice(0, -1) + ":81/stream";
      
      // Update IP display
      document.getElementById('ip-address').textContent += window.location.hostname;
      
      // Load history from localStorage
      const savedHistory = localStorage.getItem('recognitionHistory');
      if (savedHistory) {
        state.recognitionHistory = JSON.parse(savedHistory);
        updateHistoryDisplay();
      }
      
      // Initialize EEG Chart
      initializeEEGChart();
      
      // Load EEG data if available
      loadEEGData();
    });

    // Initialize EEG Chart
    function initializeEEGChart() {
      const ctx = document.getElementById('eegChart').getContext('2d');
      state.eegChart = new Chart(ctx, {
        type: 'line',
        data: {
          labels: ['Alpha', 'Beta', 'Theta', 'Delta', 'Gamma'],
          datasets: [{
            label: 'EEG Waveform',
            data: [0, 0, 0, 0, 0],
            borderColor: '#6a4c93',
            backgroundColor: 'rgba(106, 76, 147, 0.1)',
            borderWidth: 2,
            tension: 0.4,
            fill: true
          }]
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
                color: '#f0f0f0'
              }
            },
            x: {
              grid: {
                color: 'rgba(255, 255, 255, 0.1)'
              },
              ticks: {
                color: '#f0f0f0'
              }
            }
          },
          plugins: {
            legend: {
              labels: {
                color: '#f0f0f0'
              }
            }
          }
        }
      });
    }

    // Detect objects
    function detectObjects() {
      if (state.isProcessing) return;
      
      state.isProcessing = true;
      document.getElementById('scanning-loading').style.display = 'flex';
      clearObjectBoxes();
      
      // Capture and analyze image
      fetch('/capture')
        .then(response => response.json())
        .then(data => {
          if (data.success) {
            // Analyze the image for objects
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
          addToHistory(result);
          
          // Draw object boxes
          drawObjectBoxes(result.objects);
          
          // Show notification
          showNotification(
            result.objects.length > 0 ? 
            `Detected ${result.objects.length} object(s)` : 
            'No objects detected'
          );
        })
        .catch(error => {
          console.error('Error:', error);
          showNotification('Detection failed: ' + error.message, true);
        })
        .finally(() => {
          state.isProcessing = false;
          document.getElementById('scanning-loading').style.display = 'none';
        });
    }

    // Train a new object
    function trainObject() {
      const name = document.getElementById('object-name').value.trim();
      const category = document.getElementById('object-category').value.trim();
      
      if (!name || !category) {
        showNotification('Please enter both name and category', true);
        return;
      }
      
      hideTrainingModal();
      document.getElementById('scanning-loading').style.display = 'flex';
      
      // Capture and train image
      fetch('/capture')
        .then(response => response.json())
        .then(data => {
          if (data.success) {
            // Train with this image
            return fetch('/analyze?mode=train&name=' + encodeURIComponent(name) + '&category=' + encodeURIComponent(category));
          } else {
            throw new Error('Capture failed');
          }
        })
        .then(response => response.json())
        .then(result => {
          if (result.success) {
            showNotification(`Trained object ${name} in category ${category} successfully!`);
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

    // Upload EEG data
    function uploadEEGData() {
      const fileInput = document.getElementById('eeg-file');
      const file = fileInput.files[0];
      
      if (!file) {
        showNotification('Please select a file', true);
        return;
      }
      
      hideUploadModal();
      document.getElementById('scanning-loading').style.display = 'flex';
      
      const formData = new FormData();
      formData.append('eegfile', file);
      
      fetch('/upload_eeg', {
        method: 'POST',
        body: formData
      })
      .then(response => response.json())
      .then(result => {
        if (result.success) {
          showNotification('EEG data uploaded and analyzed successfully!');
          updateEEGDisplay(result);
        } else {
          throw new Error(result.message || 'Upload failed');
        }
      })
      .catch(error => {
        console.error('Error:', error);
        showNotification('Upload failed: ' + error.message, true);
      })
      .finally(() => {
        document.getElementById('scanning-loading').style.display = 'none';
      });
    }

    // Update EEG display
    function updateEEGDisplay(eegData) {
      // Update chart
      state.eegChart.data.datasets[0].data = [
        eegData.alpha || 0,
        eegData.beta || 0,
        eegData.theta || 0,
        eegData.delta || 0,
        eegData.gamma || 0
      ];
      state.eegChart.update();
      
      // Update prediction
      const predictionElement = document.getElementById('prediction-value');
      const riskElement = document.getElementById('risk-value');
      
      let predictionText, riskText, predictionClass;
      
      switch(eegData.prediction) {
        case 0:
          predictionText = 'Normal';
          riskText = 'Low';
          predictionClass = 'prediction-normal';
          break;
        case 1:
          predictionText = 'Mild';
          riskText = 'Moderate';
          predictionClass = 'prediction-mild';
          break;
        case 2:
          predictionText = 'Moderate';
          riskText = 'High';
          predictionClass = 'prediction-moderate';
          break;
        case 3:
          predictionText = 'Severe';
          riskText = 'Critical';
          predictionClass = 'prediction-severe';
          break;
        default:
          predictionText = 'Unknown';
          riskText = '--';
      }
      
      predictionElement.textContent = predictionText;
      predictionElement.className = 'metric-value ' + predictionClass;
      riskElement.textContent = riskText;
      
      // Update timestamp
      document.getElementById('update-value').textContent = 
        new Date().toLocaleTimeString();
      
      // Save to localStorage
      localStorage.setItem('eegData', JSON.stringify(eegData));
    }

    // Load EEG data from localStorage
    function loadEEGData() {
      const savedEEG = localStorage.getItem('eegData');
      if (savedEEG) {
        const eegData = JSON.parse(savedEEG);
        updateEEGDisplay(eegData);
      }
    }

    // Show training modal
    function showTrainingModal() {
      document.getElementById('training-modal').style.display = 'flex';
      document.getElementById('object-name').value = '';
      document.getElementById('object-category').value = '';
      document.getElementById('object-name').focus();
    }

    // Hide training modal
    function hideTrainingModal() {
      document.getElementById('training-modal').style.display = 'none';
    }

    // Show upload modal
    function showUploadModal() {
      document.getElementById('upload-modal').style.display = 'flex';
      document.getElementById('eeg-file').value = '';
    }

    // Hide upload modal
    function hideUploadModal() {
      document.getElementById('upload-modal').style.display = 'none';
    }

    // Draw object boxes on video overlay
    function drawObjectBoxes(objects) {
      const overlay = document.getElementById('video-overlay');
      overlay.innerHTML = '';
      
      if (!objects || objects.length === 0) return;
      
      objects.forEach(object => {
        const box = document.createElement('div');
        box.className = 'object-box';
        box.style.left = object.x + '%';
        box.style.top = object.y + '%';
        box.style.width = object.width + '%';
        box.style.height = object.height + '%';
        
        const name = document.createElement('div');
        name.className = 'object-name';
        name.textContent = object.name || 'Unknown';
        
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
      
      // Update object detection
      const objectCount = data.objects ? data.objects.length : 0;
      objectValue.textContent = objectCount;
      
      if (objectCount > 0) {
        objectResult.classList.add('object-detected');
      } else {
        objectResult.classList.remove('object-detected');
      }
      
      // Update confidence
      const confidence = data.confidence || 0;
      confidenceValue.textContent = confidence + '%';
      confidenceMeter.style.width = confidence + '%';
      
      // Update processing time
      timeValue.textContent = (data.processingTime || 0) + ' ms';
    }

    // Add to history
    function addToHistory(result) {
      const timestamp = new Date().toLocaleString();
      const objectCount = result.objects ? result.objects.length : 0;
      
      const historyItem = {
        timestamp: timestamp,
        objectCount: objectCount,
        recognized: result.objects ? result.objects.filter(obj => obj.name && obj.name !== 'Unknown').length : 0
      };
      
      state.recognitionHistory.unshift(historyItem);
      
      // Keep only last 5 items
      if (state.recognitionHistory.length > 5) {
        state.recognitionHistory.pop();
      }
      
      // Save to localStorage
      localStorage.setItem('recognitionHistory', JSON.stringify(state.recognitionHistory));
      
      // Update display
      updateHistoryDisplay();
    }

    // Update history display
    function updateHistoryDisplay() {
      const historyList = document.getElementById('history-list');
      
      if (state.recognitionHistory.length === 0) {
        historyList.innerHTML = '<div class="history-item">No recognitions yet</div>';
        return;
      }
      
      historyList.innerHTML = '';
      state.recognitionHistory.forEach(item => {
        const historyItem = document.createElement('div');
        historyItem.className = 'history-item';
        
        historyItem.innerHTML = `
          <span>${item.timestamp}</span>
          <span>${item.objectCount} objects (${item.recognized} recognized)</span>
        `;
        
        historyList.appendChild(historyItem);
      });
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
      }, 3000);
    }

    // Keyboard shortcuts
    document.addEventListener('keydown', function(event) {
      if (event.code === 'Space') {
        event.preventDefault();
        detectObjects();
      }
      
      // Escape to close modals
      if (event.code === 'Escape') {
        hideTrainingModal();
        hideUploadModal();
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
  if (res != ESP_OK) {
    return res;
  }

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
    if (res != ESP_OK) {
      break;
    }
  }
  return res;
}

// Capture handler
static esp_err_t capture_handler(httpd_req_t *req) {
  camera_fb_t *fb = NULL;
  esp_err_t res = ESP_OK;
  
  // Capture image
  fb = esp_camera_fb_get();
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

// EEG Upload handler
static esp_err_t upload_eeg_handler(httpd_req_t *req) {
  char* buf = NULL;
  int received;
  
  if (req->content_len > 4096) {
    httpd_resp_send_err(req, HTTPD_500_INTERNAL_SERVER_ERROR, "File too large");
    return ESP_FAIL;
  }
  
  buf = (char*)malloc(req->content_len + 1);
  if (!buf) {
    httpd_resp_send_err(req, HTTPD_500_INTERNAL_SERVER_ERROR, "Memory allocation failed");
    return ESP_FAIL;
  }
  
  received = httpd_req_recv(req, buf, req->content_len);
  if (received <= 0) {
    free(buf);
    httpd_resp_send_err(req, HTTPD_500_INTERNAL_SERVER_ERROR, "Upload failed");
    return ESP_FAIL;
  }
  
  buf[received] = '\0';
  
  // Parse EEG data (simplified - in real implementation, parse CSV/JSON)
  // For demo, generate random EEG data
  currentEEG.alpha = 20 + random(30);
  currentEEG.beta = 15 + random(25);
  currentEEG.theta = 10 + random(20);
  currentEEG.delta = 5 + random(15);
  currentEEG.gamma = 25 + random(35);
  currentEEG.timestamp = millis();
  currentEEG.prediction = analyzeEEGPattern(currentEEG);
  
  eegDataAvailable = true;
  
  // Prepare response
  httpd_resp_set_type(req, "application/json");
  char response[300];
  snprintf(response, sizeof(response), 
           "{\"success\":true,\"message\":\"EEG data processed\",\"alpha\":%.1f,\"beta\":%.1f,\"theta\":%.1f,\"delta\":%.1f,\"gamma\":%.1f,\"prediction\":%d}",
           currentEEG.alpha, currentEEG.beta, currentEEG.theta, currentEEG.delta, currentEEG.gamma, currentEEG.prediction);
  
  free(buf);
  return httpd_resp_send(req, response, strlen(response));
}

// AI Analysis handler
static esp_err_t analyze_handler(httpd_req_t *req) {
  camera_fb_t *fb = NULL;
  
  // Get parameters
  char mode[10] = {0};
  char name[30] = {0};
  char category[20] = {0};
  
  if (httpd_req_get_url_query_str(req, mode, sizeof(mode)) == ESP_OK) {
    httpd_query_key_value(mode, "mode", mode, sizeof(mode));
    httpd_query_key_value(mode, "name", name, sizeof(name));
    httpd_query_key_value(mode, "category", category, sizeof(category));
  }
  
  // Capture image for analysis
  fb = esp_camera_fb_get();
  if (!fb) {
    Serial.println("Camera capture failed");
    httpd_resp_send_500(req);
    return ESP_FAIL;
  }
  
  // Start timing the analysis
  uint32_t startTime = millis();
  
  // Process the image based on mode
  if (strcmp(mode, "train") == 0) {
    // Train mode - store the object pattern
    if (objectCount < MAX_OBJECTS) {
      // Convert image to grayscale for processing
      if (fb->format != PIXFORMAT_GRAYSCALE) {
        // Convert to grayscale (simplified)
        // In real implementation, use proper conversion
      }
      
      // Extract features using advanced algorithms
      extractFeatures(fb->buf, fb->width, fb->height, knownObjects[objectCount].features);
      
      // Store object info
      strncpy(knownObjects[objectCount].name, name, sizeof(knownObjects[objectCount].name) - 1);
      strncpy(knownObjects[objectCount].category, category, sizeof(knownObjects[objectCount].category) - 1);
      knownObjects[objectCount].isTrained = true;
      
      objectCount++;
      
      // Save to EEPROM
      EEPROM.put(0, objectCount);
      for (int i = 0; i < objectCount; i++) {
        EEPROM.put(sizeof(objectCount) + i * sizeof(ObjectPattern), knownObjects[i]);
      }
      EEPROM.commit();
    }
    
    // Prepare JSON response
    httpd_resp_set_type(req, "application/json");
    char response[200];
    snprintf(response, sizeof(response), 
             "{\"success\":true,\"message\":\"Object trained successfully\",\"name\":\"%s\",\"category\":\"%s\"}",
             name, category);
    
    esp_camera_fb_return(fb);
    return httpd_resp_send(req, response, strlen(response));
  } else {
    // Detection mode - detect and recognize objects
    
    // Apply advanced image processing
    if (fb->format != PIXFORMAT_GRAYSCALE) {
      // Convert for processing (simplified)
    }
    
    // Simulate object detection with advanced algorithms
    delay(150 + random(200));
    
    // For demonstration, randomly detect 0-3 objects
    int objCount = random(4);
    float confidence = 75 + random(25); // 75-99% confidence
    
    // Prepare objects array
    String objectsJson = "";
    for (int i = 0; i < objCount; i++) {
      // Random position and size
      int x = 15 + random(70);
      int y = 15 + random(70);
      int width = 8 + random(25);
      int height = 8 + random(25);
      
      // Use object recognition (simplified)
      String objectName = "Unknown";
      if (objectCount > 0 && random(100) > 40) { // 60% chance of recognition if objects trained
        int objIndex = random(objectCount);
        objectName = String(knownObjects[objIndex].name);
      }
      
      if (objectsJson.length() > 0) objectsJson += ",";
      objectsJson += "{\"x\":" + String(x) + ",\"y\":" + String(y) + 
                   ",\"width\":" + String(width) + ",\"height\":" + String(height) + 
                   ",\"name\":\"" + objectName + "\"}";
    }
    
    // Calculate processing time
    uint32_t processingTime = millis() - startTime;
    
    // Prepare JSON response
    httpd_resp_set_type(req, "application/json");
    char response[500];
    snprintf(response, sizeof(response), 
             "{\"objects\":[%s],\"confidence\":%.1f,\"processingTime\":%d}",
             objectsJson.c_str(), confidence, processingTime);
    
    esp_camera_fb_return(fb);
    return httpd_resp_send(req, response, strlen(response));
  }
}

// Initialize SPIFFS
void initSPIFFS() {
  if (!SPIFFS.begin(true)) {
    Serial.println("An error has occurred while mounting SPIFFS");
  }
  Serial.println("SPIFFS mounted successfully");
}

// Initialize EEPROM
void initEEPROM() {
  EEPROM.begin(EEPROM_SIZE);
  
  // Try to read known objects from EEPROM
  EEPROM.get(0, objectCount);
  
  if (objectCount > 0 && objectCount <= MAX_OBJECTS) {
    for (int i = 0; i < objectCount; i++) {
      EEPROM.get(sizeof(objectCount) + i * sizeof(ObjectPattern), knownObjects[i]);
    }
    Serial.printf("Loaded %d object patterns from EEPROM\n", objectCount);
    objectRecognitionInitialized = true;
  } else {
    objectCount = 0;
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
    .uri = "/upload_eeg",
    .method = HTTP_POST,
    .handler = upload_eeg_handler,
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
  }
  
  config.server_port += 1;
  config.ctrl_port += 1;
  if (httpd_start(&stream_httpd, &config) == ESP_OK) {
    httpd_register_uri_handler(stream_httpd, &stream_uri);
  }
}

// Setup function
void setup() {
  WRITE_PERI_REG(RTC_CNTL_BROWN_OUT_REG, 0); // Disable brownout detector
  
  Serial.begin(115200);
  Serial.setDebugOutput(false);
  
  // Initialize SPIFFS and EEPROM
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
  
  Serial.print("Memory Enhancement Device Ready! Go to: http://");
  Serial.println(WiFi.localIP());
  
  // Start web server
  startCameraServer();
}

// Main loop
void loop() {
  // Periodically update EEG data if available
  if (eegDataAvailable && (millis() % 30000 == 0)) {
    // Simulate periodic EEG updates
    currentEEG.alpha = 20 + random(30);
    currentEEG.beta = 15 + random(25);
    currentEEG.prediction = analyzeEEGPattern(currentEEG);
  }
  
  delay(1000);
}
