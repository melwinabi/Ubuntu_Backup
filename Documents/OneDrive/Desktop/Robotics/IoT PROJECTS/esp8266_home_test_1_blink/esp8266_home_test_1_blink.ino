
#define BLYNK_PRINT Serial
#include <ESP8266WiFi.h>


char auth[ ] = "-rHbuTjtPwzLXQAVrksr1Dbeux0vgA6M";
char ssid[ ] = "MelwinAbi";
char pass[ ] = "Melwin@321";

WidgetLED test(V0);
BlynkTimer timer;

void setup()
{
  Serial.begin(9600);
  Blynk.begin(auth, ssid, pass);
  timer.setInterval(1000L, blinkLedWidget);
  pinMode(D0,OUTPUT);
}


void blinkLedWidget()  // function for switching off and on LED
{
  if (test==1) {
    led1.off();
    Serial.println("LED on V1: off");
  } else {
    led1.on();
    Serial.println("LED on V1: on");
  }
}



//In the loop function include Blynk.run() command.
void loop()
{
  Blynk.run();
  timer.run();
}
