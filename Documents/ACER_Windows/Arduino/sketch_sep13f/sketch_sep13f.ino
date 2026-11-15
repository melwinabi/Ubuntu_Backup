#include <SoftwareSerial.h>

SoftwareSerial rs485(6, 4); // RX, TX

#define RS485CommControl 5
#define RS485Transmit    HIGH
#define RS485Receive     LOW

uint8_t response[50];  // Buffer for Modbus response
int index;

float totaliser_1, flow_1, totaliser_2, flow_2;

void setup() {
  Serial.begin(9600);
  rs485.begin(9600);
  pinMode(RS485CommControl, OUTPUT);
  digitalWrite(RS485CommControl, RS485Receive);
  Serial.println("Started");
  sendFMRequest();
}

void loop() {
  if (rs485.available()) {
    index = 0;
    while (rs485.available() && index < 50) {
      response[index] = rs485.read();
      //Serial.print(response[index], HEX); Serial.print(" ");
      index++;
    }
    //Serial.println("\nINDEX: " + String(index));
    delay(50);
    decodeFT650();
  }

  sendFMRequest();
  delay(1000);
}

void sendFMRequest() {
  //Serial.println("FM Request Sent");
  digitalWrite(RS485CommControl, RS485Transmit);
  delay(10);
  uint8_t request[] = { 0x01, 0x03, 0x00, 0x00, 0x00, 0x14, 0x45, 0xC5 }; // Read 20 registers
  rs485.write(request, sizeof(request));
  delay(10);
  digitalWrite(RS485CommControl, RS485Receive);
  delay(10);
}

uint32_t combineRegisters(uint16_t msw, uint16_t lsw) {
  return ((uint32_t)msw << 16) | lsw;
}

void decodeFT650() {
  if (index >= 45 && response[1] == 0x03) {
    // Read registers from index 3 onward (3 = byte count start)
    uint16_t total2_lsw   = (response[3+28] << 8) | response[3+29]; // 40017
    uint16_t total2_msw   = (response[3+30] << 8) | response[3+31]; // 40018
    uint16_t flow1_lsw    = (response[3+24] << 8) | response[3+25]; // 40015
    uint16_t flow1_msw    = (response[3+26] << 8) | response[3+27]; // 40016

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
