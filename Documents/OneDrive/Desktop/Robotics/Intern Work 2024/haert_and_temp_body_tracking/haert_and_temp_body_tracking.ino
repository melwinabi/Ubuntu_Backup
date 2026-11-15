
#include <LiquidCrystal.h>
#define USE_ARDUINO_INTERRUPTS true    
#include <PulseSensorPlayground.h> 
#include <OneWire.h>
#include <DallasTemperature.h> //DS18B20
#define ONE_WIRE_BUS 2 // digital pin 2

int timer2_counter,Timer_Second;
String url="",field1data,field2data;
int i=0,Count;
float temp;
float pulse,myBPM;
 
OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature sensors(&oneWire);
LiquidCrystal lcd(12,11,5,6,7,8);

PulseSensorPlayground pulseSensor;

const int PulseWire = A1;    
const int buzz =13;       
int Threshold = 550;           

void setup(){
    lcd.begin(16, 2);
    Serial.begin(9600);
    pulseSensor.analogInput(PulseWire);   
    pulseSensor.blinkOnPulse(buzz);     
    pulseSensor.setThreshold(Threshold);   
    if (pulseSensor.begin()) {
    //Serial.println("PulseSensor object created!");
    lcd.setCursor(0,0);
    lcd.print("Pulse Reading!");
    sensors.begin();
   
    }  
      noInterrupts(); 
  TCCR2A = 0;
  TCCR2B |= (1 << CS22)|(1 << CS21)|(1 << CS20);
  TCNT2 = timer2_counter;
  TIMSK2 |=(1<<TOIE2);
  timer2_counter = 240;
  interrupts(); 
}

void loop() {
    myBPM = pulseSensor.getBeatsPerMinute();  
    if (pulseSensor.sawStartOfBeat()){
    lcd.setCursor(0,0);
    lcd.print("                                      ");
    //Serial.println("♥  A HeartBeat Happened ! "); 
    //Serial.print("BPM: ");
    //Serial.println(myBPM);
    lcd.setCursor(0,0);
    lcd.print("BPM:");
    lcd.setCursor(4,0);
    lcd.print(myBPM);
    
   
   }
    
    sensors.requestTemperatures();
    lcd.setCursor(0,1);
    lcd.print("Tp:");
    lcd.setCursor(3,1);
    lcd.print(sensors.getTempCByIndex(0)); 
    temp = sensors.getTempCByIndex(0);
    pulse = myBPM;
    Serial.print(pulse);  
    //Serial.print("Celsius temperature: ");
    //Serial.print(sensors.getTempCByIndex(0)); 

    
    lcd.setCursor(9,1);
    lcd.print("Fh:");
    lcd.setCursor(12,1);
    lcd.print(sensors.getTempFByIndex(0));
    //Serial.print(" - Fahrenheit temperature: ");
    //Serial.println(sensors.getTempFByIndex(0));
   Serial.println("......................................"); 
    //delay(20);
    
   
}
ISR(TIMER2_OVF_vect)         
{
    TCNT2 = timer2_counter;  
    Timer_Second++;
    if(Timer_Second==4000)
    {       lcd.setCursor(5,1);
        lcd.print("           ");    
        Timer_Second = 0;
        url ="/update?api_key=SGH7IY82VOT2YLFD&";
        url += "field1=";
        url +=  pulse ;
        url += "&field2=";
        url += temp;
        Serial.println(url);
    }
}
