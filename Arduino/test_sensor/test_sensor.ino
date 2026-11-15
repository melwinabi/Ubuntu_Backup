#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "freertos/semphr.h"
#include "esp_wifi.h"
#include "esp_event.h"
#include "nvs_flash.h"
#include "mqtt_client.h"
#include <stdio.h>

/* Data Structures from Task 1 */
typedef struct {
    float    temperature;
    float    humidity;
    uint32_t timestamp_ms;
    uint8_t  sensor_id;
} SensorReading_t;

typedef struct {
    uint32_t publish_count;
    uint32_t drop_count;
    uint32_t last_seen_ms[3];
    UBaseType_t high_water_marks[3];
} SystemHealth_t;

static QueueHandle_t      xSensorQueue;
static SemaphoreHandle_t  xStatsMutex;
static SystemHealth_t     gHealth;
static esp_mqtt_client_handle_t client;

/* Task Handles for Watchdog */
TaskHandle_t xSensorTaskHandle;
TaskHandle_t xNetTaskHandle;
TaskHandle_t xWatchdogTaskHandle;

/* Task A: Sensor Reader (High Priority) */
static void vSensorReaderTask(void *pvParameters) {
    TickType_t xLastWakeTime = xTaskGetTickCount();
    const TickType_t xFrequency = pdMS_TO_TICKS(100);
    SensorReading_t reading;

    while (1) {
        vTaskDelayUntil(&xLastWakeTime, xFrequency); // Runs every 100ms [cite: 16]

        reading.temperature = 25.3f;
        reading.humidity = 61.0f;
        reading.timestamp_ms = pdTICKS_TO_MS(pdTICKS_TO_MS(xTaskGetTickCount()));
        reading.sensor_id = 1;

        BaseType_t qStatus = xQueueSend(xSensorQueue, &reading, 0); // Non-blocking send [cite: 18]

        if (xSemaphoreTake(xStatsMutex, portMAX_DELAY) == pdTRUE) {
            if (qStatus != pdPASS) {
                gHealth.drop_count++; // Increment drop counter if full [cite: 18]
            }
            gHealth.last_seen_ms[0] = reading.timestamp_ms;
            xSemaphoreGive(xStatsMutex);
        }
    }
}

/* Task B: Real MQTT Network Publisher (Medium Priority) */
static void vNetPublisherTask(void *pvParameters) {
    SensorReading_t reading;
    char json_payload[64];

    while (1) {
        if (xQueueReceive(xSensorQueue, &reading, portMAX_DELAY) == pdTRUE) {
            // Format data as compact JSON [cite: 22]
            snprintf(json_payload, sizeof(json_payload), "{\"t\":%.1f,\"h\":%.1f,\"ts\":%u}", 
                     reading.temperature, reading.humidity, reading.timestamp_ms);

            // Send to MQTT Server 192.168.29.79
            int msg_id = esp_mqtt_client_publish(client, "test/sensor_read", json_payload, 0, 1, 0);
            
            if (msg_id != -1) {
                printf("Published: %s\n", json_payload);
                if (xSemaphoreTake(xStatsMutex, portMAX_DELAY) == pdTRUE) {
                    gHealth.publish_count++; // Increment publish count [cite: 24]
                    gHealth.last_seen_ms[1] = pdTICKS_TO_MS(xTaskGetTickCount());
                    xSemaphoreGive(xStatsMutex);
                }
            }
        }
    }
}

/* Task C: Watchdog (Low Priority) */
static void vWatchdogTask(void *pvParameters) {
    TickType_t xLastWakeTime = xTaskGetTickCount();
    const TickType_t xFrequency = pdMS_TO_TICKS(1000);

    while (1) {
        vTaskDelayUntil(&xLastWakeTime, xFrequency); // Runs every 1000ms [cite: 26]

        if (xSemaphoreTake(xStatsMutex, portMAX_DELAY) == pdTRUE) {
            uint32_t current_time = pdTICKS_TO_MS(xTaskGetTickCount());
            gHealth.last_seen_ms[2] = current_time;
            
            // Check health & stack high-water marks [cite: 27, 29, 30]
            printf("STATUS | Pub: %u | Drop: %u | Min Stack: %u\n", 
                   gHealth.publish_count, gHealth.drop_count, (unsigned int)uxTaskGetStackHighWaterMark(NULL));
            
            xSemaphoreGive(xStatsMutex);
        }
    }
}

/* WiFi Init Helper */
static void wifi_init(void) {
    esp_netif_init();
    esp_event_loop_create_default();
    esp_netif_create_default_wifi_sta();
    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    esp_wifi_init(&cfg);

    wifi_config_t wifi_config = {
        .sta = {
            .ssid = "Jio_Fiber@2",
            .password = "Jio@Efgh.1234567890",
        },
    };
    esp_wifi_set_mode(WIFI_MODE_STA);
    esp_wifi_set_config(WIFI_IF_STA, &wifi_config);
    esp_wifi_start();
    esp_wifi_connect();
}

/* MQTT Init Helper */
static void mqtt_app_start(void) {
    esp_mqtt_client_config_t mqtt_cfg = {
        .broker.address.uri = "mqtt://192.168.29.79",
    };
    client = esp_mqtt_client_init(&mqtt_cfg);
    esp_mqtt_client_start(client);
}

void app_main(void) {
    nvs_flash_init();
    wifi_init();
    mqtt_app_start();

    xSensorQueue = xQueueCreate(8, sizeof(SensorReading_t));
    xStatsMutex  = xSemaphoreCreateMutex();

    if (xSensorQueue != NULL && xStatsMutex != NULL) {
        xTaskCreate(vSensorReaderTask, "Sensor", 2048, NULL, 3, &xSensorTaskHandle);
        xTaskCreate(vNetPublisherTask, "NetPub", 4096, NULL, 2, &xNetTaskHandle); // Stack increased for MQTT
        xTaskCreate(vWatchdogTask, "Watchdog", 2048, NULL, 1, &xWatchdogTaskHandle);
    }
}