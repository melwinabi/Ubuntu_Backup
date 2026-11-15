// Include the necessary libraries
#include <Wire.h>
#include <Adafruit_TCS3200.h>

// Define S0, S1, S2, S3, and OUT pins
#define S0 6
#define S1 5
#define S2 4
#define S3 3
#define OUT 2

// Initialize a TCS3200 object
Adafruit_TCS3200 tcs = Adafruit_TCS3200(TCS3200_LED_ONBOARD);

void setup() {
  // Initialize serial communication
  Serial.begin(9600);

  // Set the sensor control pins as OUTPUT
  pinMode(S0, OUTPUT);
  pinMode(S1, OUTPUT);
  pinMode(S2, OUTPUT);
  pinMode(S3, OUTPUT);
  pinMode(OUT, INPUT);

  // Set the frequency scaling to 20%
  digitalWrite(S0, HIGH);
  digitalWrite(S1, LOW);

  // Enable the color sensor
  tcs.begin();
}

void loop() {
  // Read the RGB values from the sensor
  uint16_t red, green, blue;
  tcs.getRawData(&red, &green, &blue);

  // Print the RGB values to the Serial Monitor
  Serial.print("Red: ");
  Serial.print(red);
  Serial.print(" Green: ");
  Serial.print(green);
  Serial.print(" Blue: ");
  Serial.println(blue);

  // Delay for a short time before reading again
  delay(500);
}
