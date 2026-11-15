#define DERE_PIN 15          // DE & RE shorted
#define RXD2     16          // Serial2 RX
#define TXD2     17          // Serial2 TX

String rxData = "";

void setup() {
  Serial.begin(115200);

  pinMode(DERE_PIN, OUTPUT);

  // Serial2 init
  Serial2.begin(9600, SERIAL_8N1, RXD2, TXD2);

  delay(1000);

  // ---------- TRANSMIT ONCE ----------
  digitalWrite(DERE_PIN, HIGH);   // TX mode
  delay(5);

  Serial2.println("HELLO FROM ESP32 RS485");

  Serial2.flush();                // wait till data sent
  delay(5);

  digitalWrite(DERE_PIN, LOW);    // BACK TO RX MODE
  Serial.println("TX done → RX mode");
}

void loop() {
  // ---------- ALWAYS RECEIVE ----------
  while (Serial2.available()) {
    char c = Serial2.read();

    if (c == '\n') {              // end of line
      rxData.trim();
      Serial.println("Received: " + rxData);
      rxData = "";                // clear buffer
    } else {
      rxData += c;
    }
  }
}
