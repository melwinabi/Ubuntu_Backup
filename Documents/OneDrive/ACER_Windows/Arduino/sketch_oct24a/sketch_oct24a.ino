#include <EEPROM.h>

void setup() {
  Serial.begin(9600);  // Start serial communication for debugging

  // Get EEPROM size for the current board
  int eepromSize = EEPROM.length();

  Serial.print("EEPROM size for this board: ");
  Serial.println(eepromSize);

  // Clear the EEPROM by writing 0 to all addresses
  for (int i = 0; i < eepromSize; i++) {
    EEPROM.write(i, 0);
  }

  Serial.println("EEPROM has been cleared.");
}

void loop() {
  // Nothing to do here
}
