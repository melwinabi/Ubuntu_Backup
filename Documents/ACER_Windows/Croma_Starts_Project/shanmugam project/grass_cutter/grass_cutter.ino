#include <Servo.h>
#define ECHO_PIN 4
#define TRIG_PIN 11
#define THRESHOLD 15
Servo s;
long distance,duration,distanceu,distancer,distancel;
void left();
void right();
void head();
void stopm();
int turn;
String received;
String readString;
bool moving = false;
//...........................................................setup.........................
void setup() {
  Serial.begin(9600);
  Serial.println("serial-delimit-21");
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

  if (Serial.available() > 0) {
    String received = Serial.readStringUntil('\n');
    if (received == "START" && !moving) {
      moving = true;
      Serial.println("MOVING");
      head();
    }
    if (received == "STOP" && moving) {
      moving = false;
      Serial.println("STOP");
      stopm();
    }
  }
  while (moving) {
    distance = ultdist();
    Serial.println(distance);

    if (distance > THRESHOLD) {
      forward();
    } else {
      stopm();
      int turn = check();

      switch (turn) {
        case 0:
          left();
          delay(700);
          s.write(105);
          break;
        case 1:
          right();
          delay(700);
          s.write(75);
          break;
        case 2:
          forward();
          delay(700);
          s.write(90);
          break;
        case 3:
          stopm();
          s.write(90);
          break;
      }
    }

    delay(250);
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
int ultdist(){
 
  digitalWrite(TRIG_PIN, LOW);
  delayMicroseconds(2);
  digitalWrite(TRIG_PIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG_PIN, LOW);
  duration = pulseIn(ECHO_PIN, HIGH);
  distanceu = (duration / 2) / 29.1; 
  return distanceu;
  
}

int check()                                            
{
 int distance = ultdist();  
 if(distance<=THRESHOLD){                                             
  s.write(0);                                        
  distancel = ultdist();//l
  delay(1000);
  s.write(200);                                        
  distancer = ultdist();    //R       
  delay(1000);
                     
  if (distancel >= THRESHOLD && distancer <= THRESHOLD) { 
      return 0; // Turn left
    } else if (distancel <= THRESHOLD && distancer >= THRESHOLD) {  
      return 1; // Turn right
    } else if (distancel >= distancer) {                        
      return 0; // Turn left
    } else if (distancel <= distancer) {                          
      return 1; // Turn right
    } else if (distance > THRESHOLD) {  
      return 2; // Move forward
    } else if (distancel <= THRESHOLD && distancer <= THRESHOLD && distance <= THRESHOLD) {    
      return 3; // Stop
    }
  }

  return -1; // No action
}

void  left(){
  digitalWrite(9,HIGH);//LT
  digitalWrite(10,LOW);
  digitalWrite(7,LOW);
  digitalWrite(8,HIGH);
}
void right(){
  digitalWrite(9,LOW);//RT
  digitalWrite(10,HIGH);
  digitalWrite(7,HIGH);
  digitalWrite(8,LOW);
}

//........................................<<<*END*>>>...............................................
//pin connection
//
//rx 1
//tx 0
//
//in 1 7 ard
//   2 8
//   3 9
//   4 10
//
//servo 5
//
//trig 11
//echo 4
//
//
//
