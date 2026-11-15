#include <Wire.h> // Include the Wire library for I2C communication

const int I2C_ADDRESS = 0x27; // Replace with your I2C device address
const int NUM_BYTES = 10;      // Number of bytes to read

void setup() {
  Serial.begin(9600);          // Initialize serial communication at 9600 baud
  Wire.begin();                // Initialize the I2C bus
  Serial.println("I2C Reader Initialized.");
  
  // Attempt to read data from the I2C device
  readI2CData(I2C_ADDRESS, NUM_BYTES);
}

void loop() {
  // You can repeat the read operation periodically if needed
  delay(1000); // Wait for 1 second
  readI2CData(I2C_ADDRESS, NUM_BYTES);
}

void readI2CData(int address, int numBytes) {
  Wire.requestFrom(address, numBytes); // Request 'numBytes' from the I2C device

  // Check if the request was successful
  if (Wire.available() == numBytes) {
    Serial.print("Received ");
    Serial.print(numBytes);
    Serial.println(" bytes:");

    // Read the bytes and print them
    for (int i = 0; i < numBytes; i++) {
      byte data = Wire.read(); // Read each byte
      Serial.print("Byte ");
      Serial.print(i);
      Serial.print(": ");
      Serial.println(data, HEX); // Print the byte in hexadecimal format
    }
  } else {
    Serial.print("Failed to read data: Expected ");
    Serial.print(numBytes);
    Serial.print(" bytes, but got ");
    Serial.println(Wire.available());
  }
}
