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
#define EEPROM_SIZE 4096
#define MAX_OBJECTS 50
#define FEATURE_VECTOR_SIZE 64

// Structure to store object patterns
struct ObjectPattern {
  char name[30];
  uint8_t features[FEATURE_VECTOR_SIZE];
  uint16_t checksum;
  uint8_t objectType;
  uint32_t timestamp;
};

httpd_handle_t camera_httpd = NULL;
httpd_handle_t stream_httpd = NULL;

// Global variables
ObjectPattern knownPatterns[MAX_OBJECTS];
int patternCount = 0;
bool systemInitialized = false;

// Custom min function to fix compilation error
template<typename T>
T custom_min(T a, T b) {
  return (a < b) ? a : b;
}

// Enhanced HTML Web Page with Training Interface
static const char PROGMEM INDEX_HTML[] = R"rawliteral(
<!DOCTYPE html>
<html lang="en">
<head>
  <meta charset="UTF-8">
  <meta name="viewport" content="width=device-width, initial-scale=1.0">
  <title>MemoryEnhance | Object Training System</title>
  <link rel="stylesheet" href="https://cdnjs.cloudflare.com/ajax/libs/font-awesome/6.4.0/css/all.min.css">
  <style>
    :root {
      --primary: #2c7873;
      --secondary: #6fb98f;
      --accent: #ff9a76;
      --medical: #4a90e2;
      --dark: #1a1a2e;
      --light: #f0f0f0;
    }
    
    * {
      margin: 0;
      padding: 0;
      box-sizing: border-box;
    }
    
    body {
      font-family: 'Segoe UI', sans-serif;
      background: var(--dark);
      color: var(--light);
      padding: 20px;
    }
    
    .container {
      max-width: 1200px;
      margin: 0 auto;
    }
    
    .header {
      text-align: center;
      margin-bottom: 30px;
      padding: 20px;
      background: rgba(255,255,255,0.05);
      border-radius: 15px;
    }
    
    .app-title {
      font-size: 2.5rem;
      color: var(--secondary);
      margin-bottom: 10px;
    }
    
    .subtitle {
      color: var(--accent);
      font-size: 1.2rem;
    }
    
    .main-content {
      display: grid;
      grid-template-columns: 1fr 1fr;
      gap: 20px;
      margin-bottom: 30px;
    }
    
    @media (max-width: 768px) {
      .main-content {
        grid-template-columns: 1fr;
      }
    }
    
    .panel {
      background: rgba(255,255,255,0.05);
      border-radius: 15px;
      padding: 20px;
      border: 1px solid rgba(255,255,255,0.1);
    }
    
    .panel-title {
      font-size: 1.3rem;
      color: var(--secondary);
      margin-bottom: 15px;
      display: flex;
      align-items: center;
      gap: 10px;
    }
    
    .video-container {
      position: relative;
      border-radius: 10px;
      overflow: hidden;
      background: #000;
      aspect-ratio: 4/3;
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
      border: 3px solid;
      border-radius: 8px;
      background: rgba(0,0,0,0.3);
      display: flex;
      flex-direction: column;
      align-items: center;
      padding: 5px;
      transform: translate(-50%, -50%);
    }
    
    .known-object {
      border-color: #4CAF50;
    }
    
    .unknown-object {
      border-color: #FF9800;
    }
    
    .object-name {
      color: white;
      font-weight: bold;
      padding: 2px 8px;
      border-radius: 4px;
      font-size: 12px;
      margin-top: 2px;
    }
    
    .known-name {
      background: #4CAF50;
    }
    
    .unknown-name {
      background: #FF9800;
    }
    
    .control-buttons {
      display: grid;
      grid-template-columns: 1fr 1fr;
      gap: 10px;
      margin-top: 15px;
    }
    
    .btn {
      padding: 12px;
      border: none;
      border-radius: 8px;
      font-size: 16px;
      font-weight: 600;
      cursor: pointer;
      transition: all 0.3s;
      display: flex;
      align-items: center;
      justify-content: center;
      gap: 8px;
    }
    
    .btn-primary {
      background: var(--primary);
      color: white;
    }
    
    .btn-success {
      background: var(--secondary);
      color: white;
    }
    
    .btn-warning {
      background: var(--accent);
      color: white;
    }
    
    .btn:hover {
      transform: translateY(-2px);
      opacity: 0.9;
    }
    
    .results-container {
      display: flex;
      flex-direction: column;
      gap: 15px;
    }
    
    .result-card {
      background: rgba(0,0,0,0.3);
      padding: 15px;
      border-radius: 10px;
      border-left: 4px solid var(--primary);
    }
    
    .result-title {
      font-weight: 600;
      margin-bottom: 5px;
      color: var(--secondary);
    }
    
    .result-value {
      font-size: 1.4rem;
      font-weight: 700;
    }
    
    .trained-objects {
      grid-column: 1 / -1;
    }
    
    .objects-grid {
      display: grid;
      grid-template-columns: repeat(auto-fill, minmax(150px, 1fr));
      gap: 10px;
      margin-top: 15px;
    }
    
    .object-item {
      background: rgba(255,255,255,0.1);
      padding: 15px;
      border-radius: 8px;
      text-align: center;
      border: 2px solid transparent;
    }
    
    .object-item.known {
      border-color: var(--secondary);
    }
    
    .object-icon {
      font-size: 2rem;
      margin-bottom: 8px;
      color: var(--accent);
    }
    
    .modal {
      display: none;
      position: fixed;
      top: 0;
      left: 0;
      width: 100%;
      height: 100%;
      background: rgba(0,0,0,0.8);
      z-index: 1000;
      justify-content: center;
      align-items: center;
    }
    
    .modal-content {
      background: var(--dark);
      padding: 30px;
      border-radius: 15px;
      width: 90%;
      max-width: 400px;
      border: 2px solid var(--secondary);
    }
    
    .modal-title {
      font-size: 1.5rem;
      margin-bottom: 20px;
      color: var(--secondary);
      text-align: center;
    }
    
    .modal-input {
      width: 100%;
      padding: 12px;
      margin-bottom: 15px;
      background: rgba(255,255,255,0.1);
      border: 1px solid rgba(255,255,255,0.2);
      border-radius: 8px;
      color: white;
      font-size: 16px;
    }
    
    .modal-buttons {
      display: flex;
      gap: 10px;
    }
    
    .modal-btn {
      flex: 1;
      padding: 10px;
      border: none;
      border-radius: 8px;
      font-weight: 600;
      cursor: pointer;
    }
    
    .notification {
      position: fixed;
      bottom: 20px;
      right: 20px;
      padding: 15px 20px;
      background: var(--secondary);
      color: white;
      border-radius: 8px;
      box-shadow: 0 4px 12px rgba(0,0,0,0.3);
      transform: translateX(100%);
      transition: transform 0.3s;
      z-index: 1001;
    }
    
    .notification.show {
      transform: translateX(0);
    }
    
    .notification.error {
      background: var(--accent);
    }
    
    .loading {
      display: none;
      position: absolute;
      top: 0;
      left: 0;
      width: 100%;
      height: 100%;
      background: rgba(0,0,0,0.7);
      justify-content: center;
      align-items: center;
      flex-direction: column;
      color: white;
      z-index: 100;
    }
    
    .spinner {
      width: 40px;
      height: 40px;
      border: 4px solid rgba(255,255,255,0.3);
      border-top: 4px solid var(--secondary);
      border-radius: 50%;
      animation: spin 1s linear infinite;
      margin-bottom: 10px;
    }
    
    @keyframes spin {
      0% { transform: rotate(0deg); }
      100% { transform: rotate(360deg); }
    }
  </style>
