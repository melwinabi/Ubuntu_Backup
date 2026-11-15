#include <Servo.h>

Servo servo;

#define LEFT_SENSOR A5 //2
#define RIGHT_SENSOR A4 //1
#define LEFT_MOTOR_PIN1 7
#define LEFT_MOTOR_PIN2 8
#define RIGHT_MOTOR_PIN1 5
#define RIGHT_MOTOR_PIN2 6  
#define THRESHOLD 100 
#define enA 2
#define enB 9
#define cd1 3
#define cd2 4 
#define trigerpin 10
#define echo 12
#define servoPin 11
#define thresholdDistance 45
#define lidOpenDelay 350    // Adjust as needed
#define lidCloseDelay 350   // Adjust as needed

unsigned long lidTimer = 0; // Timer for lid opening/closing
void front_ultra();
int duration;
int distance;

void setup() {
  Serial.begin(9600);
  pinMode(LEFT_MOTOR_PIN1, OUTPUT);
  pinMode(LEFT_MOTOR_PIN2, OUTPUT);
  pinMode(RIGHT_MOTOR_PIN1, OUTPUT);
  pinMode(RIGHT_MOTOR_PIN2, OUTPUT);
  analogWrite(enA, 200); // Write The Duty Cycle 0 to 255 Enable Pin A for Motor1 Speed 
  analogWrite(enB, 200);
  pinMode(cd1, OUTPUT);
  pinMode(cd2, OUTPUT);
  pinMode(trigerpin, OUTPUT);
  pinMode(echo, INPUT);
  pinMode(LEFT_SENSOR , INPUT);
  pinMode(RIGHT_SENSOR , INPUT);
  servo.attach(servoPin);
  servo.write(0); 
  digitalWrite(cd1, HIGH); // close
  digitalWrite(cd2, LOW);

}

void loop() {
  front_ultra();
  if (distance <= thresholdDistance) {
    ultra_stop();
    servo.write(180); // Open the lid
    lidTimer = millis(); // Start the timer for lid opening
  } else if (millis() - lidTimer >= lidOpenDelay) {
    servo.write(0); // Close the lid after lidOpenDelay milliseconds
    lidTimer = millis(); // Start the timer for lid closing
  }
  int leftSensorValue = digitalRead(LEFT_SENSOR);
  int rightSensorValue = digitalRead(RIGHT_SENSOR);
   //Serial.print(LEFT_SENSOR);
//   Serial.print("\t");
//   Serial.print(RIGHT_SENSOR);
  if (THRESHOLD > leftSensorValue && THRESHOLD > rightSensorValue) {
    moveForward();
  } else if (leftSensorValue > THRESHOLD && rightSensorValue > THRESHOLD) {
    stopm();
  } else if (leftSensorValue <= THRESHOLD && rightSensorValue > THRESHOLD) {
    turnLeft();
  } else if (leftSensorValue > THRESHOLD && rightSensorValue <= THRESHOLD) {
    turnRight();
  }
}

void moveForward() {
  digitalWrite(LEFT_MOTOR_PIN1, LOW); // FORWARD
  digitalWrite(LEFT_MOTOR_PIN2, HIGH);
  digitalWrite(RIGHT_MOTOR_PIN1, HIGH);
  digitalWrite(RIGHT_MOTOR_PIN2, LOW);
  cd_close() ;
}

void turnLeft() {
 digitalWrite(LEFT_MOTOR_PIN1, HIGH); // LEFT
  digitalWrite(LEFT_MOTOR_PIN2, LOW);
  digitalWrite(RIGHT_MOTOR_PIN1, HIGH);
  digitalWrite(RIGHT_MOTOR_PIN2, LOW);
}

void turnRight() {
 digitalWrite(LEFT_MOTOR_PIN1, LOW); // RIGHT
  digitalWrite(LEFT_MOTOR_PIN2, HIGH);
  digitalWrite(RIGHT_MOTOR_PIN1, LOW);
  digitalWrite(RIGHT_MOTOR_PIN2, HIGH);
}  

void stopm() {
  digitalWrite(LEFT_MOTOR_PIN1, LOW);
  digitalWrite(LEFT_MOTOR_PIN2, LOW);
  digitalWrite(RIGHT_MOTOR_PIN1, LOW);
  digitalWrite(RIGHT_MOTOR_PIN2, LOW);
  cd_open();
  
}

void cd_open() {
  digitalWrite(cd1, LOW); // OPEN
  digitalWrite(cd2, HIGH);
  
}
void cd_close() {
  digitalWrite(cd1,HIGH); // CLOSE
  digitalWrite(cd2,LOW);
}
void front_ultra() {
  digitalWrite(trigerpin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigerpin, HIGH);
  delayMicroseconds(2);
  digitalWrite(trigerpin, LOW);
  duration = pulseIn(echo, HIGH);
  distance = duration * 0.034 / 2;
  delay(1000);
   Serial.println("distance");
   Serial.print("\t");
  Serial.println(distance);
}
void ultra_stop(){
  digitalWrite(LEFT_MOTOR_PIN1, LOW);
  digitalWrite(LEFT_MOTOR_PIN2, LOW);
  digitalWrite(RIGHT_MOTOR_PIN1, LOW);
  digitalWrite(RIGHT_MOTOR_PIN2, LOW);
}
