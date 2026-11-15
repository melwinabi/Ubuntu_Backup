#define LEFT_MOTOR_PIN1 7
#define LEFT_MOTOR_PIN2 8
#define RIGHT_MOTOR_PIN1 5
#define RIGHT_MOTOR_PIN2 6 
void setup(){
  Serial.begin(9600);
    pinMode(LEFT_MOTOR_PIN1, OUTPUT);
  pinMode(LEFT_MOTOR_PIN2, OUTPUT);
  pinMode(RIGHT_MOTOR_PIN1, OUTPUT);
  pinMode(RIGHT_MOTOR_PIN2, OUTPUT);
}

void loop() {
 digitalWrite(LEFT_MOTOR_PIN1, HIGH); // FORWARD
  digitalWrite(LEFT_MOTOR_PIN2, LOW);
  digitalWrite(RIGHT_MOTOR_PIN1, HIGH);
  digitalWrite(RIGHT_MOTOR_PIN2, LOW);
//  delay(6000);
//  digitalWrite(LEFT_MOTOR_PIN1, LOW);
//  digitalWrite(LEFT_MOTOR_PIN2, LOW);
//  digitalWrite(RIGHT_MOTOR_PIN1, LOW);
//  digitalWrite(RIGHT_MOTOR_PIN2, LOW);
//  delay(3000);
//  Serial.print("hasg");
}
