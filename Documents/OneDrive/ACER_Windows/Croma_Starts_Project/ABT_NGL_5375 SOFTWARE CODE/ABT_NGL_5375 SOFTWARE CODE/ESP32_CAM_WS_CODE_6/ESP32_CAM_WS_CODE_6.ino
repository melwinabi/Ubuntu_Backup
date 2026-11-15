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
#include <WebServer.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>
#include <base64.h>

// WiFi Credentials
const char* ssid = "MA's A56";
const char* password = "Melwin@123";

// EEG Simulation Parameters
#define EEG_SAMPLE_RATE 256
#define EEG_CHANNELS 8
#define EEG_BUFFER_SIZE 512

// Object Detection Parameters
#define PART_BOUNDARY "123456789000000000000987654321"
#define IMAGE_WIDTH 160
#define IMAGE_HEIGHT 120
#define FEATURE_VECTOR_SIZE 256
#define EEPROM_SIZE 8192
#define MAX_OBJECTS 100
#define DATASET_PATH "/datasets"

// Stream constants
static const char* _STREAM_CONTENT_TYPE = "multipart/x-mixed-replace;boundary=" PART_BOUNDARY;
static const char* _STREAM_BOUNDARY = "\r\n--" PART_BOUNDARY "\r\n";
static const char* _STREAM_PART = "Content-Type: image/jpeg\r\nContent-Length: %u\r\n\r\n";

// Camera model
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

// Global variables
httpd_handle_t camera_httpd = NULL;
httpd_handle_t stream_httpd = NULL;

// EEG Data Structure
struct EEGData {
  float channels[EEG_CHANNELS];
  uint32_t timestamp;
  float attention;
  float meditation;
  float alpha;
  float beta;
  float theta;
  float delta;
};

// Enhanced Object Pattern Structure
struct ObjectPattern {
  uint8_t features[FEATURE_VECTOR_SIZE];
  char name[32];
  char category[32];
  uint16_t checksum;
  uint32_t timestamp;
  uint8_t sampleCount;
  float confidenceThreshold;
  uint32_t lastDetected;
};

// Training Session Structure
struct TrainingSession {
  char objectName[32];
  char category[32];
  uint8_t samplesCollected;
  uint8_t targetSamples;
  uint32_t startTime;
  bool isActive;
};

// System State
enum SystemMode {
  MODE_TRAIN,
  MODE_ADVANCED_TRAIN,
  MODE_TEST,
  MODE_ADVANCED_TEST,
  MODE_EEG_MONITOR,
  MODE_OBJECT_IDENTIFICATION,
  MODE_DATASET_MANAGEMENT
};

// Global variables
EEGData currentEEG;
ObjectPattern objectDatabase[MAX_OBJECTS];
int objectCount = 0;
SystemMode currentMode = MODE_OBJECT_IDENTIFICATION;
bool eegConnected = false;
unsigned long lastEEGUpdate = 0;
unsigned long lastObjectDetection = 0;
TrainingSession currentTraining;

// Predefined object categories
const char* objectCategories[] = {
  "Personal Items", "Household Items", "Medical Items", "Food Items", "Emergency Items", "Clothing", "Electronics"
};

const char* commonObjects[] = {
  "Keys", "Wallet", "Phone", "Glasses", "Medication", "Water Bottle", 
  "Remote", "Book", "Newspaper", "Food Container", "Cup", "Plate",
  "Shoes", "Watch", "Headphones", "Laptop", "Tablet", "Charger"
};

// Function Declarations
uint16_t calculateChecksum(uint8_t* data, size_t length);
void saveObjectDatabase();
void loadObjectDatabase();
void initSPIFFS();
void initEEPROM();
void startCameraServer();
void extractFeatures(camera_fb_t* fb, uint8_t* features);
float compareFeatures(uint8_t* features1, uint8_t* features2);
bool saveDatasetImage(const char* objectName, camera_fb_t* fb);
void loadDatasetImages();
String getFileList();
void deleteDataset(const char* objectName);

