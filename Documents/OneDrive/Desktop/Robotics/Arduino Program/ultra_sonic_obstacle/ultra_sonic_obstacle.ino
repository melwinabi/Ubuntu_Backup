#define motor_r1 2 // motor1
#define motor_r2 4 //''''''
#define motor_l1 5 //motor 2
#define motor_l2 6 //'''''''
#define trigf 3
#define trigr 10
#define trigl 11
#define echof 7 //front
#define echor 8 //right
#define echol 9 //left
int durationf,durationr,durationl;
int distancef,distancer,distancel;

void setup() {
  Serial.begin(9600);
  pinMode(trigf, OUTPUT);
  pinMode(echof, INPUT);
  pinMode(trigr, OUTPUT);
  pinMode(echor, INPUT);
  pinMode(trigl, OUTPUT);
  pinMode(echol, INPUT);
  pinMode(motor_r1,OUTPUT);
  pinMode(motor_r2,OUTPUT);
  pinMode(motor_l1,OUTPUT);
  pinMode(motor_l2,OUTPUT);
}

void loop() {
  long durationf, distancef;
  digitalWrite(trigf, LOW);
  delayMicroseconds(2);
  digitalWrite(trigf, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigf, LOW);
  durationf = pulseIn(echof, HIGH);
  distancef = durationf * 0.034 / 2;
  Serial.print("Sensor f: ");
  Serial.print(distancef);
  Serial.print(" cm   ");


// Sensor 2
  long durationr, distancer;
  digitalWrite(trigr, LOW);
  delayMicroseconds(2);
  digitalWrite(trigr, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigr, LOW);
  durationr = pulseIn(echor, HIGH);
  distancer = durationr * 0.034 / 2;
  Serial.print("Sensor r: ");
  Serial.print(distancer);
  Serial.print(" cm   ");

  // Sensor 3
  long durationl, distancel;
  digitalWrite(trigl, LOW);
  delayMicroseconds(2);
  digitalWrite(trigl, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigl, LOW);
  durationl = pulseIn(echol, HIGH);
  distancel = durationl * 0.034 / 2;
  Serial.print("Sensor l: ");
  Serial.print(distancel);
  Serial.println(" cm");

 if ((distancef >8 || distancer <8 || distancel <8)){
 forward();
 delay(50);
 }else if((distancef >8 || distancer <8 || distancel <8)){
 forward();
 delay(50);
 }else if((distancef <8 || distancer >8 || distancer <8)){
 right(); 
 delay(50);
 }else if((distancef <8 || distancer <8 || distancer >8)){
 left();
 delay(50);
 }else if (distancef <8 || distancer <8 || distancel <8) {
 stopm();
 delay(50);
 }
} 
  
void stopm(){
  digitalWrite(motor_r1,LOW);
  digitalWrite(motor_r2,LOW);
  digitalWrite(motor_l1,LOW);
  digitalWrite(motor_l2,LOW); 
  
}
void forward(){
  digitalWrite(motor_r1,LOW);//FR
  digitalWrite(motor_r2,HIGH);
  digitalWrite(motor_l1,LOW);//FR
  digitalWrite(motor_l2,HIGH);
}
void right(){
  digitalWrite(motor_r1,HIGH);//BC
  digitalWrite(motor_r2,LOW);
  digitalWrite(motor_l1,LOW);//FR
  digitalWrite(motor_l2,HIGH);
}
void left(){
  digitalWrite(motor_r1,HIGH);//FR
  digitalWrite(motor_r2,LOW);
  digitalWrite(motor_l1,HIGH);//BC
  digitalWrite(motor_l2,LOW);
}
