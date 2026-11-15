#include <DHT.h>


// Define the pin where the DHT11 data pin is connected
#define DHTPIN 2

// Define the type of sensor
#define DHTTYPE DHT11

// Create an instance of the DHT sensor
DHT dht(DHTPIN, DHTTYPE);

void setup() {
  // Start serial communication
  Serial.begin(9600);

  // Initialize the DHT sensor
  dht.begin();
}

void loop() {
  // Wait a few seconds
