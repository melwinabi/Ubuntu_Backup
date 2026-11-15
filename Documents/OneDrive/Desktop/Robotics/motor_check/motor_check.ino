#include <Servo.h>
#define ECHO_PIN 4
#define TRIG_PIN 11
#define OBSTACLE_THRESHOLD 15
Servo s;

//...........................................................setup.........................


void setup() {
  Serial.begin(9600);
  pinMode(TRIG_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);
  pinMode(7,OUTPUT);//R
  pinMode(8,OUTPUT);
  pinMode(9,OUTPUT);//L
  pinMode(10,OUTPUT);
  s.attach(5);
  s.write(105);//0 105 200
}


//...........................................................loop..........................

void loop(){
 long duration, distance;
  digitalWrite(TRIG_PIN, LOW);
  delayMicroseconds(2);
  digitalWrite(TRIG_PIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG_PIN, LOW);
  duration = pulseIn(ECHO_PIN, HIGH);
  distance = (duration / 2) / 29.1; // Convert the distance to centimeters
  Serial.println(distance);

   if (distance < OBSTACLE_THRESHOLD) {
    stopm();
     
    delay(500); 
  } else {
    forward();
    s.write(105); 
  }
}

void stopm(){
  digitalWrite(7,LOW);
  digitalWrite(8,LOW);
  digitalWrite(9,LOW);
  digitalWrite(10,LOW);
}

void forward(){
  digitalWrite(7,LOW);
  digitalWrite(8,HIGH);
  digitalWrite(9,LOW);
  digitalWrite(10,HIGH);
}
