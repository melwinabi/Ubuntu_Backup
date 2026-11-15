#define LEFT_MOTOR_PIN1 7
#define LEFT_MOTOR_PIN2 8
#define RIGHT_MOTOR_PIN1 5
#define RIGHT_MOTOR_PIN2 6 

void setup(){
  pinMode(LEFT_MOTOR_PIN1, OUTPUT);
  pinMode(LEFT_MOTOR_PIN2, OUTPUT);
  pinMode(RIGHT_MOTOR_PIN1, OUTPUT);
  pinMode(RIGHT_MOTOR_PIN2, OUTPUT);
}
void loop(){
  digitalWrite(LEFT_MOTOR_PIN1, LOW); // FORWARD
  digitalWrite(LEFT_MOTOR_PIN2, HIGH);
  digitalWrite(RIGHT_MOTOR_PIN1, HIGH);
  digitalWrite(RIGHT_MOTOR_PIN2, LOW);
Serial.print("kdfsg");
}