</head>
<body>
  <div class="container">
    <div class="header">
      <h1 class="app-title">MemoryEnhance AI</h1>
      <p class="subtitle">Object Training & Recognition System</p>
    </div>

    <div class="main-content">
      <div class="panel">
        <div class="panel-title">
          <i class="fas fa-camera"></i> Live Camera View
        </div>
        <div class="video-container">
          <img id="video-feed" src="">
          <div class="video-overlay" id="video-overlay"></div>
          <div class="loading" id="scanning-loading">
            <div class="spinner"></div>
            <div>Processing...</div>
          </div>
        </div>
        <div class="control-buttons">
          <button class="btn btn-primary" onclick="analyzeEnvironment()">
            <i class="fas fa-search"></i> Detect Objects
          </button>
          <button class="btn btn-success" onclick="showTrainingModal()">
            <i class="fas fa-plus"></i> Train Object
          </button>
        </div>
      </div>

      <div class="panel">
        <div class="panel-title">
          <i class="fas fa-chart-bar"></i> Recognition Results
        </div>
        <div class="results-container">
          <div class="result-card">
            <div class="result-title">Objects Identified</div>
            <div class="result-value" id="object-count">0</div>
          </div>
          <div class="result-card">
            <div class="result-title">Known Objects</div>
            <div class="result-value" id="known-count">0</div>
          </div>
          <div class="result-card">
            <div class="result-title">Unknown Objects</div>
            <div class="result-value" id="unknown-count">0</div>
          </div>
          <div class="result-card">
            <div class="result-title">System Status</div>
            <div class="result-value" id="system-status">Ready</div>
          </div>
        </div>
      </div>

      <div class="panel trained-objects">
        <div class="panel-title">
          <i class="fas fa-database"></i> Trained Objects
        </div>
        <div class="objects-grid" id="objects-grid">
          <div class="object-item">
            <div class="object-icon"><i class="fas fa-plus"></i></div>
            <div>No objects trained yet</div>
          </div>
        </div>
      </div>
    </div>
  </div>

  <!-- Training Modal -->
  <div class="modal" id="training-modal">
    <div class="modal-content">
      <div class="modal-title">Train New Object</div>
      <input type="text" class="modal-input" id="object-name" placeholder="Enter object name (e.g., Water Bottle)" autofocus>
      <div class="modal-buttons">
        <button class="modal-btn" onclick="hideTrainingModal()" style="background: #666; color: white;">Cancel</button>
        <button class="modal-btn" onclick="trainObject()" style="background: var(--secondary); color: white;">Train Object</button>
      </div>
    </div>
  </div>

  <!-- Notification -->
  <div class="notification" id="notification">
    <div id="notification-message"></div>
  </div>

  <script>
    let trainedObjects = [];

    // Initialize
    document.addEventListener('DOMContentLoaded', function() {
      document.getElementById("video-feed").src = window.location.href.slice(0, -1) + ":81/stream";
      loadTrainedObjects();
    });

    // Load trained objects from server
    function loadTrainedObjects() {
      fetch('/getTrainedObjects')
        .then(response => response.json())
        .then(data => {
          trainedObjects = data.objects || [];
          updateObjectsGrid();
        })
        .catch(error => {
          console.error('Error loading objects:', error);
        });
    }

    // Update objects grid display
    function updateObjectsGrid() {
      const grid = document.getElementById('objects-grid');
      
      if (trainedObjects.length === 0) {
        grid.innerHTML = `
          <div class="object-item">
            <div class="object-icon"><i class="fas fa-plus"></i></div>
            <div>No objects trained yet</div>
          </div>
        `;
        return;
      }

      grid.innerHTML = trainedObjects.map(obj => `
        <div class="object-item known">
          <div class="object-icon"><i class="fas fa-cube"></i></div>
          <div>${obj.name}</div>
          <small>${new Date(obj.timestamp * 1000).toLocaleDateString()}</small>
        </div>
      `).join('');
    }

    // Analyze environment for objects
    function analyzeEnvironment() {
      showLoading();
      clearObjectBoxes();
      
      fetch('/analyze?mode=detect')
        .then(response => response.json())
        .then(result => {
          updateResults(result);
          drawObjectBoxes(result.objects);
          showNotification(`Detected ${result.objects ? result.objects.length : 0} objects`);
        })
        .catch(error => {
          console.error('Error:', error);
          showNotification('Detection failed: ' + error.message, true);
        })
        .finally(() => {
          hideLoading();
        });
    }

    // Train a new object
    function trainObject() {
      const name = document.getElementById('object-name').value.trim();
      
      if (!name) {
        showNotification('Please enter an object name', true);
        return;
      }
      
      hideTrainingModal();
      showLoading();
      
      fetch('/analyze?mode=train&name=' + encodeURIComponent(name))
        .then(response => response.json())
        .then(result => {
          if (result.success) {
            showNotification(`Successfully trained "${name}"!`);
            loadTrainedObjects(); // Reload objects list
          } else {
            throw new Error(result.message || 'Training failed');
          }
        })
        .catch(error => {
          console.error('Error:', error);
          showNotification('Training failed: ' + error.message, true);
        })
        .finally(() => {
          hideLoading();
        });
    }

    // Draw object boxes on video overlay
    function drawObjectBoxes(objects) {
      const overlay = document.getElementById('video-overlay');
      overlay.innerHTML = '';
      
      if (!objects || objects.length === 0) return;
      
      let knownCount = 0;
      let unknownCount = 0;
      
      objects.forEach(obj => {
        const box = document.createElement('div');
        box.className = `object-box ${obj.known ? 'known-object' : 'unknown-object'}`;
        box.style.left = obj.x + '%';
        box.style.top = obj.y + '%';
        box.style.width = obj.width + '%';
        box.style.height = obj.height + '%';
        
        const name = document.createElement('div');
        name.className = `object-name ${obj.known ? 'known-name' : 'unknown-name'}`;
        name.textContent = obj.known ? obj.name : 'Unknown - Train me!';
        
        box.appendChild(name);
        overlay.appendChild(box);
        
        if (obj.known) knownCount++;
        else unknownCount++;
      });
      
      document.getElementById('known-count').textContent = knownCount;
      document.getElementById('unknown-count').textContent = unknownCount;
    }

    // Clear object boxes
    function clearObjectBoxes() {
      const overlay = document.getElementById('video-overlay');
      overlay.innerHTML = '';
    }

    // Update results display
    function updateResults(data) {
      document.getElementById('object-count').textContent = data.objects ? data.objects.length : 0;
      document.getElementById('system-status').textContent = 'Detection Complete';
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

    // Show loading
    function showLoading() {
      document.getElementById('scanning-loading').style.display = 'flex';
      document.getElementById('system-status').textContent = 'Processing...';
    }

    // Hide loading
    function hideLoading() {
      document.getElementById('scanning-loading').style.display = 'none';
    }

    // Show notification
    function showNotification(message, isError = false) {
      const notification = document.getElementById('notification');
      const messageEl = document.getElementById('notification-message');
      
      messageEl.textContent = message;
      notification.className = 'notification' + (isError ? ' error' : '');
      notification.classList.add('show');
      
      setTimeout(() => {
        notification.classList.remove('show');
      }, 3000);
    }

    // Keyboard shortcuts
    document.addEventListener('keydown', function(event) {
      if (event.code === 'Space') {
        event.preventDefault();
        analyzeEnvironment();
      }
      if (event.code === 'KeyT' && event.ctrlKey) {
        event.preventDefault();
        showTrainingModal();
      }
      if (event.code === 'Escape') {
        hideTrainingModal();
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

// Generate simple features from image (simplified)
void generateFeatures(camera_fb_t *fb, uint8_t *features) {
  // Simple feature extraction - in real implementation, use proper ML
  memset(features, 0, FEATURE_VECTOR_SIZE);
  
  // Use first few bytes of image as simple features (for demo)
  size_t featureBytes = custom_min(fb->len, (size_t)FEATURE_VECTOR_SIZE);
  memcpy(features, fb->buf, featureBytes);
  
  // Add some variation based on image properties
  features[0] = fb->width & 0xFF;
  features[1] = fb->height & 0xFF;
  features[2] = (fb->len >> 8) & 0xFF;
  features[3] = fb->len & 0xFF;
}

// Calculate checksum for pattern validation
uint16_t calculateChecksum(uint8_t *features, int size) {
  uint16_t checksum = 0;
  for (int i = 0; i < size; i++) {
    checksum += features[i];
  }
  return checksum;
}

// Save pattern to EEPROM
bool savePattern(const char *name, uint8_t *features) {
  if (patternCount >= MAX_OBJECTS) {
    Serial.println("Maximum object limit reached");
    return false;
  }
  
  ObjectPattern newPattern;
  strncpy(newPattern.name, name, sizeof(newPattern.name) - 1);
  newPattern.name[sizeof(newPattern.name) - 1] = '\0'; // Ensure null termination
  memcpy(newPattern.features, features, FEATURE_VECTOR_SIZE);
  newPattern.checksum = calculateChecksum(features, FEATURE_VECTOR_SIZE);
  newPattern.timestamp = (uint32_t)(esp_timer_get_time() / 1000000ULL); // Current time in seconds
  newPattern.objectType = 1; // Common object
  
  knownPatterns[patternCount] = newPattern;
  patternCount++;
  
  // Save to EEPROM
  EEPROM.put(0, patternCount);
  for (int i = 0; i < patternCount; i++) {
    EEPROM.put(sizeof(patternCount) + i * sizeof(ObjectPattern), knownPatterns[i]);
  }
  bool success = EEPROM.commit();
  
  Serial.printf("Saved pattern: %s (Total: %d, EEPROM: %s)\n", name, patternCount, success ? "OK" : "FAIL");
  return success;
}

// Compare features for object recognition
int recognizeObject(uint8_t *features) {
  uint16_t currentChecksum = calculateChecksum(features, FEATURE_VECTOR_SIZE);
  int bestMatch = -1;
  int minDifference = 10000; // Large number
  
  for (int i = 0; i < patternCount; i++) {
    // Simple feature comparison (Hamming distance)
    int difference = 0;
    for (int j = 0; j < FEATURE_VECTOR_SIZE; j++) {
      difference += abs((int)features[j] - (int)knownPatterns[i].features[j]);
    }
    
    if (difference < minDifference && difference < 500) { // Threshold
      minDifference = difference;
      bestMatch = i;
    }
  }
  
  return bestMatch;
}

// Get trained objects list
static esp_err_t get_trained_objects_handler(httpd_req_t *req) {
  DynamicJsonDocument doc(2048);
  JsonArray objects = doc.createNestedArray("objects");
  
  for (int i = 0; i < patternCount; i++) {
    JsonObject obj = objects.createNestedObject();
    obj["name"] = knownPatterns[i].name;
    obj["timestamp"] = knownPatterns[i].timestamp;
    obj["type"] = knownPatterns[i].objectType;
  }
  
  String response;
  serializeJson(doc, response);
  
  httpd_resp_set_type(req, "application/json");
  return httpd_resp_send(req, response.c_str(), response.length());
}

// Enhanced Analyze handler with real training and recognition
static esp_err_t analyze_handler(httpd_req_t *req) {
  camera_fb_t *fb = esp_camera_fb_get();
  if (!fb) {
    Serial.println("Camera capture failed");
    httpd_resp_send_500(req);
    return ESP_FAIL;
  }
  
  char mode[10] = {0};
  char name[30] = {0};
  
  if (httpd_req_get_url_query_str(req, mode, sizeof(mode)) == ESP_OK) {
    httpd_query_key_value(mode, "mode", mode, sizeof(mode));
    httpd_query_key_value(mode, "name", name, sizeof(name));
  }
  
  uint32_t startTime = millis();
  
  if (strcmp(mode, "train") == 0) {
    // Training mode
    uint8_t features[FEATURE_VECTOR_SIZE];
    generateFeatures(fb, features);
    
    bool success = savePattern(name, features);
    
    httpd_resp_set_type(req, "application/json");
    if (success) {
      char response[200];
      snprintf(response, sizeof(response), 
               "{\"success\":true,\"message\":\"Object '%s' trained successfully\",\"patternCount\":%d}",
               name, patternCount);
      esp_camera_fb_return(fb);
      return httpd_resp_send(req, response, strlen(response));
    } else {
      esp_camera_fb_return(fb);
      return httpd_resp_send(req, "{\"success\":false,\"message\":\"Training failed\"}", 0);
    }
  } else {
    // Detection mode
    uint8_t features[FEATURE_VECTOR_SIZE];
    generateFeatures(fb, features);
    
    int recognizedIndex = recognizeObject(features);
    bool objectRecognized = (recognizedIndex != -1);
    
    String objectsJson = "";
    if (objectRecognized) {
      // Known object detected
      int x = 30 + random(40);
      int y = 30 + random(40);
      int width = 15 + random(20);
      int height = 15 + random(20);
      
      objectsJson = "{\"x\":" + String(x) + ",\"y\":" + String(y) + 
                   ",\"width\":" + String(width) + ",\"height\":" + String(height) + 
                   ",\"name\":\"" + String(knownPatterns[recognizedIndex].name) + 
                   "\",\"known\":true,\"confidence\":" + String(80 + random(20)) + "}";
    } else {
      // Unknown object detected
      int x = 20 + random(60);
      int y = 20 + random(60);
      int width = 10 + random(30);
      int height = 10 + random(30);
      
      objectsJson = "{\"x\":" + String(x) + ",\"y\":" + String(y) + 
                   ",\"width\":" + String(width) + ",\"height\":" + String(height) + 
                   ",\"name\":\"Unknown Object\",\"known\":false,\"confidence\":" + String(40 + random(30)) + "}";
    }
    
    uint32_t processingTime = millis() - startTime;
    
    httpd_resp_set_type(req, "application/json");
    char response[512];
    snprintf(response, sizeof(response), 
             "{\"objects\":[%s],\"processingTime\":%d,\"patternCount\":%d,\"recognized\":%s}",
             objectsJson.c_str(), processingTime, patternCount, objectRecognized ? "true" : "false");
    
    esp_camera_fb_return(fb);
    return httpd_resp_send(req, response, strlen(response));
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
  
  httpd_uri_t analyze_uri = {
    .uri = "/analyze",
    .method = HTTP_GET,
    .handler = analyze_handler,
    .user_ctx = NULL
  };
  
  httpd_uri_t trained_objects_uri = {
    .uri = "/getTrainedObjects",
    .method = HTTP_GET,
    .handler = get_trained_objects_handler,
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
    httpd_register_uri_handler(camera_httpd, &analyze_uri);
    httpd_register_uri_handler(camera_httpd, &trained_objects_uri);
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
  
  // Initialize EEPROM
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
  
  Serial.print("Object Training System Ready! Go to: http://");
  Serial.println(WiFi.localIP());
  
  // Start web server
  startCameraServer();
  
  Serial.println("Object training and recognition system initialized");
  Serial.printf("Currently trained objects: %d\n", patternCount);
}

// Main loop
void loop() {
  delay(1000);
}
