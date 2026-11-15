#include<EEPROM.h>
#define led 2
#define Switch 4
#define EEPROM_SIZE 64
#define EEPROM_add 0 
int ledupd;
void setup() {
  Serial.begin(9600);
  EEPROM.begin(EEPROM_SIZE);
  pinMode(led,OUTPUT);
  pinMode(Switch,INPUT_PULLUP);
  ledupd=EEPROM.read(0);
  digitalWrite(led,ledupd);
  //delay(8000);check ok
}

void loop() {
  int switch_state = digitalRead(Switch);
  // Serial.println(switch_state); //0 low, 1 high
  if (switch_state==1){
      ledupd=1;
      EEPROM.write(EEPROM_add,ledupd);
      digitalWrite(led,ledupd);
      EEPROM.commit();
  }else if(switch_state==0){
      ledupd=0;
      digitalWrite(led,ledupd);
      EEPROM.write(EEPROM_add,ledupd);
      EEPROM.commit();
  }
 int eep = EEPROM.read(0);
 Serial.println(eep);
}
