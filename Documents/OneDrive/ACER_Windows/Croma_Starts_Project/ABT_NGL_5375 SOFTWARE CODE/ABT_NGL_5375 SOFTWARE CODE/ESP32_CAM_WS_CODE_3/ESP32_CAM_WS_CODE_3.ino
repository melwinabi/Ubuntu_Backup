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

const char* ssid = "ACTFIBERNET";
const char* password = "act@12345";
#define PART_BOUNDARY "123456789000000000000987654321"

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

#define EEPROM_SIZE 512
#define MAX_OBJECTS 20
#define FEATURE_VECTOR_SIZE 16

struct ObjectPattern {
  char name[20];
  uint8_t features[FEATURE_VECTOR_SIZE];
};

ObjectPattern knownPatterns[MAX_OBJECTS];
int patternCount = 0;
httpd_handle_t camera_httpd = NULL;
httpd_handle_t stream_httpd = NULL;

static const char PROGMEM INDEX_HTML[] = R"rawliteral(
<!DOCTYPE html>
<html>
<head>
  <meta name="viewport" content="width=device-width, initial-scale=1.0">
  <title>ESP32-CAM Object Training</title>
  <style>
    body { 
      font-family: Arial; 
      background: #1a1a2e; 
      color: white; 
      margin: 0; 
      padding: 20px; 
    }
    .container { 
      max-width: 1200px; 
      margin: 0 auto; 
    }
    .header { 
      text-align: center; 
      margin-bottom: 20px; 
    }
    .video-container { 
      position: relative; 
      width: 640px; 
      height: 480px; 
      margin: 0 auto; 
      background: black; 
    }
    #video-feed { 
      width: 100%; 
      height: 100%; 
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
      border: 3px solid #4CAF50;
      background: rgba(76, 175, 80, 0.1);
      border-radius: 5px;
    }
    .object-name {
      background: #4CAF50;
      color: white;
      padding: 2px 5px;
      font-size: 12px;
      border-radius: 3px;
      margin-top: -20px;
    }
    .controls { 
      text-align: center; 
      margin: 20px 0; 
    }
    button {
      background: #4CAF50;
      color: white;
      border: none;
      padding: 10px 20px;
      margin: 5px;
      border-radius: 5px;
      cursor: pointer;
      font-size: 16px;
    }
    button:hover { background: #45a049; }
    .results { 
      background: #2d3047; 
      padding: 15px; 
      border-radius: 5px; 
      margin: 10px 0; 
    }
    .trained-objects {
      display: flex;
      flex-wrap: wrap;
      gap: 10px;
      margin: 10px 0;
    }
    .object-item {
      background: #4CAF50;
      padding: 5px 10px;
      border-radius: 15px;
      font-size: 14px;
    }
    .modal {
      display: none;
      position: fixed;
      top: 0;
      left: 0;
      width: 100%;
      height: 100%;
      background: rgba(0,0,0,0.5);
      justify-content: center;
      align-items: center;
    }
    .modal-content {
      background: #2d3047;
      padding: 20px;
      border-radius: 10px;
      width: 300px;
    }
    input[type="text"] {
      width: 100%;
      padding: 10px;
      margin: 10px 0;
      border: 1px solid #444;
      background: #1a1a2e;
      color: white;
      border-radius: 5px;
    }
  </style>
</head>
<body>
  <div class="container">
    <div class="header">
      <h1>ESP32-CAM Object Training System</h1>
      <p>Train objects and recognize them in real-time</p>
    </div>

    <div class="video-container">
      <img id="video-feed" src="">
      <div class="video-overlay" id="video-overlay"></div>
    </div>

    <div class="controls">
      <button onclick="detectObjects()">Detect Objects</button>
      <button onclick="showTrainModal()">Train New Object</button>
      <button onclick="loadTrainedObjects()">Refresh Objects</button>
    </div>

    <div class="results">
      <h3>Detection Results:</h3>
      <div>Recognized Objects: <span id="recognized-count">0</span></div>
      <div>Unknown Objects: <span id="unknown-count">0</span></div>
    </div>

    <div class="results">
      <h3>Trained Objects (<span id="object-count">0</span>):</h3>
      <div class="trained-objects" id="trained-objects-list"></div>
    </div>
  </div>

  <div class="modal" id="train-modal">
    <div class="modal-content">
      <h3>Train New Object</h3>
      <input type="text" id="object-name" placeholder="Enter object name">
      <div style="text-align: right;">
        <button onclick="hideTrainModal()">Cancel</button>
        <button onclick="trainObject()">Train</button>
      </div>
    </div>
  </div>

  <script>
    let trainedObjects = [];

    document.addEventListener('DOMContentLoaded', function() {
      document.getElementById("video-feed").src = window.location.href.slice(0, -1) + ":81/stream";
      loadTrainedObjects();
    });

    function loadTrainedObjects() {
      fetch('/getTrainedObjects')
        .then(r => r.json())
        .then(data => {
          trainedObjects = data.objects || [];
          document.getElementById('object-count').textContent = trainedObjects.length;
          const container = document.getElementById('trained-objects-list');
          container.innerHTML = trainedObjects.map(obj => 
            `<div class="object-item">${obj.name}</div>`
          ).join('');
        });
    }

    function detectObjects() {
      fetch('/analyzeObjects')
        .then(r => r.json())
        .then(data => {
          document.getElementById('recognized-count').textContent = data.recognizedCount || 0;
          document.getElementById('unknown-count').textContent = data.unknownCount || 0;
          drawObjectBoxes(data.objects || []);
        });
    }

    function drawObjectBoxes(objects) {
      const overlay = document.getElementById('video-overlay');
      overlay.innerHTML = '';
      
      objects.forEach(obj => {
        const box = document.createElement('div');
        box.className = 'object-box';
        box.style.left = obj.x + '%';
        box.style.top = obj.y + '%';
        box.style.width = obj.width + '%';
        box.style.height = obj.height + '%';
        box.style.borderColor = obj.recognized ? '#4CAF50' : '#FF9800';
        
        const name = document.createElement('div');
        name.className = 'object-name';
        name.textContent = obj.recognized ? obj.name : 'Unknown - Train me!';
        name.style.background = obj.recognized ? '#4CAF50' : '#FF9800';
        
        box.appendChild(name);
        overlay.appendChild(box);
      });
    }

    function showTrainModal() {
      document.getElementById('train-modal').style.display = 'flex';
      document.getElementById('object-name').value = '';
    }

    function hideTrainModal() {
      document.getElementById('train-modal').style.display = 'none';
    }

    function trainObject() {
      const name = document.getElementById('object-name').value.trim();
      if (!name) {
        alert('Please enter object name');
        return;
      }

      fetch('/trainObject?name=' + encodeURIComponent(name))
        .then(r => r.json())
        .then(data => {
          if (data.success) {
            alert('Object trained successfully: ' + name);
            hideTrainModal();
            loadTrainedObjects();
          } else {
            alert('Training failed: ' + data.message);
          }
        });
    }
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
      res = ESP_FAIL;
    } else {
      if (fb->format != PIXFORMAT_JPEG) {
        bool jpeg_converted = frame2jpg(fb, 80, &_jpg_buf, &_jpg_buf_len);
        esp_camera_fb_return(fb);
        fb = NULL;
        if (!jpeg_converted) {
          res = ESP_FAIL;
        }
      } else {
        _jpg_buf_len = fb->len;
        _jpg_buf = fb->buf;
      }
    }
    if (res == ESP_OK) {
      size_t hlen = snprintf(part_buf, 64, _STREAM_PART, _jpg_buf_len);
      res = httpd_resp_send_chunk(req, part_buf, hlen);
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

void generateFeatures(camera_fb_t* fb, uint8_t* features) {
  // Simple feature extraction from image
  for(int i = 0; i < FEATURE_VECTOR_SIZE; i++) {
    int pos = (i * fb->len) / FEATURE_VECTOR_SIZE;
    features[i] = fb->buf[pos % fb->len];
  }
}

float calculateSimilarity(uint8_t* feat1, uint8_t* feat2) {
  int sum = 0;
  for(int i = 0; i < FEATURE_VECTOR_SIZE; i++) {
    int diff = feat1[i] - feat2[i];
    sum += diff * diff;
  }
  return 100.0 - (sqrt(sum) / FEATURE_VECTOR_SIZE);
}

String recognizeObject(camera_fb_t* fb) {
  uint8_t currentFeatures[FEATURE_VECTOR_SIZE];
  generateFeatures(fb, currentFeatures);
  
  float bestSimilarity = 0;
  int bestIndex = -1;
  
  for(int i = 0; i < patternCount; i++) {
    float similarity = calculateSimilarity(currentFeatures, knownPatterns[i].features);
    if(similarity > bestSimilarity && similarity > 40.0) {
      bestSimilarity = similarity;
      bestIndex = i;
    }
  }
  
  if(bestIndex != -1) {
    return String(knownPatterns[bestIndex].name);
  }
  
  return "unknown";
}

void savePatternsToEEPROM() {
  EEPROM.put(0, patternCount);
  for(int i = 0; i < patternCount; i++) {
    EEPROM.put(4 + i * sizeof(ObjectPattern), knownPatterns[i]);
  }
  EEPROM.commit();
}

void loadPatternsFromEEPROM() {
  EEPROM.get(0, patternCount);
  if(patternCount > MAX_OBJECTS) patternCount = 0;
  
  for(int i = 0; i < patternCount; i++) {
    EEPROM.get(4 + i * sizeof(ObjectPattern), knownPatterns[i]);
  }
}

static esp_err_t train_object_handler(httpd_req_t *req) {
  char name[50] = {0};
  
  if (httpd_req_get_url_query_str(req, name, sizeof(name)) == ESP_OK) {
    httpd_query_key_value(name, "name", name, sizeof(name));
  }
  
  if(strlen(name) == 0) {
    httpd_resp_send_500(req);
    return ESP_FAIL;
  }
  
  camera_fb_t *fb = esp_camera_fb_get();
  if (!fb) {
    httpd_resp_send_500(req);
    return ESP_FAIL;
  }
  
  if (patternCount >= MAX_OBJECTS) {
    esp_camera_fb_return(fb);
    httpd_resp_set_type(req, "application/json");
    httpd_resp_send(req, "{\"success\":false,\"message\":\"Maximum objects limit reached\"}", -1);
    return ESP_OK;
  }
  
  // Create new pattern
  ObjectPattern newPattern;
  strncpy(newPattern.name, name, sizeof(newPattern.name) - 1);
  generateFeatures(fb, newPattern.features);
  
  knownPatterns[patternCount] = newPattern;
  patternCount++;
  
  savePatternsToEEPROM();
  
  esp_camera_fb_return(fb);
  
  httpd_resp_set_type(req, "application/json");
  char response[200];
  snprintf(response, sizeof(response), 
           "{\"success\":true,\"message\":\"Object '%s' trained\",\"total\":%d}",
           name, patternCount);
  
  return httpd_resp_send(req, response, strlen(response));
}

static esp_err_t analyze_objects_handler(httpd_req_t *req) {
  camera_fb_t *fb = esp_camera_fb_get();
  if (!fb) {
    httpd_resp_send_500(req);
    return ESP_FAIL;
  }
  
  String recognitionResult = recognizeObject(fb);
  int recognizedCount = 0;
  int unknownCount = 0;
  String objectsJson = "";
  
  if (recognitionResult != "unknown") {
    recognizedCount = 1;
    int x = 30 + random(40);
    int y = 30 + random(40);
    int width = 20 + random(20);
    int height = 20 + random(20);
    
    objectsJson = String("{\"x\":") + x + ",\"y\":" + y + ",\"width\":" + width + 
                  ",\"height\":" + height + ",\"name\":\"" + recognitionResult + 
                  "\",\"recognized\":true}";
  } else {
    unknownCount = 1;
    int x = 30 + random(40);
    int y = 30 + random(40);
    int width = 20 + random(20);
    int height = 20 + random(20);
    
    objectsJson = String("{\"x\":") + x + ",\"y\":" + y + ",\"width\":" + width + 
                  ",\"height\":" + height + ",\"name\":\"Unknown Object\",\"recognized\":false}";
  }
  
  esp_camera_fb_return(fb);
  
  httpd_resp_set_type(req, "application/json");
  char response[300];
  snprintf(response, sizeof(response),
           "{\"recognizedCount\":%d,\"unknownCount\":%d,\"objects\":[%s]}",
           recognizedCount, unknownCount, objectsJson.c_str());
  
  return httpd_resp_send(req, response, strlen(response));
}

static esp_err_t get_trained_objects_handler(httpd_req_t *req) {
  String objectsJson = "";
  for(int i = 0; i < patternCount; i++) {
    if(objectsJson.length() > 0) objectsJson += ",";
    objectsJson += "{\"name\":\"" + String(knownPatterns[i].name) + "\"}";
  }
  
  httpd_resp_set_type(req, "application/json");
  char response[500];
  snprintf(response, sizeof(response), "{\"objects\":[%s]}", objectsJson.c_str());
  
  return httpd_resp_send(req, response, strlen(response));
}

void startCameraServer() {
  httpd_config_t config = HTTPD_DEFAULT_CONFIG();
  config.server_port = 80;
  
  httpd_uri_t index_uri = {
    .uri = "/",
    .method = HTTP_GET,
    .handler = index_handler,
    .user_ctx = NULL
  };
  
  httpd_uri_t train_object_uri = {
    .uri = "/trainObject",
    .method = HTTP_GET,
    .handler = train_object_handler,
    .user_ctx = NULL
  };
  
  httpd_uri_t analyze_objects_uri = {
    .uri = "/analyzeObjects",
    .method = HTTP_GET,
    .handler = analyze_objects_handler,
    .user_ctx = NULL
  };
  
  httpd_uri_t get_trained_objects_uri = {
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
    httpd_register_uri_handler(camera_httpd, &train_object_uri);
    httpd_register_uri_handler(camera_httpd, &analyze_objects_uri);
    httpd_register_uri_handler(camera_httpd, &get_trained_objects_uri);
  }
  
  config.server_port += 1;
  config.ctrl_port += 1;
  if (httpd_start(&stream_httpd, &config) == ESP_OK) {
    httpd_register_uri_handler(stream_httpd, &stream_uri);
  }
}

void setup() {
  WRITE_PERI_REG(RTC_CNTL_BROWN_OUT_REG, 0);
  Serial.begin(115200);
  
  EEPROM.begin(EEPROM_SIZE);
  loadPatternsFromEEPROM();
  
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
  
  if(psramFound()){
    config.frame_size = FRAMESIZE_VGA;
    config.jpeg_quality = 10;
    config.fb_count = 2;
  } else {
    config.frame_size = FRAMESIZE_SVGA;
    config.jpeg_quality = 12;
    config.fb_count = 1;
  }
  
  esp_err_t err = esp_camera_init(&config);
  if (err != ESP_OK) {
    Serial.printf("Camera init failed with error 0x%x", err);
    return;
  }
  
  WiFi.begin(ssid, password);
  WiFi.setSleep(false);
  
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected");
  
  startCameraServer();
  
  Serial.print("Camera Ready! Use 'http://");
  Serial.print(WiFi.localIP());
  Serial.println("' to connect");
  Serial.print("Trained Objects: ");
  Serial.println(patternCount);
}

void loop() {
  delay(1000);
}
