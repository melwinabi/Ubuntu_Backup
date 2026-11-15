#define SfTrig 5
#define SrTrig 6
#define SlTrig 3
#define SfEcho 4
#define SrEcho 7
#define SlEcho 2
#define ml1 A0 // motor1
#define ml2 A1 //''''''
#define mr1 A2 //motor 2
#define mr2 A3 //'''''''

void setup() {
  Serial.begin(9600);
  //Set the Trig pins as output pins
  pinMode(SfTrig, OUTPUT);
  pinMode(SrTrig, OUTPUT);
  pinMode(SlTrig, OUTPUT);
  //Set the Echo pins as input pins
  pinMode(SfEcho, INPUT);
  pinMode(SrEcho, INPUT);
  pinMode(SlEcho, INPUT);

  pinMode(mr1,OUTPUT);
  pinMode(mr2,OUTPUT);
  pinMode(ml1,OUTPUT);
  pinMode(ml2,OUTPUT);
}

void loop() {
  Serial.print("sensorThree: ");
  Serial.print(sensorThree());
  Serial.print("    sensorOne: ");
  Serial.print(sensorOne());
  Serial.print("    sensorTwo: ");
  Serial.println(sensorTwo()); 
  int rightSensor  = sensorTwo();
  int centerSensor = sensorOne();
  int leftSensor = sensorThree();
  if (8 >= centerSensor) {
  Stop();
  Serial.println("Stop");
  delay(1000);
  if (leftSensor > rightSensor) {
  left();
  Serial.println("Left");
  delay(500);
  } else {
  right();
  Serial.println("Right");
  delay(500);
  }
  }
  Serial.println("Forward");
  forward();
}
int sensorOne() {
  //pulse output
  digitalWrite(SfTrig, LOW);
  delayMicroseconds(4);
  digitalWrite(SfTrig, HIGH);
  delayMicroseconds(10);
  digitalWrite(SfTrig, LOW);
  long t = pulseIn(SfEcho, HIGH);
  int cm = t / 29 / 2; 
  return cm;
}
int sensorTwo() {
  //pulse output
  digitalWrite(SrTrig, LOW);
  delayMicroseconds(4);
  digitalWrite(SrTrig, HIGH);
  delayMicroseconds(10);
  digitalWrite(SrTrig, LOW);
  long t = pulseIn(SrEcho, HIGH);
  int cm = t / 29 / 2; 
  return cm;
}
int sensorThree() {
  //pulse output
  digitalWrite(SlTrig, LOW);
  delayMicroseconds(4);
  digitalWrite(SlTrig, HIGH);
  delayMicroseconds(10);
  digitalWrite(SlTrig, LOW);
  long t = pulseIn(SlEcho, HIGH);//Get the pulse
  int cm = t / 29 / 2; //Convert time to the distance
  return cm; // Return the values from the sensor
}

//..........................................................//.................................//.............................//............................//..............................
void forward() {
  digitalWrite(mr1,LOW);//FR
  digitalWrite(mr2,HIGH);
  digitalWrite(ml1,LOW);//FR
  digitalWrite(ml2,HIGH);
}
void left() {
  digitalWrite(mr1,HIGH);//FR
  digitalWrite(mr2,LOW);
  digitalWrite(ml1,HIGH);//BC
  digitalWrite(ml2,LOW);
}
void right() {
  digitalWrite(mr1,HIGH);//BC
  digitalWrite(mr2,LOW);
  digitalWrite(ml1,LOW);//FR
  digitalWrite(ml2,HIGH);
 
}
void Stop() {
  digitalWrite(mr1,LOW);
  digitalWrite(mr2,LOW);
  digitalWrite(ml1,LOW);
  digitalWrite(ml2,LOW); 
}
