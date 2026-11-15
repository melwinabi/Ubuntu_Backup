#include <Wire.h>
#define PUMB 6
#define Signal 7
const int trigPin = 9;
const int echoPin = 10;
int STATE;

const int motor1Pin1 = 2;
const int motor1Pin2 = 3;
const int motor2pumb1 =4;
const int motor2pumb2 =5;

const int obstacleDistanceThreshold = 8; 

//void START_PUMB();
//void STOP_PUMB();

void setup() {

  Serial.begin(9600);

  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);
  pinMode(motor2pumb1,OUTPUT);
  pinMode(motor2pumb2,OUTPUT);
  pinMode(Signal,INPUT);
  pinMode(motor1Pin1, OUTPUT);
  pinMode(motor1Pin2, OUTPUT);
  
  Serial.print("HAPPY FARMING"); 

}

void loop() {
  
  long duration, distance;
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);
  duration = pulseIn(echoPin, HIGH);
  distance = duration * 0.034 / 2;


  Serial.print("Distance: ");
  Serial.print(distance);
  Serial.println(" cm");
  int STATE = digitalRead(Signal);
  if(STATE==HIGH){
  if (distance <= obstacleDistanceThreshold) {

       stopVehicle();
  } else {
   
    moveForward();
  }
}else if(STATE==LOW){
  stopVehicle();
}else
 stopVehicle();
 
}

void moveForward() {
  digitalWrite(motor1Pin1, LOW);
  digitalWrite(motor1Pin2, HIGH);

  START_PUMB();
  Serial.print("MOVING");
}


void stopVehicle() {
  digitalWrite(motor1Pin1, LOW);
  digitalWrite(motor1Pin2, LOW);
 
  STOP_PUMB();
  Serial.print("STOP");
}
void START_PUMB(){
  digitalWrite(motor2pumb1,HIGH);
  digitalWrite(motor2pumb2,LOW);
}
void STOP_PUMB(){
   digitalWrite(motor2pumb1,LOW);
   digitalWrite(motor2pumb2,LOW); 
}
