#include <driver/adc.h>
#include <driver/dma.h>

#define ADC_CHANNEL ADC1_CHANNEL_0  // ADC1 Channel 0 (GPIO36)
#define DMA_BUFFER_SIZE 1024        // Buffer size for DMA

uint16_t dmaBuffer[DMA_BUFFER_SIZE];  // DMA buffer for ADC readings

void setup() {
  Serial.begin(115200);

  // Configure ADC
  adc1_config_width(ADC_WIDTH_BIT_12);
  adc1_config_channel_atten(ADC_CHANNEL, ADC_ATTEN_DB_0);

  // Configure DMA
  dma_config_t dmaConfig;
  dmaConfig.periph = ADC1_DMA;          // Peripheral connected to DMA
  dmaConfig.buffer = dmaBuffer;        // Buffer to store ADC readings
  dmaConfig.length = DMA_BUFFER_SIZE;  // Number of samples

  if (dma_init(&dmaConfig) != ESP_OK) {
    Serial.println("DMA initialization failed!");
    return;
  }

  Serial.println("DMA initialized. Starting ADC readings...");

  // Start DMA transfer
  dma_start_transfer(&dmaConfig);
}

void loop() {
  // Check if DMA has completed transferring data
  if (dma_is_transfer_complete()) {
    Serial.println("DMA Transfer Complete!");
    for (int i = 0; i < DMA_BUFFER_SIZE; i++) {
      Serial.println(dmaBuffer[i]);  // Print ADC values
    }

    // Restart DMA transfer
    dma_restart_transfer();
  }
}
