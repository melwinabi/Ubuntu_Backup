#include <Arduino.h>
#include <WiFi.h>
#include <PubSubClient.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "freertos/semphr.h"
#include <stdio.h>

// --- Network & MQTT Credentials ---
const char* ssid = "Jio_Fiber@2";
const char* password = "Jio@Efgh.1234567890";
// PubSubClient requires just the IP string, without the mqtt:// prefix
const char* mqtt_server = "192.168.29.79"; 
const char* mqtt_topic = "test/sensor_read";

WiFiClient espClient;
PubSubClient client(espClient);

// --- Data Structures ---
typedef struct {
    float    temperature;
    float    humidity;
    uint32_t timestamp_ms;
    uint8_t  sensor_id;
} SensorReading_t;

typedef struct {
    uint32_t publish_count;
    uint32_t drop_count;
    uint32_t last_seen_ms[3];  /* indexed by task slot: 0=Sensor, 1=NetPub, 2=Watchdog */
    UBaseType_t high_water_marks[3]; 
} SystemHealth_t;

static QueueHandle_t      xSensorQueue;
static SemaphoreHandle_t  xStatsMutex;
static SystemHealth_t     gHealth;

TaskHandle_t xSensorTaskHandle;
TaskHandle_t xNetTaskHandle;
TaskHandle_t xWatchdogTaskHandle;

// --- MQTT Reconnect Function ---
void reconnect() {
    while (!client.connected()) {
        Serial.print("Attempting MQTT connection...");
        String clientId = "ESP32Client-";
        clientId += String(random(0xffff), HEX);
        
        if (client.connect(clientId.c_str())) {
            Serial.println("connected");
        } else {
            Serial.print("failed, rc=");
            Serial.print(client.state());
            Serial.println(" try again in 5 seconds");
            
            // Feed the watchdog during reconnect delay to avoid false warnings
            for(int i = 0; i < 50; i++) {
                if (xSemaphoreTake(xStatsMutex, portMAX_DELAY) == pdTRUE) {
                    gHealth.last_seen_ms[1] = pdTICKS_TO_MS(xTaskGetTickCount());
                    xSemaphoreGive(xStatsMutex);
                }
                vTaskDelay(pdMS_TO_TICKS(100));
            }
        }
    }
}

// --- Task A: Sensor Reader (HIGH Priority) ---
static void vSensorReaderTask(void *pvParameters) {
    TickType_t xLastWakeTime = xTaskGetTickCount();
    const TickType_t xFrequency = pdMS_TO_TICKS(100);
    SensorReading_t reading;

    while (1) {
        // Run strictly every 100 ms using vTaskDelayUntil
        vTaskDelayUntil(&xLastWakeTime, xFrequency); 

        // Generate reading
        reading.temperature = 25.0f + ((float)random(0, 100) / 10.0f);
        reading.humidity = 61.0f + ((float)random(0, 100) / 10.0f);
        reading.timestamp_ms = pdTICKS_TO_MS(xTaskGetTickCount());
        reading.sensor_id = 1;

        // Non-blocking send (timeout = 0)
        BaseType_t qStatus = xQueueSend(xSensorQueue, &reading, 0); 

        if (xSemaphoreTake(xStatsMutex, portMAX_DELAY) == pdTRUE) {
            if (qStatus != pdPASS) {
                gHealth.drop_count++; // Increment drop counter safely
            }
            gHealth.last_seen_ms[0] = pdTICKS_TO_MS(xTaskGetTickCount());
            xSemaphoreGive(xStatsMutex);
        }
    }
}

