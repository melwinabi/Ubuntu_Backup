// Analog pin for pH sensor
const int PH_SENSOR_PIN = A0;

void setup() {
  Serial.begin(9600);
}

void loop() {
  // Read analog value from pH sensor
  int sensorValue = analogRead(PH_SENSOR_PIN);

  // Print raw sensor value to serial monitor
  Serial.print("Analog Value: ");
  Serial.println(sensorValue);

  // You can also map the analog value to pH value if your sensor provides a linear relationship
  // float pHValue = map(sensorValue, 0, 1023, 0, 14);
  // Serial.print("pH Value: ");
  // Serial.println(pHValue);

  delay(1000); // Delay for 1 second before reading again
}