// HTML Web Page with Enhanced Features
static const char PROGMEM INDEX_HTML[] = R"rawliteral(
<!DOCTYPE html>
<html lang="en">
<head>
  <meta charset="UTF-8">
  <meta name="viewport" content="width=device-width, initial-scale=1.0">
  <title>Advanced Memory Enhancement Device | Alzheimer's Support System</title>
  <link rel="stylesheet" href="https://cdnjs.cloudflare.com/ajax/libs/font-awesome/6.4.0/css/all.min.css">
  <link href="https://fonts.googleapis.com/css2?family=Montserrat:wght@300;400;600;800&family=Roboto+Mono:wght@400;700&display=swap" rel="stylesheet">
  <script src="https://cdn.jsdelivr.net/npm/chart.js"></script>
  <style>
    :root {
      --primary: #2c7873;
      --primary-dark: #004445;
      --secondary: #6fb98f;
      --accent: #ff9a76;
      --warning: #ffcc00;
      --danger: #ff5252;
      --info: #4a90e2;
      --dark: #1a2a3a;
      --darker: #0d1a26;
      --light: #f0f7ff;
      --overlay: rgba(26, 42, 58, 0.95);
      --glass: rgba(255, 255, 255, 0.08);
      --glass-border: rgba(255, 255, 255, 0.12);
    }

    * {
      box-sizing: border-box;
      margin: 0;
      padding: 0;
    }

    body {
      font-family: 'Montserrat', sans-serif;
      background: linear-gradient(135deg, var(--dark) 0%, var(--primary-dark) 100%);
      color: var(--light);
      min-height: 100vh;
      overflow-x: hidden;
    }

    .container {
      max-width: 1600px;
      margin: 0 auto;
      padding: 1rem;
    }

    .header {
      background: var(--overlay);
      backdrop-filter: blur(10px);
      border-radius: 16px;
      padding: 1.5rem;
      margin-bottom: 1.5rem;
      border: 1px solid var(--glass-border);
      display: flex;
      justify-content: space-between;
      align-items: center;
    }

    .app-title {
      font-family: 'Roboto Mono', monospace;
      font-size: 2rem;
      font-weight: 800;
      color: var(--secondary);
    }

    .app-subtitle {
      font-size: 1rem;
      opacity: 0.8;
      margin-top: 0.25rem;
    }

    .status-panel {
      display: flex;
      gap: 1rem;
      align-items: center;
      flex-wrap: wrap;
    }

    .status-item {
      display: flex;
      align-items: center;
      gap: 0.5rem;
      padding: 0.5rem 1rem;
      background: var(--glass);
      border-radius: 8px;
      border: 1px solid var(--glass-border);
      font-size: 0.9rem;
    }

    .status-indicator {
      width: 12px;
      height: 12px;
      border-radius: 50%;
    }

    .connected { background: var(--secondary); animation: pulse 2s infinite; }
    .disconnected { background: var(--danger); }
    .warning { background: var(--warning); }
    .training { background: var(--info); }

    @keyframes pulse {
      0% { opacity: 1; }
      50% { opacity: 0.6; }
      100% { opacity: 1; }
    }

    .main-dashboard {
      display: grid;
      grid-template-columns: 1fr 1fr;
      gap: 1.5rem;
      margin-bottom: 1.5rem;
    }

    @media (max-width: 1200px) {
      .main-dashboard {
        grid-template-columns: 1fr;
      }
    }

    .panel {
      background: var(--overlay);
      border-radius: 16px;
      padding: 1.5rem;
      backdrop-filter: blur(5px);
      border: 1px solid var(--glass-border);
      box-shadow: 0 8px 32px rgba(0, 0, 0, 0.3);
    }

    .panel-title {
      font-size: 1.3rem;
      font-weight: 600;
      margin-bottom: 1.5rem;
      color: var(--secondary);
      display: flex;
      align-items: center;
      gap: 0.75rem;
    }

    .video-container {
      position: relative;
      border-radius: 12px;
      overflow: hidden;
      background: var(--darker);
      aspect-ratio: 4/3;
      margin-bottom: 1rem;
    }

    #video-feed {
      width: 100%;
      height: 100%;
      object-fit: cover;
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
      border: 3px solid var(--secondary);
      border-radius: 8px;
      background: rgba(111, 185, 143, 0.15);
      display: flex;
      flex-direction: column;
      align-items: center;
      padding: 0.75rem;
      transform: translate(-50%, -50%);
      box-shadow: 0 4px 12px rgba(0, 0, 0, 0.3);
    }

    .object-name {
      color: white;
      font-size: 0.9rem;
      font-weight: 700;
      margin-top: 0.5rem;
      text-shadow: 2px 2px 4px rgba(0, 0, 0, 0.8);
      background: rgba(0, 0, 0, 0.7);
      padding: 0.25rem 0.75rem;
      border-radius: 12px;
    }

    .advanced-controls {
      display: grid;
      grid-template-columns: repeat(auto-fit, minmax(150px, 1fr));
      gap: 0.75rem;
      margin: 1rem 0;
    }

    .control-btn {
      padding: 1rem;
      background: var(--glass);
      border: 2px solid var(--glass-border);
      border-radius: 10px;
      color: var(--light);
      font-size: 0.9rem;
      font-weight: 600;
      cursor: pointer;
      transition: all 0.3s ease;
      display: flex;
      align-items: center;
      justify-content: center;
      gap: 0.5rem;
      text-align: center;
    }

    .control-btn:hover {
      transform: translateY(-2px);
      box-shadow: 0 6px 20px rgba(0, 0, 0, 0.3);
    }

    .control-btn.primary {
      background: var(--primary);
      border-color: var(--secondary);
    }

    .control-btn.warning {
      background: var(--warning);
      border-color: var(--warning);
      color: var(--dark);
    }

    .control-btn.danger {
      background: var(--danger);
      border-color: var(--danger);
    }

    .control-btn.info {
      background: var(--info);
      border-color: var(--info);
    }

    .training-progress {
      background: var(--darker);
      border-radius: 10px;
      padding: 1rem;
      margin: 1rem 0;
      display: none;
    }

    .progress-bar {
      width: 100%;
      height: 20px;
      background: var(--glass);
      border-radius: 10px;
      overflow: hidden;
      margin: 0.5rem 0;
    }

    .progress-fill {
      height: 100%;
      background: linear-gradient(90deg, var(--primary), var(--secondary));
      border-radius: 10px;
      transition: width 0.5s ease;
      width: 0%;
    }

    .charts-container {
      display: grid;
      grid-template-columns: 1fr 1fr;
      gap: 1.5rem;
      margin-bottom: 1.5rem;
    }

    @media (max-width: 900px) {
      .charts-container {
        grid-template-columns: 1fr;
      }
    }

    .chart-panel {
      background: var(--darker);
      border-radius: 12px;
      padding: 1.5rem;
      height: 300px;
    }

    .chart-title {
      font-size: 1.1rem;
      font-weight: 600;
      margin-bottom: 1rem;
      color: var(--secondary);
    }

    .results-grid {
      display: grid;
      grid-template-columns: repeat(auto-fit, minmax(200px, 1fr));
      gap: 1rem;
      margin-top: 1.5rem;
    }

    .result-card {
      background: var(--darker);
      border-radius: 12px;
      padding: 1.5rem;
      border-left: 5px solid var(--primary);
    }

    .result-card.warning {
      border-left-color: var(--warning);
    }

    .result-card.danger {
      border-left-color: var(--danger);
    }

    .result-card.info {
      border-left-color: var(--info);
    }

    .result-title {
      font-size: 0.9rem;
      font-weight: 600;
      margin-bottom: 0.75rem;
      color: var(--light);
    }

    .result-value {
      font-size: 1.8rem;
      font-weight: 800;
      color: var(--secondary);
    }

    .result-card.warning .result-value {
      color: var(--warning);
    }

    .result-card.danger .result-value {
      color: var(--danger);
    }

    .result-card.info .result-value {
      color: var(--info);
    }

    .result-unit {
      font-size: 0.9rem;
      opacity: 0.8;
      margin-left: 0.5rem;
    }

    .dataset-management {
      background: var(--darker);
      border-radius: 12px;
      padding: 1.5rem;
      margin-top: 1.5rem;
    }

    .dataset-list {
      max-height: 300px;
      overflow-y: auto;
      margin: 1rem 0;
    }

    .dataset-item {
      display: flex;
      justify-content: space-between;
      align-items: center;
      padding: 1rem;
      background: var(--glass);
      border-radius: 8px;
      margin-bottom: 0.5rem;
    }

    .dataset-info {
      display: flex;
      flex-direction: column;
      gap: 0.25rem;
    }

    .dataset-name {
      font-weight: 600;
      color: var(--secondary);
    }

    .dataset-stats {
      font-size: 0.8rem;
      opacity: 0.8;
    }

    .file-upload {
      border: 2px dashed var(--glass-border);
      border-radius: 12px;
      padding: 2rem;
      text-align: center;
      margin: 1rem 0;
      cursor: pointer;
      transition: all 0.3s ease;
    }

    .file-upload:hover {
      border-color: var(--secondary);
      background: rgba(111, 185, 143, 0.1);
    }

    .file-upload input {
      display: none;
    }

    .history-panel {
      margin-top: 1.5rem;
    }

    .history-list {
      display: flex;
      flex-direction: column;
      gap: 0.75rem;
      max-height: 300px;
      overflow-y: auto;
    }

    .history-item {
      display: flex;
      justify-content: space-between;
      align-items: center;
      padding: 1rem;
      background: var(--darker);
      border-radius: 8px;
      border-left: 4px solid var(--primary);
    }

    .history-item.warning {
      border-left-color: var(--warning);
    }

    .history-item.danger {
      border-left-color: var(--danger);
    }

    .history-item.info {
      border-left-color: var(--info);
    }

    .history-time {
      font-size: 0.8rem;
      opacity: 0.8;
    }

    .history-content {
      display: flex;
      flex-direction: column;
      gap: 0.25rem;
    }

    .history-object {
      font-weight: 600;
      color: var(--secondary);
    }

    .history-eeg {
      font-size: 0.8rem;
      opacity: 0.9;
    }

    .modal {
      position: fixed;
      top: 0;
      left: 0;
      width: 100%;
      height: 100%;
      background: rgba(0, 0, 0, 0.8);
      display: flex;
      justify-content: center;
      align-items: center;
      z-index: 1000;
      display: none;
    }

    .modal-content {
      background: var(--dark);
      border-radius: 16px;
      padding: 2rem;
      width: 90%;
      max-width: 500px;
      border: 1px solid var(--glass-border);
      max-height: 90vh;
      overflow-y: auto;
    }

    .modal-title {
      font-size: 1.5rem;
      font-weight: 700;
      margin-bottom: 1.5rem;
      color: var(--secondary);
    }

    .form-group {
      margin-bottom: 1.5rem;
    }

    .form-label {
      display: block;
      margin-bottom: 0.5rem;
      font-weight: 600;
    }

    .form-input, .form-select, .form-textarea {
      width: 100%;
      padding: 1rem;
      background: var(--darker);
      border: 1px solid var(--glass-border);
      border-radius: 8px;
      color: var(--light);
      font-size: 1rem;
    }

    .form-textarea {
      height: 100px;
      resize: vertical;
    }

    .form-buttons {
      display: flex;
      gap: 1rem;
      justify-content: flex-end;
      margin-top: 1.5rem;
    }

    .btn {
      padding: 1rem 2rem;
      border: none;
      border-radius: 8px;
      font-weight: 600;
      cursor: pointer;
      transition: all 0.3s ease;
    }

    .btn-primary {
      background: var(--primary);
      color: white;
    }

    .btn-secondary {
      background: var(--glass);
      color: var(--light);
      border: 1px solid var(--glass-border);
    }

    .btn-warning {
      background: var(--warning);
      color: var(--dark);
    }

    .btn-danger {
      background: var(--danger);
      color: white;
    }

    .btn:hover {
      transform: translateY(-2px);
      box-shadow: 0 4px 12px rgba(0, 0, 0, 0.3);
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
      width: 50px;
      height: 50px;
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
      padding: 1.25rem;
      background: var(--darker);
      border-left: 5px solid var(--secondary);
      border-radius: 8px;
      box-shadow: 0 8px 25px rgba(0, 0, 0, 0.4);
      transform: translateX(100%);
      transition: transform 0.4s ease;
      z-index: 1000;
      max-width: 400px;
    }

    .notification.show {
      transform: translateX(0);
    }

    .notification.warning {
      border-left-color: var(--warning);
    }

    .notification.danger {
      border-left-color: var(--danger);
    }

    .notification.info {
      border-left-color: var(--info);
    }

    .footer {
      text-align: center;
      margin-top: 2rem;
      padding: 1.5rem;
      font-size: 0.9rem;
      opacity: 0.7;
      border-top: 1px solid var(--glass-border);
    }

    .tab-container {
      margin: 1rem 0;
    }

    .tabs {
      display: flex;
      gap: 0.5rem;
      margin-bottom: 1rem;
      flex-wrap: wrap;
    }

    .tab {
      padding: 0.75rem 1.5rem;
      background: var(--glass);
      border: none;
      border-radius: 8px;
      color: var(--light);
      cursor: pointer;
      transition: all 0.3s ease;
    }

    .tab.active {
      background: var(--primary);
      color: white;
    }

    .tab-content {
      display: none;
    }

    .tab-content.active {
      display: block;
    }
  </style>
