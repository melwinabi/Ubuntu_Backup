#include <avr/io.h>
#include <avr/interrupt.h>
//int flash;
const int LED_pin = 13; 
unsigned int reload = 49910; 
volatile int count;
void setup()
{
Serial.begin(9600);
pinMode(LED_pin, OUTPUT);
digitalWrite(LED_pin, LOW);
pinMode(12,OUTPUT);
cli();
TCNT1 = reload;
TCCR1B = (1<<CS10) | (1<<CS12); 
TIMSK1 = (1<<OCIE1A); 
sei(); 
Serial.println("TIMER1 Setup Finished.");
}
void loop()
{
digitalWrite(12,HIGH);
delay(2000);
digitalWrite(12,LOW);
delay(2000);
}
ISR(TIMER1_OVF_vect)
{
count++; 
if (count==1)
flash();
}
void flash()
{
static boolean output = HIGH;
digitalWrite(LED_pin, output);
output = !output;
}