// --- Task B: Network Publisher (MEDIUM Priority) ---
static void vNetPublisherTask(void *pvParameters) {
    SensorReading_t reading;
    char json_payload[128];

    Serial.print("Connecting to WiFi: ");
    Serial.println(ssid);
    WiFi.begin(ssid, password);
    
    while (WiFi.status() != WL_CONNECTED) {
        // Feed watchdog during WiFi connection
        if (xSemaphoreTake(xStatsMutex, portMAX_DELAY) == pdTRUE) {
            gHealth.last_seen_ms[1] = pdTICKS_TO_MS(xTaskGetTickCount());
            xSemaphoreGive(xStatsMutex);
        }
        vTaskDelay(pdMS_TO_TICKS(100));
        Serial.print(".");
    }
    Serial.println("\nWiFi connected.");
    
    client.setServer(mqtt_server, 1883);

    while (1) {
        // Blocks indefinitely on the queue using xQueueReceive(... portMAX_DELAY)
        if (xQueueReceive(xSensorQueue, &reading, portMAX_DELAY) == pdTRUE) {
            
            if (!client.connected()) {
                reconnect();
            }
            client.loop(); // maintain MQTT connection
            
            snprintf(json_payload, sizeof(json_payload), "{\"t\":%.1f,\"h\":%.1f,\"ts\":%u}", 
                     reading.temperature, reading.humidity, (unsigned int)reading.timestamp_ms);

            Serial.printf("MQTT_PUB: %s\n", json_payload);
            
            bool published = client.publish(mqtt_topic, json_payload);

            if (xSemaphoreTake(xStatsMutex, portMAX_DELAY) == pdTRUE) {
                if (published) {
                    gHealth.publish_count++; 
                }
                gHealth.last_seen_ms[1] = pdTICKS_TO_MS(xTaskGetTickCount());
                xSemaphoreGive(xStatsMutex);
            }
        }
    }
}

// --- Task C: Watchdog / Health Monitor (LOW Priority) ---
static void vWatchdogTask(void *pvParameters) {
    TickType_t xLastWakeTime = xTaskGetTickCount();
    const TickType_t xFrequency = pdMS_TO_TICKS(1000);

    while (1) {
        vTaskDelayUntil(&xLastWakeTime, xFrequency); 

        uint32_t current_time = pdTICKS_TO_MS(xTaskGetTickCount());
        uint32_t copy_pub = 0, copy_drop = 0;
        bool stale[3] = {false, false, false};
        uint32_t time_since[3] = {0, 0, 0};

        if (xSemaphoreTake(xStatsMutex, portMAX_DELAY) == pdTRUE) {
            gHealth.last_seen_ms[2] = current_time; 
            
            for (int i = 0; i < 3; i++) {
                time_since[i] = current_time - gHealth.last_seen_ms[i];
                if (time_since[i] > 500) {
                    stale[i] = true;
                }
            }

            gHealth.high_water_marks[0] = uxTaskGetStackHighWaterMark(xSensorTaskHandle);
            gHealth.high_water_marks[1] = uxTaskGetStackHighWaterMark(xNetTaskHandle);
            gHealth.high_water_marks[2] = uxTaskGetStackHighWaterMark(NULL); 

            copy_pub = gHealth.publish_count;
            copy_drop = gHealth.drop_count;

            xSemaphoreGive(xStatsMutex);
        }
        
        for (int i = 0; i < 3; i++) {
            if (stale[i]) {
                Serial.printf("WARNING: Task %d is unresponsive! (Last seen %u ms ago)\n", 
                       i, (unsigned int)time_since[i]);
            }
        }

        Serial.printf("STATUS | Pub: %u | Drop: %u | Min Stack A:%u B:%u C:%u\n", 
               (unsigned int)copy_pub, 
               (unsigned int)copy_drop, 
               (unsigned int)gHealth.high_water_marks[0],
               (unsigned int)gHealth.high_water_marks[1],
               (unsigned int)gHealth.high_water_marks[2]);
    }
}

void setup(void) {
    Serial.begin(115200);
    delay(1000); 

    xSensorQueue = xQueueCreate(8, sizeof(SensorReading_t));
    xStatsMutex  = xSemaphoreCreateMutex();

    if (xSensorQueue != NULL && xStatsMutex != NULL) {
        
        // Stack Size Justification:
        // Sensor (2048): Minimal memory footprint, only reads mock data and queues it.
        // NetPub (8192): Requires a larger stack because WiFi and MQTT libraries allocate 
        //                large buffers locally during connection and string parsing.
        // Watchdog (2048): Minimal stack needs, only reads variables and prints to Serial.
        
        xTaskCreate(vSensorReaderTask, "Sensor",   2048, NULL, 3, &xSensorTaskHandle);
        xTaskCreate(vNetPublisherTask, "NetPub",   8192, NULL, 2, &xNetTaskHandle); 
        xTaskCreate(vWatchdogTask,     "Watchdog", 2048, NULL, 1, &xWatchdogTaskHandle);
        
    } else {
        Serial.printf("Failed to create RTOS primitives!\n");
    }
}

void loop() {
    vTaskDelete(NULL); // Free memory allocated for the loop task
}
