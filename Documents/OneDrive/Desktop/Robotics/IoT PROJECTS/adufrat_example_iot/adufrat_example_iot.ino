
#include "config.h"

#define LED_PIN 5 //D1

AdafruitIO_Feed *led = io.feed("led");

void setup() {
  
  pinMode(LED_PIN, OUTPUT);
  
  
  Serial.begin(9600);

 
  while(! Serial);

  // connect to io.adafruit.com
  Serial.print("Connecting to Adafruit IO");
  io.connect();

  // set up a message handler for the 'digital' feed.
  // the handleMessage function (defined below)
  // will be called whenever a message is
  // received from adafruit io.
  led->onMessage(handleMessage);

  // wait for a connection
  while(io.status() < AIO_CONNECTED) {
    Serial.print(".");
    delay(500);
  }

  // we are connected
  Serial.println();
  Serial.println(io.statusText());
  led->get();

}

void loop() {

  // io.run(); is required for all sketches.
  // it should always be present at the top of your loop
  // function. it keeps the client connected to
  // io.adafruit.com, and processes any incoming data.
  io.run();

}

// this function is called whenever an 'digital' feed message
// is received from Adafruit IO. it was attached to
// the 'digital' feed in the setup() function above.
void handleMessage(AdafruitIO_Data *data) {

  Serial.print("received <- ");

  if(data->toPinLevel() == 1)
    Serial.println("HIGH");
    digitalWrite(LED_PIN,HIGH);
  if(data->toPinLevel() == 0)
    Serial.println("LOW");
     digitalWrite(LED_PIN,LOW);
     


  digitalWrite(LED_PIN, data->toPinLevel());
}