</head>
<body>
  <div class="container">
    <div class="header">
      <div>
        <div class="app-title">Advanced Memory Enhancement Device</div>
        <div class="app-subtitle">AI-Powered Alzheimer's Support System with Advanced Object Recognition</div>
      </div>
      <div class="status-panel">
        <div class="status-item">
          <div class="status-indicator connected" id="eeg-status"></div>
          <span>EEG: <span id="eeg-status-text">Connected</span></span>
        </div>
        <div class="status-item">
          <div class="status-indicator connected" id="camera-status"></div>
          <span>Camera: <span id="camera-status-text">Active</span></span>
        </div>
        <div class="status-item">
          <div class="status-indicator connected" id="ai-status"></div>
          <span>AI: <span id="ai-status-text">Online</span></span>
        </div>
        <div class="status-item">
          <div class="status-indicator" id="training-status"></div>
          <span>Training: <span id="training-status-text">Ready</span></span>
        </div>
      </div>
    </div>

    <div class="main-dashboard">
      <!-- Left Column -->
      <div class="panel">
        <div class="panel-title">
          <i class="fas fa-camera"></i> Advanced Object Recognition
        </div>
        <div class="video-container">
          <img id="video-feed" src="">
          <div class="video-overlay" id="video-overlay"></div>
          <div class="loading" id="processing-loading">
            <div class="spinner"></div>
            <div id="loading-text">Processing Object Recognition...</div>
          </div>
        </div>
        
        <div class="tab-container">
          <div class="tabs">
            <button class="tab active" onclick="showTab('basic-controls')">Basic</button>
            <button class="tab" onclick="showTab('advanced-controls')">Advanced</button>
            <button class="tab" onclick="showTab('dataset-controls')">Dataset</button>
          </div>
          
          <div id="basic-controls" class="tab-content active">
            <div class="advanced-controls">
              <button class="control-btn primary" onclick="setMode('object')">
                <i class="fas fa-search"></i> Object ID
              </button>
              <button class="control-btn info" onclick="setMode('train')">
                <i class="fas fa-graduation-cap"></i> Quick Train
              </button>
              <button class="control-btn" onclick="setMode('eeg')">
                <i class="fas fa-brain"></i> EEG Monitor
              </button>
              <button class="control-btn" onclick="setMode('test')">
                <i class="fas fa-vial"></i> Test Mode
              </button>
            </div>
          </div>
          
          <div id="advanced-controls" class="tab-content">
            <div class="advanced-controls">
              <button class="control-btn warning" onclick="startAdvancedTraining()">
                <i class="fas fa-brain"></i> Adv Train
              </button>
              <button class="control-btn info" onclick="setMode('advanced_test')">
                <i class="fas fa-microscope"></i> Adv Test
              </button>
              <button class="control-btn primary" onclick="captureTrainingSample()">
                <i class="fas fa-camera"></i> Capture
              </button>
              <button class="control-btn" onclick="clearObjects()">
                <i class="fas fa-trash"></i> Clear
              </button>
            </div>
          </div>
          
          <div id="dataset-controls" class="tab-content">
            <div class="advanced-controls">
              <button class="control-btn primary" onclick="showDatasetModal()">
                <i class="fas fa-upload"></i> Upload Dataset
              </button>
              <button class="control-btn info" onclick="loadDatasetList()">
                <i class="fas fa-sync"></i> Refresh
              </button>
              <button class="control-btn warning" onclick="showImportModal()">
                <i class="fas fa-file-import"></i> Import
              </button>
              <button class="control-btn danger" onclick="deleteAllDatasets()">
                <i class="fas fa-trash-alt"></i> Clear All
              </button>
            </div>
          </div>
        </div>

        <div class="training-progress" id="training-progress">
          <div class="result-title">Training Progress</div>
          <div id="training-object">Object: --</div>
          <div class="progress-bar">
            <div class="progress-fill" id="training-progress-bar"></div>
          </div>
          <div id="training-samples">Samples: 0/0</div>
        </div>

        <div class="results-grid">
          <div class="result-card">
            <div class="result-title">Objects Detected</div>
            <div class="result-value" id="object-count">0</div>
          </div>
          <div class="result-card">
            <div class="result-title">Recognition Confidence</div>
            <div class="result-value" id="confidence-value">--<span class="result-unit">%</span></div>
          </div>
          <div class="result-card">
            <div class="result-title">EEG Attention</div>
            <div class="result-value" id="attention-value">--<span class="result-unit">%</span></div>
          </div>
          <div class="result-card">
            <div class="result-title">Processing Time</div>
            <div class="result-value" id="processing-time">--<span class="result-unit">ms</span></div>
          </div>
        </div>
      </div>

      <!-- Right Column -->
      <div class="panel">
        <div class="panel-title">
          <i class="fas fa-brain"></i> EEG Brain Wave Monitoring
        </div>
        
        <div class="charts-container">
          <div class="chart-panel">
            <div class="chart-title">Brain Wave Patterns</div>
            <canvas id="eegChart"></canvas>
          </div>
          <div class="chart-panel">
            <div class="chart-title">Cognitive Metrics</div>
            <canvas id="metricsChart"></canvas>
          </div>
        </div>

        <div class="panel-title">
          <i class="fas fa-database"></i> Dataset Management
        </div>
        <div class="dataset-management">
          <div class="dataset-list" id="dataset-list">
            <div class="dataset-item">
              <div class="dataset-info">
                <div class="dataset-name">No datasets available</div>
                <div class="dataset-stats">Upload or train new objects</div>
              </div>
              <button class="control-btn danger" disabled>
                <i class="fas fa-trash"></i>
              </button>
            </div>
          </div>
          
          <div class="file-upload" onclick="document.getElementById('file-input').click()">
            <i class="fas fa-cloud-upload-alt" style="font-size: 2rem; margin-bottom: 1rem;"></i>
            <div>Click to upload dataset (ZIP/JPG)</div>
            <div style="font-size: 0.8rem; opacity: 0.7; margin-top: 0.5rem;">
              Supports: ZIP archives, JPG images
            </div>
          </div>
          <input type="file" id="file-input" accept=".zip,.jpg,.jpeg" onchange="handleFileUpload(this.files)" multiple>
        </div>

        <div class="panel-title" style="margin-top: 1.5rem;">
          <i class="fas fa-history"></i> Recent Activity History
        </div>
        <div class="history-list" id="activity-history">
          <div class="history-item">
            <div class="history-content">
              <div class="history-object">System Initialized</div>
              <div class="history-eeg">Advanced mode activated</div>
            </div>
            <div class="history-time" id="current-time">Just now</div>
          </div>
        </div>
      </div>
    </div>
  </div>

  <!-- Training Modal -->
  <div class="modal" id="training-modal">
    <div class="modal-content">
      <div class="modal-title">Advanced Object Training</div>
      <div class="form-group">
        <label class="form-label">Object Name</label>
        <input type="text" class="form-input" id="object-name" placeholder="Enter object name">
      </div>
      <div class="form-group">
        <label class="form-label">Category</label>
        <select class="form-select" id="object-category">
          <option value="Personal Items">Personal Items</option>
          <option value="Household Items">Household Items</option>
          <option value="Medical Items">Medical Items</option>
          <option value="Food Items">Food Items</option>
          <option value="Emergency Items">Emergency Items</option>
          <option value="Clothing">Clothing</option>
          <option value="Electronics">Electronics</option>
        </select>
      </div>
      <div class="form-group">
        <label class="form-label">Training Samples</label>
        <select class="form-select" id="training-samples-count">
          <option value="5">5 Samples (Quick)</option>
          <option value="10">10 Samples (Standard)</option>
          <option value="20">20 Samples (Advanced)</option>
          <option value="30">30 Samples (Comprehensive)</option>
        </select>
      </div>
      <div class="form-group">
        <label class="form-label">Confidence Threshold (%)</label>
        <input type="number" class="form-input" id="confidence-threshold" value="70" min="50" max="95">
      </div>
      <div class="form-buttons">
        <button class="btn btn-secondary" onclick="hideTrainingModal()">Cancel</button>
        <button class="btn btn-primary" onclick="startAdvancedTrainingFromModal()">Start Training</button>
      </div>
    </div>
  </div>

  <!-- Dataset Upload Modal -->
  <div class="modal" id="dataset-modal">
    <div class="modal-content">
      <div class="modal-title">Upload Dataset</div>
      <div class="form-group">
        <label class="form-label">Dataset Name</label>
        <input type="text" class="form-input" id="dataset-name" placeholder="Enter dataset name">
      </div>
      <div class="form-group">
        <label class="form-label">Object Category</label>
        <select class="form-select" id="dataset-category">
          <option value="Personal Items">Personal Items</option>
          <option value="Household Items">Household Items</option>
          <option value="Medical Items">Medical Items</option>
          <option value="Food Items">Food Items</option>
          <option value="Emergency Items">Emergency Items</option>
          <option value="Clothing">Clothing</option>
          <option value="Electronics">Electronics</option>
        </select>
      </div>
      <div class="form-group">
        <label class="form-label">Dataset Description</label>
        <textarea class="form-textarea" id="dataset-description" placeholder="Describe the dataset..."></textarea>
      </div>
      <div class="form-buttons">
        <button class="btn btn-secondary" onclick="hideDatasetModal()">Cancel</button>
        <button class="btn btn-primary" onclick="uploadDataset()">Upload Dataset</button>
      </div>
    </div>
  </div>

  <!-- Import Modal -->
  <div class="modal" id="import-modal">
    <div class="modal-content">
      <div class="modal-title">Import Training Data</div>
      <div class="form-group">
        <label class="form-label">Select Object to Import</label>
        <select class="form-select" id="import-object">
          <option value="">-- Select Object --</option>
        </select>
      </div>
      <div class="form-group">
        <label class="form-label">Import Method</label>
        <select class="form-select" id="import-method">
          <option value="replace">Replace Existing</option>
          <option value="augment">Augment Existing</option>
          <option value="merge">Merge with Existing</option>
        </select>
      </div>
      <div class="form-buttons">
        <button class="btn btn-secondary" onclick="hideImportModal()">Cancel</button>
        <button class="btn btn-primary" onclick="importDataset()">Import Data</button>
      </div>
    </div>
  </div>

  <!-- Notification -->
  <div class="notification" id="notification">
    <div id="notification-message"></div>
  </div>

  <div class="footer">
    <p>Advanced AI-Integrated Memory Enhancement Device | Alzheimer's Patient Support System</p>
    <p id="ip-address">Device IP: </p>
  </div>

  <script>
    // Enhanced Global state
    const state = {
      currentMode: 'object',
      eegData: {
        attention: 0,
        meditation: 0,
        alpha: 0,
        beta: 0,
        theta: 0,
        delta: 0
      },
      objects: [],
      activityHistory: [],
      datasets: [],
      charts: {},
      isProcessing: false,
      isTraining: false,
      trainingProgress: 0,
      currentTraining: null
    };

    // Initialize when page loads
    document.addEventListener('DOMContentLoaded', function() {
      initializeVideoFeed();
      initializeCharts();
      startEEGMonitoring();
      startObjectDetection();
      updateCurrentTime();
      loadDatasetList();
      
      // Update time every second
      setInterval(updateCurrentTime, 1000);
      
      // Load saved history
      loadActivityHistory();
    });

    // Show tab content
    function showTab(tabId) {
      // Hide all tab contents
      document.querySelectorAll('.tab-content').forEach(tab => {
        tab.classList.remove('active');
      });
      
      // Show selected tab content
      document.getElementById(tabId).classList.add('active');
      
      // Update tab buttons
      document.querySelectorAll('.tab').forEach(tab => {
        tab.classList.remove('active');
      });
      event.target.classList.add('active');
    }

    // Set operation mode
    function setMode(mode) {
      state.currentMode = mode;
      
      // Update UI based on mode
      updateModeDisplay();
      
      showNotification(`Mode switched to ${mode.toUpperCase()} mode`, 'info');
      addToHistory(`Switched to ${mode} mode`, 'system');
    }

    // Update mode display
    function updateModeDisplay() {
      const statusText = document.getElementById('training-status-text');
      const statusIndicator = document.getElementById('training-status');
      
      if (state.currentMode.includes('train')) {
        statusText.textContent = 'Training';
        statusIndicator.className = 'status-indicator training';
      } else {
        statusText.textContent = 'Ready';
        statusIndicator.className = 'status-indicator connected';
      }
    }

    // Initialize video feed
    function initializeVideoFeed() {
      const videoFeed = document.getElementById('video-feed');
      videoFeed.src = window.location.href.slice(0, -1) + ":81/stream";
      document.getElementById('ip-address').textContent += window.location.hostname;
    }

    // Initialize charts
    function initializeCharts() {
      // EEG Chart
      const eegCtx = document.getElementById('eegChart').getContext('2d');
      state.charts.eeg = new Chart(eegCtx, {
        type: 'line',
        data: {
          labels: Array.from({length: 20}, (_, i) => i + 1),
          datasets: [
            {
              label: 'Alpha',
              data: [],
              borderColor: '#6fb98f',
              backgroundColor: 'rgba(111, 185, 143, 0.1)',
              tension: 0.4
            },
            {
              label: 'Beta',
              data: [],
              borderColor: '#ff9a76',
              backgroundColor: 'rgba(255, 154, 118, 0.1)',
              tension: 0.4
            },
            {
              label: 'Theta',
              data: [],
              borderColor: '#ffcc00',
              backgroundColor: 'rgba(255, 204, 0, 0.1)',
              tension: 0.4
            },
            {
              label: 'Delta',
              data: [],
              borderColor: '#2c7873',
              backgroundColor: 'rgba(44, 120, 115, 0.1)',
              tension: 0.4
            }
          ]
        },
        options: {
          responsive: true,
          maintainAspectRatio: false,
          scales: {
            y: {
              beginAtZero: true,
              max: 100
            }
          }
        }
      });

      // Metrics Chart
      const metricsCtx = document.getElementById('metricsChart').getContext('2d');
      state.charts.metrics = new Chart(metricsCtx, {
        type: 'doughnut',
        data: {
          labels: ['Attention', 'Meditation', 'Cognitive Load'],
          datasets: [{
            data: [0, 0, 0],
            backgroundColor: ['#6fb98f', '#2c7873', '#ff9a76']
          }]
        },
        options: {
          responsive: true,
          maintainAspectRatio: false,
          cutout: '70%'
        }
      });
    }

    // Start EEG monitoring
    function startEEGMonitoring() {
      setInterval(fetchEEGData, 1000);
    }

    // Fetch EEG data from server
    function fetchEEGData() {
      fetch('/eeg-data')
        .then(response => response.json())
        .then(data => {
          if (data.success) {
            updateEEGDisplay(data);
            updateCharts(data);
            
            // Check for alerts
            checkEEGAlerts(data);
          }
        })
        .catch(error => {
          console.error('EEG data fetch error:', error);
        });
    }

    // Update EEG display
    function updateEEGDisplay(data) {
      document.getElementById('attention-value').textContent = Math.round(data.attention);
      state.eegData = data;
    }

    // Update charts with new data
    function updateCharts(data) {
      // Update EEG chart
      state.charts.eeg.data.datasets[0].data.push(data.alpha);
      state.charts.eeg.data.datasets[1].data.push(data.beta);
      state.charts.eeg.data.datasets[2].data.push(data.theta);
      state.charts.eeg.data.datasets[3].data.push(data.delta);
      
      // Keep only last 20 data points
      state.charts.eeg.data.datasets.forEach(dataset => {
        if (dataset.data.length > 20) {
          dataset.data.shift();
        }
      });
      
      state.charts.eeg.update();
      
      // Update metrics chart
      state.charts.metrics.data.datasets[0].data = [
        data.attention,
        data.meditation,
        100 - (data.attention + data.meditation) / 2
      ];
      state.charts.metrics.update();
    }

    // Start object detection
    function startObjectDetection() {
      setInterval(detectObjects, 2000);
    }

    // Detect objects
    function detectObjects() {
      if (state.isProcessing || !state.currentMode.includes('object') && !state.currentMode.includes('test')) return;
      
      state.isProcessing = true;
      document.getElementById('processing-loading').style.display = 'flex';
      
      const endpoint = state.currentMode.includes('advanced') ? '/advanced-detect' : '/detect-objects';
      
      fetch(endpoint)
        .then(response => response.json())
        .then(data => {
          if (data.success) {
            updateObjectDisplay(data);
            drawObjectBoxes(data.objects);
            
            // Add to history if objects detected
            if (data.objects && data.objects.length > 0) {
              data.objects.forEach(obj => {
                addToHistory(`Detected: ${obj.name} (${Math.round(obj.confidence)}%)`, 'object');
              });
            }
          }
        })
        .catch(error => {
          console.error('Object detection error:', error);
        })
        .finally(() => {
          state.isProcessing = false;
          document.getElementById('processing-loading').style.display = 'none';
        });
    }

    // Update object detection display
    function updateObjectDisplay(data) {
      document.getElementById('object-count').textContent = data.objects ? data.objects.length : 0;
      document.getElementById('confidence-value').textContent = data.confidence ? Math.round(data.confidence) : '--';
      document.getElementById('processing-time').textContent = data.processingTime || '--';
    }

    // Draw object boxes on video
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
        name.textContent = `${obj.name} (${Math.round(obj.confidence)}%)`;
        
        box.appendChild(name);
        overlay.appendChild(box);
      });
    }

    // Advanced Training Functions
    function startAdvancedTraining() {
      showTrainingModal();
    }

    function startAdvancedTrainingFromModal() {
      const name = document.getElementById('object-name').value.trim();
      const category = document.getElementById('object-category').value;
      const samples = parseInt(document.getElementById('training-samples-count').value);
      const confidence = parseInt(document.getElementById('confidence-threshold').value);
      
      if (!name) {
        showNotification('Please enter an object name', 'warning');
        return;
      }
      
      hideTrainingModal();
      
      // Start advanced training
      fetch('/start-advanced-training', {
        method: 'POST',
        headers: {
          'Content-Type': 'application/json',
        },
        body: JSON.stringify({
          name: name,
          category: category,
          samples: samples,
          confidence: confidence
        })
      })
      .then(response => response.json())
      .then(data => {
        if (data.success) {
          showNotification(`Advanced training started for "${name}"`, 'info');
          addToHistory(`Started advanced training: ${name}`, 'training');
          startTrainingProgress(data.session);
        } else {
          throw new Error(data.message);
        }
      })
      .catch(error => {
        showNotification('Training failed: ' + error.message, 'danger');
      });
    }

    function startTrainingProgress(session) {
      state.isTraining = true;
      state.currentTraining = session;
      
      const progressDiv = document.getElementById('training-progress');
      const progressBar = document.getElementById('training-progress-bar');
      const objectName = document.getElementById('training-object');
      const samplesText = document.getElementById('training-samples');
      
      progressDiv.style.display = 'block';
      objectName.textContent = `Object: ${session.objectName}`;
      
      // Update progress periodically
      const progressInterval = setInterval(() => {
        fetch('/training-progress')
          .then(response => response.json())
          .then(data => {
            if (data.success) {
              const progress = (data.samplesCollected / data.targetSamples) * 100;
              progressBar.style.width = progress + '%';
              samplesText.textContent = `Samples: ${data.samplesCollected}/${data.targetSamples}`;
              
              if (data.samplesCollected >= data.targetSamples) {
                clearInterval(progressInterval);
                state.isTraining = false;
                showNotification(`Training completed for ${session.objectName}`, 'info');
                addToHistory(`Training completed: ${session.objectName}`, 'training');
                setTimeout(() => {
                  progressDiv.style.display = 'none';
                }, 3000);
              }
            }
          })
          .catch(error => {
            console.error('Progress fetch error:', error);
          });
      }, 1000);
    }

    function captureTrainingSample() {
      if (!state.isTraining) {
        showNotification('No active training session', 'warning');
        return;
      }
      
      fetch('/capture-sample', {
        method: 'POST'
      })
      .then(response => response.json())
      .then(data => {
        if (data.success) {
          showNotification(`Sample captured (${data.currentSample}/${data.targetSamples})`, 'info');
        } else {
          throw new Error(data.message);
        }
      })
      .catch(error => {
        showNotification('Sample capture failed: ' + error.message, 'danger');
      });
    }

    // Dataset Management Functions
    function showDatasetModal() {
      document.getElementById('dataset-modal').style.display = 'flex';
    }

    function hideDatasetModal() {
      document.getElementById('dataset-modal').style.display = 'none';
    }

    function showImportModal() {
      document.getElementById('import-modal').style.display = 'flex';
      loadImportObjects();
    }

    function hideImportModal() {
      document.getElementById('import-modal').style.display = 'none';
    }

    function loadDatasetList() {
      fetch('/list-datasets')
        .then(response => response.json())
        .then(data => {
          if (data.success) {
            state.datasets = data.datasets;
            updateDatasetList();
          }
        })
        .catch(error => {
          console.error('Dataset load error:', error);
        });
    }

    function updateDatasetList() {
      const datasetList = document.getElementById('dataset-list');
      datasetList.innerHTML = '';
      
      if (state.datasets.length === 0) {
        datasetList.innerHTML = `
          <div class="dataset-item">
            <div class="dataset-info">
              <div class="dataset-name">No datasets available</div>
              <div class="dataset-stats">Upload or train new objects</div>
            </div>
            <button class="control-btn danger" disabled>
              <i class="fas fa-trash"></i>
            </button>
          </div>
        `;
        return;
      }
      
      state.datasets.forEach(dataset => {
        const datasetItem = document.createElement('div');
        datasetItem.className = 'dataset-item';
        datasetItem.innerHTML = `
          <div class="dataset-info">
            <div class="dataset-name">${dataset.name}</div>
            <div class="dataset-stats">${dataset.samples} samples | ${dataset.category} | ${new Date(dataset.timestamp).toLocaleDateString()}</div>
          </div>
          <button class="control-btn danger" onclick="deleteDataset('${dataset.name}')">
            <i class="fas fa-trash"></i>
          </button>
        `;
        datasetList.appendChild(datasetItem);
      });
    }

    function handleFileUpload(files) {
      if (files.length === 0) return;
      
      const formData = new FormData();
      for (let i = 0; i < files.length; i++) {
        formData.append('files', files[i]);
      }
      
      document.getElementById('processing-loading').style.display = 'flex';
      document.getElementById('loading-text').textContent = 'Uploading dataset...';
      
      fetch('/upload-dataset', {
        method: 'POST',
        body: formData
      })
      .then(response => response.json())
      .then(data => {
        if (data.success) {
          showNotification('Dataset uploaded successfully!', 'info');
          loadDatasetList();
        } else {
          throw new Error(data.message);
        }
      })
      .catch(error => {
        showNotification('Upload failed: ' + error.message, 'danger');
      })
      .finally(() => {
        document.getElementById('processing-loading').style.display = 'none';
      });
    }

    function uploadDataset() {
      // Implementation for structured dataset upload
      hideDatasetModal();
      showNotification('Dataset upload feature activated', 'info');
    }

    function loadImportObjects() {
      // Load available objects for import
      const importSelect = document.getElementById('import-object');
      importSelect.innerHTML = '<option value="">-- Select Object --</option>';
      
      // Add sample objects (in real implementation, fetch from server)
      const objects = ['Keys', 'Phone', 'Glasses', 'Wallet', 'Medication'];
      objects.forEach(obj => {
        const option = document.createElement('option');
        option.value = obj;
        option.textContent = obj;
        importSelect.appendChild(option);
      });
    }

    function importDataset() {
      const selectedObject = document.getElementById('import-object').value;
      const method = document.getElementById('import-method').value;
      
      if (!selectedObject) {
        showNotification('Please select an object', 'warning');
        return;
      }
      
      hideImportModal();
      
      fetch('/import-dataset', {
        method: 'POST',
        headers: {
          'Content-Type': 'application/json',
        },
        body: JSON.stringify({
          objectName: selectedObject,
          method: method
        })
      })
      .then(response => response.json())
      .then(data => {
        if (data.success) {
          showNotification(`Dataset imported for ${selectedObject}`, 'info');
          loadDatasetList();
        } else {
          throw new Error(data.message);
        }
      })
      .catch(error => {
        showNotification('Import failed: ' + error.message, 'danger');
      });
    }

    function deleteDataset(datasetName) {
      if (!confirm(`Are you sure you want to delete dataset "${datasetName}"?`)) return;
      
      fetch('/delete-dataset', {
        method: 'POST',
        headers: {
          'Content-Type': 'application/json',
        },
        body: JSON.stringify({
          name: datasetName
        })
      })
      .then(response => response.json())
      .then(data => {
        if (data.success) {
          showNotification(`Dataset "${datasetName}" deleted`, 'info');
          loadDatasetList();
        } else {
          throw new Error(data.message);
        }
      })
      .catch(error => {
        showNotification('Delete failed: ' + error.message, 'danger');
      });
    }

    function deleteAllDatasets() {
      if (!confirm('Are you sure you want to delete ALL datasets? This cannot be undone.')) return;
      
      fetch('/delete-all-datasets', {
        method: 'POST'
      })
      .then(response => response.json())
      .then(data => {
        if (data.success) {
          showNotification('All datasets deleted', 'info');
          loadDatasetList();
        } else {
          throw new Error(data.message);
        }
      })
      .catch(error => {
        showNotification('Delete failed: ' + error.message, 'danger');
      });
    }

    function clearObjects() {
      fetch('/clear-objects', {
        method: 'POST'
      })
      .then(response => response.json())
      .then(data => {
        if (data.success) {
          showNotification('All objects cleared', 'info');
          addToHistory('Cleared all objects', 'system');
        } else {
          throw new Error(data.message);
        }
      })
      .catch(error => {
        showNotification('Clear failed: ' + error.message, 'danger');
      });
    }

    // Show training modal
    function showTrainingModal() {
      document.getElementById('training-modal').style.display = 'flex';
      document.getElementById('object-name').value = '';
      document.getElementById('object-category').value = 'Personal Items';
      document.getElementById('training-samples-count').value = '10';
      document.getElementById('confidence-threshold').value = '70';
    }

    // Hide training modal
    function hideTrainingModal() {
      document.getElementById('training-modal').style.display = 'none';
    }

    // Check for EEG alerts
    function checkEEGAlerts(data) {
      if (data.attention < 30) {
        showNotification('Low attention level detected', 'warning');
        addToHistory('Low attention alert', 'eeg-warning');
      }
      
      if (data.meditation < 20) {
        showNotification('High stress level detected', 'danger');
        addToHistory('High stress alert', 'eeg-danger');
      }
    }

    // Add to activity history
    function addToHistory(message, type = 'info') {
      const historyItem = {
        timestamp: new Date().toLocaleString(),
        message: message,
        type: type
      };
      
      state.activityHistory.unshift(historyItem);
      
      // Keep only last 10 items
      if (state.activityHistory.length > 10) {
        state.activityHistory.pop();
      }
      
      updateHistoryDisplay();
      saveActivityHistory();
    }

    // Update history display
    function updateHistoryDisplay() {
      const historyList = document.getElementById('activity-history');
      historyList.innerHTML = '';
      
      state.activityHistory.forEach(item => {
        const historyItem = document.createElement('div');
        historyItem.className = `history-item ${item.type.includes('warning') ? 'warning' : ''} ${item.type.includes('danger') ? 'danger' : ''} ${item.type.includes('info') ? 'info' : ''}`;
        
        historyItem.innerHTML = `
          <div class="history-content">
            <div class="history-object">${item.message}</div>
            <div class="history-eeg">${item.type}</div>
          </div>
          <div class="history-time">${item.timestamp}</div>
        `;
        
        historyList.appendChild(historyItem);
      });
    }

    // Save history to localStorage
    function saveActivityHistory() {
      localStorage.setItem('activityHistory', JSON.stringify(state.activityHistory));
    }

    // Load history from localStorage
    function loadActivityHistory() {
      const saved = localStorage.getItem('activityHistory');
      if (saved) {
        state.activityHistory = JSON.parse(saved);
        updateHistoryDisplay();
      }
    }

    // Show notification
    function showNotification(message, type = 'info') {
      const notification = document.getElementById('notification');
      const messageEl = document.getElementById('notification-message');
      
      messageEl.textContent = message;
      notification.className = 'notification';
      
      if (type === 'warning') {
        notification.classList.add('warning');
      } else if (type === 'danger') {
        notification.classList.add('danger');
      } else if (type === 'info') {
        notification.classList.add('info');
      }
      
      notification.classList.add('show');
      
      setTimeout(() => {
        notification.classList.remove('show');
      }, 5000);
    }

    // Update current time
    function updateCurrentTime() {
      document.getElementById('current-time').textContent = new Date().toLocaleString();
    }

    // Keyboard shortcuts
    document.addEventListener('keydown', function(event) {
      if (event.code === 'Space') {
        event.preventDefault();
        detectObjects();
      } else if (event.code === 'KeyT' && event.ctrlKey) {
        event.preventDefault();
        startAdvancedTraining();
      } else if (event.code === 'KeyO') {
        event.preventDefault();
        setMode('object');
      } else if (event.code === 'KeyD') {
        event.preventDefault();
        showTab('dataset-controls');
      } else if (event.code === 'Escape') {
        hideTrainingModal();
        hideDatasetModal();
        hideImportModal();
      }
    });
  </script>
