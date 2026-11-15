#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "freertos/semphr.h"
#include <stdio.h>

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

TaskHandle_t xSensorTaskHandle;
TaskHandle_t xNetTaskHandle;
TaskHandle_t xWatchdogTaskHandle;

static void vSensorReaderTask(void *pvParameters) {
    TickType_t xLastWakeTime = xTaskGetTickCount();
    const TickType_t xFrequency = pdMS_TO_TICKS(100);
    SensorReading_t reading;

    while (1) {
        vTaskDelayUntil(&xLastWakeTime, xFrequency);

        reading.temperature = 25.3f;
        reading.humidity = 61.0f;
        reading.timestamp_ms = pdTICKS_TO_MS(xTaskGetTickCount());
        reading.sensor_id = 1;

        BaseType_t qStatus = xQueueSend(xSensorQueue, &reading, 0);

        if (xSemaphoreTake(xStatsMutex, portMAX_DELAY) == pdTRUE) {
            if (qStatus != pdPASS) { //if failed
                gHealth.drop_count++;
            }
            gHealth.last_seen_ms[0] = reading.timestamp_ms;
            xSemaphoreGive(xStatsMutex);
        }
    }
}

static void vNetPublisherTask(void *pvParameters) {
    SensorReading_t reading;

    while (1) {
        if (xQueueReceive(xSensorQueue, &reading, portMAX_DELAY) == pdTRUE) {
            printf("{\"t\":%.1f,\"h\":%.1f,\"ts\":%u}\n", 
                   reading.temperature, reading.humidity, reading.timestamp_ms);

            if (xSemaphoreTake(xStatsMutex, portMAX_DELAY) == pdTRUE) {
                gHealth.publish_count++;
                gHealth.last_seen_ms[1] = pdTICKS_TO_MS(xTaskGetTickCount());
                xSemaphoreGive(xStatsMutex);
            }
        }
    }
}

static void vWatchdogTask(void *pvParameters) {
    TickType_t xLastWakeTime = xTaskGetTickCount();
    const TickType_t xFrequency = pdMS_TO_TICKS(1000);

    while (1) {
        vTaskDelayUntil(&xLastWakeTime, xFrequency);

        uint32_t current_time = pdTICKS_TO_MS(xTaskGetTickCount());
        uint32_t local_publish = 0;
        uint32_t local_drop = 0;
        UBaseType_t min_stack = 0xFFFFFFFF;

        gHealth.high_water_marks[0] = uxTaskGetStackHighWaterMark(xSensorTaskHandle);
        gHealth.high_water_marks[1] = uxTaskGetStackHighWaterMark(xNetTaskHandle);
        gHealth.high_water_marks[2] = uxTaskGetStackHighWaterMark(xWatchdogTaskHandle);

        if (xSemaphoreTake(xStatsMutex, portMAX_DELAY) == pdTRUE) {
            gHealth.last_seen_ms[2] = current_time;

            for (int i = 0; i < 3; i++) {
                if ((current_time - gHealth.last_seen_ms[i]) > 500) {
                    printf("WARNING: Task %d is stale! Last seen: %u ms\n", i, gHealth.last_seen_ms[i]);
                }
                
                if (gHealth.high_water_marks[i] < min_stack) {
                    min_stack = gHealth.high_water_marks[i];
                }
            }

            local_publish = gHealth.publish_count;
            local_drop = gHealth.drop_count;

            xSemaphoreGive(xStatsMutex);
        }

        printf("STATUS | Pub: %u | Drop: %u | Min Stack Remaining: %u words\n", 
               local_publish, local_drop, min_stack);
    }
}

void app_main(void) {
    xSensorQueue = xQueueCreate(8, sizeof(SensorReading_t));
    xStatsMutex  = xSemaphoreCreateMutex();

    if (xSensorQueue != NULL && xStatsMutex != NULL) {
        xTaskCreate(vSensorReaderTask, "Sensor", 2048, NULL, 3, &xSensorTaskHandle);
        
        xTaskCreate(vNetPublisherTask, "NetPub", 3072, NULL, 2, &xNetTaskHandle);
        
        xTaskCreate(vWatchdogTask, "Watchdog", 2048, NULL, 1, &xWatchdogTaskHandle);
    } else {
        printf("Failed to create Queue or Mutex!\n");
    }
}