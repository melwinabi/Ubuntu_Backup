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
  delay(300);
  ultdist();
  int distance = ultdist();
  if (distance > THRESHOLD){
   forward();
  } 
  while(distance >= THRESHOLD){ 
   distance = ultdist();
   delay(250);
  }
  stopm();                                     
  int turn = check();               
  Serial.print(turn);
  switch (turn) 
  {
    case 0:                                      
      left();
      delay(700);
      s.write(105);
      break;
     case 1:                                    
      right();
      delay(700);
      s.write(105);
      break;
     case 2:                                       
      forward();
      delay(700);
      s.write(105);
      break;
      case 3:                                       
      stopm();
      s.write(105);
      break; 
      }
}

void stopm(){
  digitalWrite(7,LOW);
  digitalWrite(8,LOW);
  digitalWrite(9,LOW);
  digitalWrite(10,LOW);
}

void forward(){
  digitalWrite(8,LOW);
  digitalWrite(7,HIGH);
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
  Serial.println(distanceu);
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
                     
  if (distancel>=THRESHOLD&& distancer<=THRESHOLD){ 
    turn = 0;
  } else if (distancel<=THRESHOLD&& distancer>=THRESHOLD) {  
    turn = 1;
  } else if (distancel>=distancer)  {                        
    turn = 0;
  } else if (distancel<=distancer) {                          
   turn = 1;
  } else if (distance>THRESHOLD)  {  
   turn = 2;  
  }else if (distancel<=THRESHOLD&& distancer<=THRESHOLD&& distance<=THRESHOLD){    
    turn = 3;  
   return turn; 
  }
}
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