</body>
</html>
)rawliteral";

// Calculate checksum for object features
uint16_t calculateChecksum(uint8_t* data, size_t length) {
  uint16_t checksum = 0;
  for (size_t i = 0; i < length; i++) {
    checksum += data[i];
  }
  return checksum;
}

// Extract features from image (simplified version)
void extractFeatures(camera_fb_t* fb, uint8_t* features) {
  // Simplified feature extraction - in real implementation, use proper computer vision
  for (int i = 0; i < FEATURE_VECTOR_SIZE; i++) {
    features[i] = (fb->len > i) ? fb->buf[i % fb->len] : random(256);
  }
}

// Compare features and return similarity score
float compareFeatures(uint8_t* features1, uint8_t* features2) {
  int matches = 0;
  for (int i = 0; i < FEATURE_VECTOR_SIZE; i++) {
    if (abs(features1[i] - features2[i]) < 10) {
      matches++;
    }
  }
  return (matches * 100.0) / FEATURE_VECTOR_SIZE;
}

// Save object database to EEPROM
void saveObjectDatabase() {
  EEPROM.put(0, objectCount);
  for (int i = 0; i < objectCount; i++) {
    EEPROM.put(sizeof(objectCount) + i * sizeof(ObjectPattern), objectDatabase[i]);
  }
  EEPROM.commit();
  Serial.printf("Saved %d objects to EEPROM\n", objectCount);
}

