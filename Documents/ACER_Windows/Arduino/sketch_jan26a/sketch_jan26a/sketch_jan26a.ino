#include <EBYTE.h>

// Define pins for the module
#define M0 4
#define M1 5
#define AUX 6
#define TX 10
#define RX 9

// Create an instance of the EByteE class
EBYTE::EBYTE(M0, M1, AUX, TX, RX);

void setup() {
  Serial.begin(9600); // Start Serial Monitor
  EBYTE.begin(9600);    // Start communication with the module at 9600 baud rate
  
  if (EBYTE.isReady()) {
    Serial.println("E22 Module Ready for Reception");
  } else {
    Serial.println("E22 Module Initialization Failed!");
    while (1); // Stop execution if the module is not ready
  }
}

void loop() {
  String receivedMessage;
  
  if (EBYTE.receiveMessage(receivedMessage)) {
    Serial.println("Received Message: " + receivedMessage);
  } else {
    Serial.println("No message received");
  }

  delay(500); // Check for messages every 500ms
}
