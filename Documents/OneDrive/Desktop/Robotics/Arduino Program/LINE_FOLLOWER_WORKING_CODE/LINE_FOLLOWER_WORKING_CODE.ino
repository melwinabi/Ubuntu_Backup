#define LEFT_SENSOR A2 //1
#define RIGHT_SENSOR A1 //2
#define LEFT_MOTOR_PIN1 7
#define LEFT_MOTOR_PIN2 8
#define RIGHT_MOTOR_PIN1 5
#define RIGHT_MOTOR_PIN2 6  
#define THRESHOLD 500  
//#define BASE_SPEED 150
//#define TURN_SPEED 100
#define enA 4
#define enB 9


void setup() {
  pinMode(LEFT_MOTOR_PIN1, OUTPUT);
  pinMode(LEFT_MOTOR_PIN2, OUTPUT);
  pinMode(RIGHT_MOTOR_PIN1, OUTPUT);
  pinMode(RIGHT_MOTOR_PIN2, OUTPUT);
  pinMode(enA, OUTPUT);
  pinMode(enB, OUTPUT);
  analogWrite(enA, 200); // Write The Duty Cycle 0 to 255 Enable Pin A for Motor1 Speed 
  analogWrite(enB, 200);
  Serial.begin(9600);
}

void loop() {
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
  digitalWrite(LEFT_MOTOR_PIN1,HIGH );
  digitalWrite(LEFT_MOTOR_PIN2, LOW);
  digitalWrite(RIGHT_MOTOR_PIN1, HIGH);
  digitalWrite(RIGHT_MOTOR_PIN2, LOW);
  //analogWrite(LEFT_MOTOR_PIN1, BASE_SPEED);
  //analogWrite(RIGHT_MOTOR_PIN1, BASE_SPEED);
}

void turnLeft() {
 digitalWrite(LEFT_MOTOR_PIN1,HIGH);
  digitalWrite(LEFT_MOTOR_PIN2, LOW);
  digitalWrite(RIGHT_MOTOR_PIN1,LOW);
  digitalWrite(RIGHT_MOTOR_PIN2, HIGH);
  //Adjust motor speed if needed
  //analogWrite(LEFT_MOTOR_PIN1, TURN_SPEED);
  //analogWrite(RIGHT_MOTOR_PIN2, TURN_SPEED);
}

void turnRight() {
digitalWrite(LEFT_MOTOR_PIN1, LOW);
  digitalWrite(LEFT_MOTOR_PIN2, HIGH);
  digitalWrite(RIGHT_MOTOR_PIN1, HIGH);
  digitalWrite(RIGHT_MOTOR_PIN2, LOW);
  //analogWrite(LEFT_MOTOR_PIN2, TURN_SPEED);
  //analogWrite(RIGHT_MOTOR_PIN1, TURN_SPEED);
}  


void stopm(){
  digitalWrite(LEFT_MOTOR_PIN1, LOW);
  digitalWrite(LEFT_MOTOR_PIN2, LOW);
  digitalWrite(RIGHT_MOTOR_PIN1, LOW);
  digitalWrite(RIGHT_MOTOR_PIN2, LOW);
  //analogWrite(LEFT_MOTOR_PIN2, TURN_SPEED);
  //analogWrite(RIGHT_MOTOR_PIN1, TURN_SPEED);
}

   
