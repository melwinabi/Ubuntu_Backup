#include <EEPROM.h> // Include the EEPROM library
 // Define the value to store
  unsigned int valueToStore =21560;

  // Choose an address to store the data; ensure it does not exceed EEPROM size
  int address = 0;
void setup() {
  // Initialize serial communication for debugging
  Serial.begin(9600);

 


}

void loop() {
    // Store the value in EEPROM at the specified address
    valueToStore++;
  EEPROM.put(address, valueToStore);

  // Retrieve and print the stored value to confirm it
  unsigned int retrievedValue;
  EEPROM.get(address, retrievedValue);
  Serial.print("Stored value: ");
  Serial.println(retrievedValue);
}
