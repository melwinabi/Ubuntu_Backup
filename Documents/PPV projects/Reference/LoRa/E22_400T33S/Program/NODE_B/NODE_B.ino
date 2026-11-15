#include <SoftwareSerial.h>
#include "LoRa_E22.h"

#define DESTINATION_ADDL 0
#define DESTINATION_CHAN 23
#define ENABLE_RSSI true

SoftwareSerial mySerial(3, 2); // RX, TX
LoRa_E22 e22ttl(&mySerial, 4, 5, 6); // AUX, M0, M1

String Message = "";
String Const_Data = "AbC1@EfG9#XyZ2$kLm!78nOpQ_+RsTuV3*W4jHiD<>?/|`~%=56";

void setup() {
  Serial.begin(9600);
  delay(500);
  e22ttl.begin();
  delay(500);

  pinMode(7, INPUT_PULLUP);

  ResponseStructContainer c = e22ttl.getConfiguration();
  Configuration cfg = *(Configuration*)c.data;

  Serial.println("---- LoRa E22 Config ----");
  Serial.print("STS: "); Serial.println(c.status.getResponseDescription());
  Serial.print("NID: "); Serial.println(cfg.NETID);
  Serial.print("CHN: "); Serial.println(cfg.CHAN);
  Serial.print("BRU: "); Serial.println(cfg.SPED.getUARTBaudRateDescription());
  Serial.print("PDT: "); Serial.println(cfg.SPED.getUARTParityDescription());
  Serial.print("ADR: "); Serial.println(cfg.SPED.getAirDataRateDescription());
  Serial.print("PKT: "); Serial.println(cfg.OPTION.getSubPacketSetting());
  Serial.print("PWR: "); Serial.println(cfg.OPTION.getTransmissionPowerDescription());
  Serial.print("LBT: "); Serial.println(cfg.TRANSMISSION_MODE.getLBTEnableByteDescription());
  Serial.print("REP: "); Serial.println(cfg.TRANSMISSION_MODE.enableRepeater ? "Yes" : "No");
  Serial.print("FIX: "); Serial.println(cfg.TRANSMISSION_MODE.fixedTransmission ? "Yes" : "No");
  Serial.print("RSS: "); Serial.println(cfg.TRANSMISSION_MODE.getRSSIEnableByteDescription());
  Serial.println("-------------------------");

  c.close();
  Serial.println("Setup complete. Ready to send/receive.");
}

void loop() {
  Lora_Rx();  // Check for incoming message

  int value = digitalRead(7);
  delay(10);

  if (Serial.available()) {
    String Data = Serial.readStringUntil('\n');
    Lora_Tx(Data);
    delay(100);
  }

  if (value == LOW) {
    Lora_Tx(Const_Data);
    delay(500);
  }

  // Handle message logic
  if (Message.length() > 0) {
    if (Message.startsWith("Ack.")) {
      String receivedData = Message.substring(4);
      int quality = calculateSignalQuality(Const_Data, receivedData);
      Serial.println("Signal Quality : " + String(quality) + "%");
    }
    else if (Message == Const_Data) {
      // Send ACK
      String Resender = "Ack." + Message;
      Lora_Tx(Resender);
      delay(200);
    }
    Message = "";  // Clear after processing
  }

  delay(100);
}

void Lora_Rx() {
  if (e22ttl.available() > 1) {
    ResponseContainer rc = e22ttl.receiveMessageRSSI();

    if (rc.status.code == SUCCESS) {
      Message = rc.data;
      //Serial.print("Message: ");
      //Serial.println(Message);

      Serial.print("RSSI: ");
      Serial.print(rc.rssi);
      Serial.println(" dBm");
    } else {
      Serial.print("Receive Error: ");
      Serial.println(rc.status.getResponseDescription());
    }
  }
}

void Lora_Tx(String input) {
  ResponseStatus rs = e22ttl.sendFixedMessage(0, DESTINATION_ADDL, DESTINATION_CHAN, input);

  if (rs.code == SUCCESS) {
    Serial.println("Send Success");
  } else {
    Serial.print("Send Error: ");
    Serial.println(rs.getResponseDescription());
  }
}

int calculateSignalQuality(String expected, String rec) {
  int totalLength = expected.length();
  int matchCount = 0;

  if (rec.length() != totalLength) return 0;

  for (int i = 0; i < totalLength; i++) {
    if (expected[i] == rec[i]) {
      matchCount++;
    }
  }

  return (matchCount * 100) / totalLength;
}