// Load object database from EEPROM
void loadObjectDatabase() {
  EEPROM.get(0, objectCount);
  if (objectCount > 0 && objectCount <= MAX_OBJECTS) {
    for (int i = 0; i < objectCount; i++) {
      EEPROM.get(sizeof(objectCount) + i * sizeof(ObjectPattern), objectDatabase[i]);
    }
    Serial.printf("Loaded %d objects from EEPROM\n", objectCount);
  } else {
    objectCount = 0;
    Serial.println("No valid object database found in EEPROM");
  }
}

// Initialize SPIFFS
void initSPIFFS() {
  if (!SPIFFS.begin(true)) {
    Serial.println("SPIFFS mount failed");
    return;
  }
  
  // Create datasets directory if it doesn't exist
  if (!SPIFFS.exists(DATASET_PATH)) {
    SPIFFS.mkdir(DATASET_PATH);
  }
  
  Serial.println("SPIFFS mounted successfully");
}

// Initialize EEPROM
void initEEPROM() {
  EEPROM.begin(EEPROM_SIZE);
  loadObjectDatabase();
}

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

// EEG Data Handler
static esp_err_t eeg_data_handler(httpd_req_t *req) {
  EEGData eegData;
  eegData.timestamp = millis();
  
  // Generate simulated EEG data
  for (int i = 0; i < EEG_CHANNELS; i++) {
    eegData.channels[i] = random(1000) / 10.0;
  }
  
  eegData.attention = 50 + random(50);
  eegData.meditation = 40 + random(60);
  eegData.alpha = 20 + random(60);
  eegData.beta = 15 + random(50);
  eegData.theta = 10 + random(40);
  eegData.delta = 5 + random(30);
  
  // Prepare JSON response
  String jsonResponse = "{";
  jsonResponse += "\"success\":true,";
  jsonResponse += "\"attention\":" + String(eegData.attention) + ",";
  jsonResponse += "\"meditation\":" + String(eegData.meditation) + ",";
  jsonResponse += "\"alpha\":" + String(eegData.alpha) + ",";
  jsonResponse += "\"beta\":" + String(eegData.beta) + ",";
  jsonResponse += "\"theta\":" + String(eegData.theta) + ",";
  jsonResponse += "\"delta\":" + String(eegData.delta) + ",";
  jsonResponse += "\"timestamp\":" + String(eegData.timestamp);
  jsonResponse += "}";
  
  httpd_resp_set_type(req, "application/json");
  return httpd_resp_send(req, jsonResponse.c_str(), jsonResponse.length());
}

