
#define BLYNK_TEMPLATE_ID "MPL3qbGbKHJQ"
#define BLYNK_TEMPLATE_NAME "test"
#define BLYNK_PRINT Serial
#include <ESP8266WiFi.h>
#include <BlynkSimpleEsp8266.h>
#define LED 5


#define BLYNK_AUTH_TOKEN "-rHbuTjtPwzLXQAVrksr1Dbeux0vgA6M"

char auth[] = BLYNK_AUTH_TOKEN;
char ssid[] = "MelwinAbi";//Enter your WIFI name
char pass[] = "Melwin@321";//Enter your WIFI password


BLYNK_WRITE(V0) {
  bool value1 = param.asInt();
  
  if (value1 == 1) {
    digitalWrite(LED, HIGH);
  } else {
    digitalWrite(LED, LOW);
  }
}

void setup() {
  //Set the relay pins as output pins
  pinMode(LED, OUTPUT);
  

  
  digitalWrite(LED, HIGH);
 

  
  Blynk.begin(auth, ssid, pass, "blynk.cloud", 80);
}

void loop() {
  //Run the Blynk library
  Blynk.run();
}
