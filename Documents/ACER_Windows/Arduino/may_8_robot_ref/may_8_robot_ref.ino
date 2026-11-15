#include <Servo.h>
Servo s;
#define LEFT_SENSOR A2 //2
#define RIGHT_SENSOR A1 //1
#define LEFT_MOTOR_PIN1 7
#define LEFT_MOTOR_PIN2 8
#define RIGHT_MOTOR_PIN1 5
#define RIGHT_MOTOR_PIN2 6  
#define THRESHOLD 100  
#define enA 2
#define enB 9
#define cd1 3
#define cd2 4 
#define sts 13 //status from esp
int trigerpin = 10;
int echo = 12;
int duration;
int distance;
void setup() {
  Serial.begin(9600);
  pinMode(LEFT_MOTOR_PIN1, OUTPUT);
  pinMode(LEFT_MOTOR_PIN2, OUTPUT);
  pinMode(RIGHT_MOTOR_PIN1, OUTPUT);
  pinMode(RIGHT_MOTOR_PIN2, OUTPUT);
  pinMode(enA, OUTPUT);
  pinMode(enB, OUTPUT);
  analogWrite(enA, 200); // Write The Duty Cycle 0 to 255 Enable Pin A for Motor1 Speed 
  analogWrite(enB, 200);
  pinMode(cd1,OUTPUT);
  pinMode(cd2,OUTPUT);
  pinMode(trigerpin,OUTPUT);
  pinMode(echo,INPUT);
  pinMode(sts,INPUT);
  s.attach(11);
  s.write(0);
  
}

void loop() {
//  Serial.print("hello");
//  front_ultra();
//  if(distance>20){
//  s.write(0);
//  }
//  else if(distance<20){
//   ultra_stop();
//   s.write(195);
//  }
//  int statuse=digitalRead(sts);
//  //Serial.print(statuse);
//  if (statuse==1){
//    operate();
//   }else if (statuse==0){
//     ultra_stop();
//     s.write(0);
//  }
//}
// void operate(){ 
  int leftSensorValue = analogRead(LEFT_SENSOR);
  int rightSensorValue = analogRead(RIGHT_SENSOR);
  Serial.print(leftSensorValue); //1
  Serial.print("    ");
  Serial.println(rightSensorValue); //2
  if (THRESHOLD > leftSensorValue && THRESHOLD > rightSensorValue ) {
  moveForward();
  }else if (leftSensorValue > THRESHOLD && rightSensorValue > THRESHOLD) {
  stopm();
  }else if (leftSensorValue <= THRESHOLD && rightSensorValue > THRESHOLD) {
  turnLeft();
  }else if (leftSensorValue > THRESHOLD && rightSensorValue <= THRESHOLD) {
  turnRight();
  }
}


void moveForward() {
  digitalWrite(LEFT_MOTOR_PIN1,LOW );//FORWARD
  digitalWrite(LEFT_MOTOR_PIN2,HIGH);
  digitalWrite(RIGHT_MOTOR_PIN1, HIGH);
  digitalWrite(RIGHT_MOTOR_PIN2, LOW);
  
}

void turnLeft() {
 digitalWrite(LEFT_MOTOR_PIN1,HIGH );//LEFT
  digitalWrite(LEFT_MOTOR_PIN2,LOW);
  digitalWrite(RIGHT_MOTOR_PIN1, HIGH);
  digitalWrite(RIGHT_MOTOR_PIN2, LOW);
  
}

void turnRight() {
 digitalWrite(LEFT_MOTOR_PIN1,LOW );//RIGHT
  digitalWrite(LEFT_MOTOR_PIN2,HIGH);
  digitalWrite(RIGHT_MOTOR_PIN1, LOW);
  digitalWrite(RIGHT_MOTOR_PIN2, HIGH);
}  


void stopm(){
  digitalWrite(LEFT_MOTOR_PIN1, LOW);
  digitalWrite(LEFT_MOTOR_PIN2, LOW);
  digitalWrite(RIGHT_MOTOR_PIN1, LOW);
  digitalWrite(RIGHT_MOTOR_PIN2, LOW);
  cd_open();
  delay(500);
  cd_close();
 }

 void cd_open(){
  digitalWrite(cd1,LOW);//OPEN
  digitalWrite(cd2,HIGH);
  delay(5000);
 }
  void cd_close(){
  digitalWrite(cd1,HIGH);//CLOSE
  digitalWrite(cd2,LOW);
  delay(5000);
  }
 void ultra_stop(){
  digitalWrite(LEFT_MOTOR_PIN1, LOW);
  digitalWrite(LEFT_MOTOR_PIN2, LOW);
  digitalWrite(RIGHT_MOTOR_PIN1, LOW);
  digitalWrite(RIGHT_MOTOR_PIN2, LOW);
 }
 void front_ultra(){
  digitalWrite(trigerpin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigerpin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigerpin, LOW);
  duration = pulseIn(echo, HIGH);
  distance = duration * .034 / 2;
  // Serial.print("\t");
   Serial.println(distance);
  //delay(200);
 }
 

 

    