// Enhanced Object Detection Handler
static esp_err_t detect_objects_handler(httpd_req_t *req) {
  camera_fb_t *fb = esp_camera_fb_get();
  if (!fb) {
    httpd_resp_send_500(req);
    return ESP_FAIL;
  }

  uint32_t startTime = millis();
  
  // Extract features from current image
  uint8_t currentFeatures[FEATURE_VECTOR_SIZE];
  extractFeatures(fb, currentFeatures);
  
  // Compare with known objects
  String objectsJson = "";
  int detectedCount = 0;
  float totalConfidence = 0;
  
  for (int i = 0; i < objectCount; i++) {
    float similarity = compareFeatures(currentFeatures, objectDatabase[i].features);
    
    if (similarity > objectDatabase[i].confidenceThreshold) {
      detectedCount++;
      totalConfidence += similarity;
      
      // Generate random position for demonstration
      int x = 10 + random(80);
      int y = 10 + random(80);
      int width = 5 + random(20);
      int height = 5 + random(20);
      
      if (objectsJson.length() > 0) objectsJson += ",";
      objectsJson += "{";
      objectsJson += "\"x\":" + String(x) + ",";
      objectsJson += "\"y\":" + String(y) + ",";
      objectsJson += "\"width\":" + String(width) + ",";
      objectsJson += "\"height\":" + String(height) + ",";
      objectsJson += "\"name\":\"" + String(objectDatabase[i].name) + "\",";
      objectsJson += "\"confidence\":" + String(similarity);
      objectsJson += "}";
      
      // Update last detected timestamp
      objectDatabase[i].lastDetected = millis();
    }
  }
  
  uint32_t processingTime = millis() - startTime;
  float avgConfidence = detectedCount > 0 ? totalConfidence / detectedCount : 0;
  
  // Prepare response
  String jsonResponse = "{";
  jsonResponse += "\"success\":true,";
  jsonResponse += "\"objects\":[" + objectsJson + "],";
  jsonResponse += "\"confidence\":" + String(avgConfidence) + ",";
  jsonResponse += "\"processingTime\":" + String(processingTime);
  jsonResponse += "}";
  
  esp_camera_fb_return(fb);
  httpd_resp_set_type(req, "application/json");
  return httpd_resp_send(req, jsonResponse.c_str(), jsonResponse.length());
}

