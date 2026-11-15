#include <SPI.h>
#include <LoRa.h>

// ===== LoRa Pins =====
#define LORA_SS    5
#define LORA_RST   14
#define LORA_DIO0  26

String serialBuffer = "";
String loraBuffer   = "";

void setup() {
  Serial.begin(115200);
  while (!Serial);

  Serial.println("ESP32 Serial <-> LoRa Bridge");

  // SPI (VSPI)
  SPI.begin(18, 19, 23, LORA_SS);

  // LoRa pins
  LoRa.setPins(LORA_SS, LORA_RST, LORA_DIO0);

  if (!LoRa.begin(433E6)) {
    Serial.println("LoRa init failed");
    while (1);
  }

  // Must match TX node
  LoRa.setSpreadingFactor(7);
  LoRa.setSignalBandwidth(125E3);
  LoRa.setCodingRate4(5);
  LoRa.enableCrc();

  Serial.println("LoRa Ready (RX always ON)");
}

void loop() {
  serialToLoRa();   // Send serial data via LoRa
  loRaToSerial();   // Receive LoRa data
}

/* ---------- SERIAL → LORA ---------- */
void serialToLoRa() {
  while (Serial.available()) {
    char c = Serial.read();

    if (c == '\n') {            // end of serial message
      sendLoRa(serialBuffer);
      serialBuffer = "";        // clear buffer
    }
    else {
      serialBuffer += c;
    }
  }
}

void sendLoRa(String msg) {
  if (msg.length() == 0) return;

  Serial.println("TX LORA -> " + msg);

  LoRa.beginPacket();
  LoRa.print(msg);
  LoRa.endPacket();             // blocking TX
}

/* ---------- LORA → SERIAL ---------- */
void loRaToSerial() {
  int packetSize = LoRa.parsePacket();

  if (packetSize) {
    loraBuffer = "";

    while (LoRa.available()) {
      loraBuffer += (char)LoRa.read();
    }

    Serial.println("RX LORA -> " + loraBuffer);
    Serial.print("RSSI: ");
    Serial.println(LoRa.packetRssi());
    Serial.println("----------------------");
  }
}

//@@*3002854*1002883*01*000