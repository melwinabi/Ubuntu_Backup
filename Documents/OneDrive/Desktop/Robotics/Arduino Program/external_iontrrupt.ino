void setup()
{

pinMode(12,OUTPUT);
pinMode(0,INPUT);
attachInterrupt(0, blink, FALLING);

}

void loop()
{

digitalWrite(3,HIGH);
delay(2000);

}

void blink()

{

digitalWrite(13, !digitalRead(13)); // Toggle LED on pin 13

}
