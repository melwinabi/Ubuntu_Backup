    // TickType_t xLastWakeTime = xTaskGetTickCount();
    // const TickType_t xFrequency = pdMS_TO_TICKS(100);
    // const TickType_t xFrequency_Sd = pdMS_TO_TICKS(1000);
    // SensorReading_t reading;

    // while (1) {
    //     vTaskDelayUntil(&xLastWakeTime, xFrequency_Sd); 
    //      float temp = 25.0f + ((float)random(0, 100) / 10.0f);
    //      float hum  = 61.0f + ((float)random(0, 100) / 10.0f);
    //     // Run strictly every 100 ms using vTaskDelayUntil
    //     vTaskDelayUntil(&xLastWakeTime, xFrequency); 

    //     // Generate reading
    //     reading.temperature = temp;
    //     reading.humidity = hum;
    //     reading.timestamp_ms = pdTICKS_TO_MS(xTaskGetTickCount());
    //     reading.sensor_id = 1;


/* task1_rtos_skeleton.c  */
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "freertos/semphr.h"
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
    uint32_t last_seen_ms[3];        /* indexed by task slot */
    UBaseType_t high_water_marks[3]; /* Per-task high-water mark */
} SystemHealth_t;

static QueueHandle_t      xSensorQueue;
static SemaphoreHandle_t  xStatsMutex;
static SystemHealth_t     gHealth;

/* Task Handles needed for high-water marks */
TaskHandle_t xSensorTaskHandle;
TaskHandle_t xNetTaskHandle;
TaskHandle_t xWatchdogTaskHandle;

/* Task A: Sensor Reader (High Priority) */
static void vSensorReaderTask(void *pvParameters) {
    TickType_t xLastWakeTime = xTaskGetTickCount();
    const TickType_t xFrequency = pdMS_TO_TICKS(100);
    const TickType_t xFrequency_Sd = pdMS_TO_TICKS(1000);
    SensorReading_t reading;

    while (1) {
        vTaskDelayUntil(&xLastWakeTime, xFrequency_Sd); 
         float temp = 25.0f + ((float)random(0, 100) / 10.0f);
         float hum  = 61.0f + ((float)random(0, 100) / 10.0f);
        vTaskDelayUntil(&xLastWakeTime, xFrequency); 
        reading.temperature = temp;
        reading.humidity = hum;
        reading.timestamp_ms = pdTICKS_TO_MS(xTaskGetTickCount());
        reading.sensor_id = 1;

        BaseType_t qStatus = xQueueSend(xSensorQueue, &reading, 0); // Non-blocking send

        if (xSemaphoreTake(xStatsMutex, portMAX_DELAY) == pdTRUE) {
            if (qStatus != pdPASS) {
                gHealth.drop_count++; // Increment drop counter if full
            }
            gHealth.last_seen_ms[0] = pdTICKS_TO_MS(xTaskGetTickCount());
            xSemaphoreGive(xStatsMutex);
        }
    }
}

/* Task B: Network Publisher (Medium Priority) */
static void vNetPublisherTask(void *pvParameters) {
    SensorReading_t reading;
    char json_payload[64];

    while (1) {
        // Blocks indefinitely on the queue
        if (xQueueReceive(xSensorQueue, &reading, portMAX_DELAY) == pdTRUE) {
            
            // Format data as compact JSON
            snprintf(json_payload, sizeof(json_payload), "{\"t\":%.1f,\"h\":%.1f,\"ts\":%u}", 
                     reading.temperature, reading.humidity, (unsigned int)reading.timestamp_ms);

            // Writes formatted string to UART as a stand-in for real MQTT
            printf("MQTT_PUB: %s\n", json_payload);

            if (xSemaphoreTake(xStatsMutex, portMAX_DELAY) == pdTRUE) {
                gHealth.publish_count++; 
                gHealth.last_seen_ms[1] = pdTICKS_TO_MS(xTaskGetTickCount());
                xSemaphoreGive(xStatsMutex);
            }
        }
    }
}

/* Task C: Watchdog / Health Monitor (Low Priority) */
static void vWatchdogTask(void *pvParameters) {
    TickType_t xLastWakeTime = xTaskGetTickCount();
    const TickType_t xFrequency = pdMS_TO_TICKS(1000);

    while (1) {
        vTaskDelayUntil(&xLastWakeTime, xFrequency); // Runs exactly every 1000ms

        uint32_t current_time = pdTICKS_TO_MS(xTaskGetTickCount());

        if (xSemaphoreTake(xStatsMutex, portMAX_DELAY) == pdTRUE) {
            gHealth.last_seen_ms[2] = current_time; // Update its own timestamp
            
            // 1. Checks whether each task updated its timestamp within 500 ms
            for (int i = 0; i < 3; i++) {
                if ((current_time - gHealth.last_seen_ms[i]) > 500) {
                    printf("WARNING: Task %d is unresponsive! (Last seen %u ms ago)\n", 
                           i, (unsigned int)(current_time - gHealth.last_seen_ms[i]));
                }
            }

            // 2. Read high-water marks for EACH task
            gHealth.high_water_marks[0] = uxTaskGetStackHighWaterMark(xSensorTaskHandle);
            gHealth.high_water_marks[1] = uxTaskGetStackHighWaterMark(xNetTaskHandle);
            gHealth.high_water_marks[2] = uxTaskGetStackHighWaterMark(NULL); // Self

            // 3. Print one-line status summary
            printf("STATUS | Pub: %u | Drop: %u | Min Stack A:%u B:%u C:%u\n", 
                   (unsigned int)gHealth.publish_count, 
                   (unsigned int)gHealth.drop_count, 
                   (unsigned int)gHealth.high_water_marks[0],
                   (unsigned int)gHealth.high_water_marks[1],
                   (unsigned int)gHealth.high_water_marks[2]);
            
            xSemaphoreGive(xStatsMutex);
        }
    }
}

void setup(void) {
    xSensorQueue = xQueueCreate(8, sizeof(SensorReading_t));
    xStatsMutex  = xSemaphoreCreateMutex();

    // Assert non-NULL
    if (xSensorQueue != NULL && xStatsMutex != NULL) {
        
        // Priority: HIGH (3), MEDIUM (2), LOW (1)
        xTaskCreate(vSensorReaderTask, "Sensor",   2048, NULL, 3, &xSensorTaskHandle);
        xTaskCreate(vNetPublisherTask, "NetPub",   3072, NULL, 2, &xNetTaskHandle); 
        xTaskCreate(vWatchdogTask,     "Watchdog", 2048, NULL, 1, &xWatchdogTaskHandle);
        
    } else {
        printf("Failed to create RTOS primitives!\n");
    }
}

void loop() {
    // The loop task is empty because FreeRTOS tasks handle everything.
    // We delay to prevent the loop task from eating up CPU cycles.
    vTaskDelay(pdMS_TO_TICKS(1000));
}        