#include <Arduino.h>
#include <SPI.h>

#define EPD_CS   5
#define EPD_DC   17
#define EPD_RST  16
#define EPD_BUSY 4

#define SPI_SCK 18
#define SPI_MOSI 23

#define EPD_WIDTH 800
#define EPD_HEIGHT 480

void epd_write_command(uint8_t cmd) {
  digitalWrite(EPD_DC, LOW);
  digitalWrite(EPD_CS, LOW);
  SPI.transfer(cmd);
  digitalWrite(EPD_CS, HIGH);
}

void epd_write_data(uint8_t data) {
  digitalWrite(EPD_DC, HIGH);
  digitalWrite(EPD_CS, LOW);
  SPI.transfer(data);
  digitalWrite(EPD_CS, HIGH);
}

void epd_wait_busy() {
  while(digitalRead(EPD_BUSY) == HIGH) {
    delay(1);
  }
}

void epd_reset() {
  digitalWrite(EPD_RST, LOW);
  delay(200);
  digitalWrite(EPD_RST, HIGH);
  delay(200);
}

void epd_init() {
  epd_reset();
  epd_wait_busy();

  epd_write_command(0x01); // POWER_SETTING
  epd_write_data(0x07);
  epd_write_data(0x07);
  epd_write_data(0x3F);
  epd_write_data(0x3F);

  epd_write_command(0x04); // POWER_ON
  epd_wait_busy();

  epd_write_command(0x00); // PANEL_SETTING
  epd_write_data(0x1F);

  epd_write_command(0x61); // RESOLUTION
  epd_write_data((EPD_WIDTH >> 8) & 0xFF);
  epd_write_data(EPD_WIDTH & 0xFF);
  epd_write_data((EPD_HEIGHT >> 8) & 0xFF);
  epd_write_data(EPD_HEIGHT & 0xFF);
}

// Send a small row buffer
void epd_draw_rows(uint8_t *row_buf, int start_row, int row_count) {
  epd_write_command(0x10); // DATA_START_TRANSMISSION_1

  for (int y = 0; y < row_count; y++) {
    for (int x = 0; x < EPD_WIDTH/8; x++) {
      epd_write_data(row_buf[x]);
    }
  }

  epd_write_command(0x12); // DISPLAY_REFRESH
  epd_wait_busy();
}

void setup() {
  pinMode(EPD_CS, OUTPUT);
  pinMode(EPD_DC, OUTPUT);
  pinMode(EPD_RST, OUTPUT);
  pinMode(EPD_BUSY, INPUT);

  SPI.begin(SPI_SCK, -1, SPI_MOSI, -1); // HSPI

  Serial.begin(115200);
  Serial.println("Initializing ePaper...");
  epd_init();

  Serial.println("Drawing small pattern...");
  const int ROWS = 16;
  uint8_t row_buf[EPD_WIDTH/8];
  memset(row_buf, 0xFF, sizeof(row_buf)); // white background

  // Draw a black bar horizontally
  for (int x = 100; x < 500; x++) {
    row_buf[x/8] &= ~(0x80 >> (x%8));
  }

  // Send multiple rows
  for (int y = 100; y < 116; y++) { // 16 rows
    epd_draw_rows(row_buf, y, 1);
  }

  Serial.println("Done!");
}

void loop() {}
