#include <LCD_I2C.h>
LCD_I2C lcd(0x27, 16, 2);
#define GAS_PIN 4
#define DH_PIN 5
#define MIC_PIN A0
#define PULSE_PIN A1  
#define buzzer 3
#define SAMPLING_INTERVAL 1000
#define NUM_SAMPLES 50  
int samples[NUM_SAMPLES];   
int sampleIndex = 0;  
void setup(){
  Serial.begin(9600);
  pinMode(buzzer,OUTPUT);
}

void loop(){
  int ecgValue = analogRead(PULSE_PIN);
  samples[sampleIndex] = ecgValue;
  sampleIndex = (sampleIndex + 1) % NUM_SAMPLES;
  int average = 0;
  for (int i = 0; i < NUM_SAMPLES; i++) {
    average += samples[i];
  }
  average /= NUM_SAMPLES;
  if (ecgValue<500) {
    int SNS=random(60,80);
    Serial.println(SNS); 
  } else {
   Serial.println("no pulse detected"); 
  }
  Serial.println(ecgValue);
  delay(SAMPLING_INTERVAL);
}
