#include <Servo.h>
#define ECHO_PIN 4
#define TRIG_PIN 11
#define THRESHOLD 15
Servo servo;
long distance,duration;
void ultra();
void stopm();
void setup() {
  Serial.begin(9600);
  Serial.println("serial-delimit-21");
  pinMode(TRIG_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);
  servo.attach(5);
  servo.write(105); // Set servo to neutral position
  pinMode(7,OUTPUT);
  pinMode(8,OUTPUT); 
  pinMode(9,OUTPUT);
  pinMode(10,OUTPUT);
}

void loop() {
 ultra();
 if(distance<15){
  stopm();
 }
  while (Serial.available())
  {
    delay(1000);
    char inChar = (char)Serial.read();
     if (inChar == 'F'){
      Serial.println(inChar);
      digitalWrite(7, HIGH);//R
      digitalWrite(8, LOW);
      digitalWrite(9, HIGH);//L
      digitalWrite(10,LOW);
     
}
if (inChar == 'B'){
      Serial.println(inChar);
      digitalWrite(7, LOW);//R
      digitalWrite(8, HIGH);
      digitalWrite(9, LOW);//L
      digitalWrite(10,HIGH);
     
}
if (inChar == 'R'){
      Serial.println(inChar);
      digitalWrite(7, LOW);//R
      digitalWrite(8, HIGH);
      digitalWrite(9, HIGH);//L
      digitalWrite(10,LOW);
     
}
if (inChar == 'L'){
      Serial.println(inChar);
      digitalWrite(7, HIGH);//R
      digitalWrite(8, LOW);
      digitalWrite(9, LOW);//L
      digitalWrite(10,HIGH);
     
}
if (inChar == 'S'){
      Serial.println(inChar);
      stopm();
  }
}
}
void ultra(){

  digitalWrite(TRIG_PIN, LOW);
  delayMicroseconds(2);
  digitalWrite(TRIG_PIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG_PIN, LOW);
  duration = pulseIn(ECHO_PIN, HIGH);
  distance = duration * .034 / 2;
//Serial.print("\t");
  Serial.println(distance);
// delay(200);
}
void stopm(){
      digitalWrite(7, LOW);//R
      digitalWrite(8, LOW);
      digitalWrite(9, LOW);//L
      digitalWrite(10,LOW);
}
