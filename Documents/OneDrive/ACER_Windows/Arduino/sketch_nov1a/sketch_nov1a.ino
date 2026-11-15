#define RS485CommControl 15     // DE/RE pin
#define RS485Transmit    HIGH
#define RS485Receive     LOW

#define RXD2 16                 // ESP32 UART2 RX pin
#define TXD2 17                 // ESP32 UART2 TX pin

uint8_t response[50];  // Buffer for Modbus response
int responseIndex = 0;

float totaliser_1, flow_1, totaliser_2, flow_2;

void setup() {
  Serial.begin(9600);  // USB monitor
  Serial2.begin(9600, SERIAL_8N1, RXD2, TXD2);  // UART2 for RS485

  pinMode(RS485CommControl, OUTPUT);
  digitalWrite(RS485CommControl, RS485Receive);

  Serial.println("ESP32 RS485 Modbus Started");
  sendFMRequest();
}

void loop() {
  if (Serial2.available()) {
    responseIndex = 0;
    while (Serial2.available() && responseIndex < 50) {
      response[responseIndex] = Serial2.read();
      responseIndex++;
    }
    delay(50);
    decodeFT650();
  }

  sendFMRequest();
  delay(1000);
}

void sendFMRequest() {
  digitalWrite(RS485CommControl, RS485Transmit);
  delay(10);

  uint8_t request[] = { 0x01, 0x03, 0x00, 0x00, 0x00, 0x14, 0x45, 0xC5 }; // Read 20 registers
  Serial2.write(request, sizeof(request));

  delay(10);
  digitalWrite(RS485CommControl, RS485Receive);
  delay(10);
}

uint32_t combineRegisters(uint16_t msw, uint16_t lsw) {
  return ((uint32_t)msw << 16) | lsw;
}

void decodeFT650() {
  if (responseIndex >= 45 && response[1] == 0x03) {
    uint16_t total2_lsw = (response[3+28] << 8) | response[3+29]; // 40017
    uint16_t total2_msw = (response[3+30] << 8) | response[3+31]; // 40018
    uint16_t flow1_lsw  = (response[3+24] << 8) | response[3+25]; // 40015
    uint16_t flow1_msw  = (response[3+26] << 8) | response[3+27]; // 40016

    flow_1      = combineRegisters(flow1_msw, flow1_lsw) * 0.01;
    totaliser_2 = combineRegisters(total2_msw, total2_lsw) * 0.01;

    Serial.print("Totaliser 1: "); Serial.println(totaliser_2, 2);
    Serial.print("Flow 1:      "); Serial.println(flow_1, 2);
    Serial.println("");
  } else {
    flow_1      = 0;
    totaliser_2 = 0;
  }
}