// Advanced Training Handler
static esp_err_t start_advanced_training_handler(httpd_req_t *req) {
  char buf[1024];
  int ret = httpd_req_recv(req, buf, sizeof(buf));
  
  if (ret <= 0) {
    httpd_resp_send_500(req);
    return ESP_FAIL;
  }
  
  buf[ret] = '\0';
  
  // Parse JSON
  DynamicJsonDocument doc(1024);
  DeserializationError error = deserializeJson(doc, buf);
  
  if (error) {
    httpd_resp_send_500(req);
    return ESP_FAIL;
  }
  
  const char* name = doc["name"];
  const char* category = doc["category"];
  int samples = doc["samples"];
  int confidence = doc["confidence"];
  
  // Initialize training session
  strncpy(currentTraining.objectName, name, sizeof(currentTraining.objectName) - 1);
  strncpy(currentTraining.category, category, sizeof(currentTraining.category) - 1);
  currentTraining.samplesCollected = 0;
  currentTraining.targetSamples = samples;
  currentTraining.startTime = millis();
  currentTraining.isActive = true;
  
  String jsonResponse = "{";
  jsonResponse += "\"success\":true,";
  jsonResponse += "\"message\":\"Advanced training started\",";
  jsonResponse += "\"session\":{";
  jsonResponse += "\"objectName\":\"" + String(name) + "\",";
  jsonResponse += "\"targetSamples\":" + String(samples);
  jsonResponse += "}}";
  
  httpd_resp_set_type(req, "application/json");
  return httpd_resp_send(req, jsonResponse.c_str(), jsonResponse.length());
}

