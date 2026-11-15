#include <ModbusMaster.h>
#include <SoftwareSerial.h>

// RS485 on pins 6 = RX, 4 = TX
SoftwareSerial RS485Serial(4, 6);

ModbusMaster node;

// MAX485 control pins
const int MAX485_DE = 3;
const int MAX485_RE = 2;

void preTransmission() {
  digitalWrite(MAX485_DE, 1);
  digitalWrite(MAX485_RE, 1);
}

void postTransmission() {
  digitalWrite(MAX485_DE, 0);
  digitalWrite(MAX485_RE, 0);
}

void setup() {
  Serial.begin(9600);        // Monitor
  RS485Serial.begin(9600);   // RS485 Modbus

  pinMode(MAX485_RE, OUTPUT);
  pinMode(MAX485_DE, OUTPUT);
  digitalWrite(MAX485_RE, 0);
  digitalWrite(MAX485_DE, 0);

  node.begin(1, RS485Serial); // temporary slave ID
  node.preTransmission(preTransmission);
  node.postTransmission(postTransmission);

  Serial.println("Scanning for MFM384 Device ID...");
}

void loop() {
  for (uint8_t id = 1; id <= 247; id++) {
    node.begin(id, RS485Serial);

    // Try reading holding register 6 (40007 in manual, Slave ID register)
    uint8_t result = node.readHoldingRegisters(6, 1);

    if (result == node.ku8MBSuccess) {
      Serial.print("✅ Device found! Slave ID = ");
      Serial.println(id);
      Serial.print("Register[6] value: ");
      Serial.println(node.getResponseBuffer(0));
      while (1); // stop scanning once found
    }
  }

  Serial.println("No device found on RS485 bus.");
  delay(5000);
}
