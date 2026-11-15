#include <avr/io.h>
#include <avr/interrupt.h>
//int flash;
const int LED_pin = 13; 
unsigned int reload = 99; 
volatile int count;
void setup()
{
Serial.begin(9600);
pinMode(LED_pin, OUTPUT);
digitalWrite(LED_pin, LOW);
pinMode(12,OUTPUT);
cli();
TCNT2 = reload;
TCCR2B = (1<<CS20)|(1<<CS22)|(1<<CS21); 
TIMSK2 = (1<<TOIE2); 
sei(); 
Serial.println("TIMER2 Setup Finished.");
}
void loop()
{
digitalWrite(12,HIGH);
delay(2000);
digitalWrite(12,LOW);
delay(2000);
}
ISR(TIMER2_OVF_vect)
{
count++; 
if (count==10)
{
flash();
count=0; 
}
}
void flash()
{
static boolean output = HIGH;
digitalWrite(LED_pin, output);
output = !output;
}
