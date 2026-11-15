#include <TinyGPSPlus.h>
TinyGPSPlus gps;
HardwareSerial GPSSerial(1);

void setup() {
  Serial.begin(115200);
 
}

void loop() {
  while (GPSSerial.available() > 0) {
    if (gps.encode(GPSSerial.read())) {
      if (gps.location.isValid()) {
        Serial.print("Latitude: ");
        Serial.print(gps.location.lat(), 6);
        Serial.print(", Longitude: ");
        Serial.println(gps.location.lng(), 6);
      } else {
        Serial.println("Waiting for valid GPS data...");
      }
    }
  }
}
