#include <avr/io.h>
#include <avr/interrupt.h>
const int mot = 13; 
const int LED = 12;
unsigned int reloadA = 99;
unsigned int reloadB = 49910;
volatile int count;
volatile int inc;
void setup()
{
Serial.begin(9600);
pinMode(13,OUTPUT);
cli();
TCNT2 = reloadA;
TCCR2B = (1<<CS20)|(1<<CS22)|(1<<CS21); 
TIMSK2 = (1<<TOIE2); 
Serial.println("TIMER2 Setup Finished.");
TCNT1 = reloadB;
TCCR1B = (1<<CS10)|(1<<CS12); 
TIMSK1 = (1<<TOIE1); 
sei(); 
Serial.println("TIMER1 Setup Finished.");
}
void loop()
{
}
ISR(TIMER2_OVF_vect)
{
reloadA = 99;  
count++; 
if (count==50)
{
flash();
count=0; 
} 
}
ISR(TIMER1_OVF_vect)
{
reloadB = 49910;
inc++; 
if (inc==500)
{
motor();
inc=0; 
}
}
void flash()
{
static boolean output = HIGH;
digitalWrite(12, output);
output = !output;
}
void motor()
{
static boolean output = HIGH;
digitalWrite(13, output);
output = !output;
}