// Training Progress Handler
static esp_err_t training_progress_handler(httpd_req_t *req) {
  String jsonResponse = "{";
  jsonResponse += "\"success\":true,";
  jsonResponse += "\"samplesCollected\":" + String(currentTraining.samplesCollected) + ",";
  jsonResponse += "\"targetSamples\":" + String(currentTraining.targetSamples) + ",";
  jsonResponse += "\"isActive\":" + String(currentTraining.isActive ? "true" : "false");
  jsonResponse += "}";
  
  httpd_resp_set_type(req, "application/json");
  return httpd_resp_send(req, jsonResponse.c_str(), jsonResponse.length());
}

// Capture Sample Handler
static esp_err_t capture_sample_handler(httpd_req_t *req) {
  if (!currentTraining.isActive) {
    httpd_resp_send_500(req);
    return ESP_FAIL;
  }
  
  camera_fb_t *fb = esp_camera_fb_get();
  if (!fb) {
    httpd_resp_send_500(req);
    return ESP_FAIL;
  }
  
  // Store sample
  currentTraining.samplesCollected++;
  
  // If this is the first sample or we've collected enough samples, create/update object
  if (currentTraining.samplesCollected == 1 || currentTraining.samplesCollected >= currentTraining.targetSamples) {
    // Find or create object in database
    int objectIndex = -1;
    for (int i = 0; i < objectCount; i++) {
      if (strcmp(objectDatabase[i].name, currentTraining.objectName) == 0) {
        objectIndex = i;
        break;
      }
    }
    
    if (objectIndex == -1 && objectCount < MAX_OBJECTS) {
      objectIndex = objectCount;
      objectCount++;
    }
    
    if (objectIndex != -1) {
      // Update object pattern
      strncpy(objectDatabase[objectIndex].name, currentTraining.objectName, sizeof(objectDatabase[objectIndex].name) - 1);
      strncpy(objectDatabase[objectIndex].category, currentTraining.category, sizeof(objectDatabase[objectIndex].category) - 1);
      extractFeatures(fb, objectDatabase[objectIndex].features);
      objectDatabase[objectIndex].timestamp = millis();
      objectDatabase[objectIndex].sampleCount = currentTraining.samplesCollected;
      objectDatabase[objectIndex].confidenceThreshold = 70; // Default threshold
      objectDatabase[objectIndex].checksum = calculateChecksum(objectDatabase[objectIndex].features, FEATURE_VECTOR_SIZE);
      
      // Save to EEPROM
      saveObjectDatabase();
    }
    
    // If training is complete
    if (currentTraining.samplesCollected >= currentTraining.targetSamples) {
      currentTraining.isActive = false;
    }
  }
  
  String jsonResponse = "{";
  jsonResponse += "\"success\":true,";
  jsonResponse += "\"currentSample\":" + String(currentTraining.samplesCollected) + ",";
  jsonResponse += "\"targetSamples\":" + String(currentTraining.targetSamples) + ",";
  jsonResponse += "\"isTrainingActive\":" + String(currentTraining.isActive ? "true" : "false");
  jsonResponse += "}";
  
  esp_camera_fb_return(fb);
  httpd_resp_set_type(req, "application/json");
  return httpd_resp_send(req, jsonResponse.c_str(), jsonResponse.length());
}

// Dataset Management Handlers
static esp_err_t list_datasets_handler(httpd_req_t *req) {
  // Simplified dataset listing - in real implementation, scan SPIFFS directory
  String jsonResponse = "{\"success\":true,\"datasets\":[";
  
  // Add sample datasets (in real implementation, read from SPIFFS)
  if (objectCount > 0) {
    for (int i = 0; i < objectCount; i++) {
      if (i > 0) jsonResponse += ",";
      jsonResponse += "{";
      jsonResponse += "\"name\":\"" + String(objectDatabase[i].name) + "\",";
      jsonResponse += "\"category\":\"" + String(objectDatabase[i].category) + "\",";
      jsonResponse += "\"samples\":" + String(objectDatabase[i].sampleCount) + ",";
      jsonResponse += "\"timestamp\":" + String(objectDatabase[i].timestamp);
      jsonResponse += "}";
    }
  }
  
  jsonResponse += "]}";
  
  httpd_resp_set_type(req, "application/json");
  return httpd_resp_send(req, jsonResponse.c_str(), jsonResponse.length());
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
  
  httpd_uri_t eeg_data_uri = {
    .uri = "/eeg-data",
    .method = HTTP_GET,
    .handler = eeg_data_handler,
    .user_ctx = NULL
  };
  
  httpd_uri_t detect_objects_uri = {
    .uri = "/detect-objects",
    .method = HTTP_GET,
    .handler = detect_objects_handler,
    .user_ctx = NULL
  };
  
  httpd_uri_t start_advanced_training_uri = {
    .uri = "/start-advanced-training",
    .method = HTTP_POST,
    .handler = start_advanced_training_handler,
    .user_ctx = NULL
  };
  
  httpd_uri_t training_progress_uri = {
    .uri = "/training-progress",
    .method = HTTP_GET,
    .handler = training_progress_handler,
    .user_ctx = NULL
  };
  
  httpd_uri_t capture_sample_uri = {
    .uri = "/capture-sample",
    .method = HTTP_POST,
    .handler = capture_sample_handler,
    .user_ctx = NULL
  };
  
  httpd_uri_t list_datasets_uri = {
    .uri = "/list-datasets",
    .method = HTTP_GET,
    .handler = list_datasets_handler,
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
    httpd_register_uri_handler(camera_httpd, &eeg_data_uri);
    httpd_register_uri_handler(camera_httpd, &detect_objects_uri);
    httpd_register_uri_handler(camera_httpd, &start_advanced_training_uri);
    httpd_register_uri_handler(camera_httpd, &training_progress_uri);
    httpd_register_uri_handler(camera_httpd, &capture_sample_uri);
    httpd_register_uri_handler(camera_httpd, &list_datasets_uri);
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
  
  // Initialize storage
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
  
  // Initialize camera
  esp_err_t err = esp_camera_init(&config);
  if (err != ESP_OK) {
    Serial.printf("Camera init failed with error 0x%x", err);
    return;
  }
  
  // Connect to WiFi
  WiFi.begin(ssid, password);
  WiFi.setSleep(false);
  
  Serial.print("Connecting to WiFi");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nWiFi connected");
  
  Serial.print("Camera Stream Ready! Go to: http://");
  Serial.println(WiFi.localIP());
  
  // Start web server
  startCameraServer();
  
  Serial.println("Advanced Memory Enhancement Device Started");
  Serial.println("Modes: Basic/Advanced Training, Testing, EEG Monitoring, Dataset Management");
}

// Main loop
void loop() {
  // Handle periodic tasks
  if (millis() - lastEEGUpdate > 1000) {
    lastEEGUpdate = millis();
  }
  
  // Check for training timeout
  if (currentTraining.isActive && (millis() - currentTraining.startTime > 300000)) { // 5 minute timeout
    currentTraining.isActive = false;
    Serial.println("Training session timed out");
  }
  
  delay(100);
}
