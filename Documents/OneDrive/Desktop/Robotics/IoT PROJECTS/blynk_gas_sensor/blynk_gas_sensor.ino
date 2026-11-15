#include <ESP8266WiFi.h>
#define BLYNK_PRINT Serial

char auth[] = "YourAuthToken";  // Replace with your Blynk authentication token
char ssid[] = "MelwinAbi";
char pass[] = "Melwin@321";
WidgetLED gas(V0); //name of widget


int gasSensorPin = A0;  // Analog pin connected to the gas sensor

void setup() {
  Serial.begin(9600);
  Blynk.begin(auth, ssid, pass);
}

void loop() {
//  Blynk.run();
  //timer.run();

  int gasValue = analogRead(gasSensorPin);
  float voltage = gasValue * (5.0 / 1023.0);  // Convert analog reading to voltage

  Serial.print("Gas Value: ");
  Serial.print(gasValue);
  Serial.print(", Voltage: ");
  Serial.println(voltage);

  Blynk.virtualWrite(V0, gasValue);  // Send gas value to Blynk app
  delay(1000);  // Add a delay to prevent flooding Blynk with data
}
