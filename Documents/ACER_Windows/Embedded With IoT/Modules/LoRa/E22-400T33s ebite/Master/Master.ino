#include <SoftwareSerial.h>
SoftwareSerial LoRaSerial(2, 3); // RX (to TXD of E22), TX (to RXD of E22)

#define M0 4
#define M1 5
#define AUX 6

int message;

void setup() {
    pinMode(M0, OUTPUT);
    pinMode(M1, OUTPUT);
    pinMode(AUX, INPUT);

    digitalWrite(M0, LOW); // Set to normal mode (transmission mode)
    digitalWrite(M1, LOW);
    delay(200); // Stabilize module

    LoRaSerial.begin(9600);
    Serial.begin(9600);
    Serial.println("Transmitter/Receiver Ready...");
}

void loop() {
    // Transmit a message
    message++;
    if (message > 100) {
        message = 0;
    }
    while (digitalRead(AUX) == LOW); // Wait for module to be ready
    LoRaSerial.println(message); // Send message
//    Serial.print("Sent: ");
//    Serial.println(message);
    
    delay(500); // Short delay before switching to receive mode

    // Receive messages
    if (LoRaSerial.available()) {
        String receivedMessage = "";

        // Read the incoming data
        while (LoRaSerial.available()) {
            char c = LoRaSerial.read();
            receivedMessage += c;
        }

        // Print received message to the Serial Monitor
        Serial.print("Received: ");
        Serial.println(receivedMessage);
    }

    delay(500); // Delay before next transmission
}
