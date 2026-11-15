#include <Servo.h>
Servo  myservo;
String container1, container2;
int timer2_counter, Timer_Second;
String url = "" ;

int trigerpin1 = 2;
int echo1 = 3;
int duration1;
int distance1;

int trigerpin2 = 4;
int echo2 = 5;
int duration2;
int distance2;

int trigerpin3 = 7;
int echo3 = 8;
int duration3;
int distance3;

int trigerpin4 = A2;
int echo4 = A3;
int duration4;
int distance4;

int servo_pin = 6;
int relay_pin = A4;
int moisture_pin = A0;
int IR_pin = A1;
int flag = 0;
int count;
int motor_pin1 = 9;
int motor_pin2 = 10;
int motor_pin3 = 11;
int motor_pin4 = 12;

void setup() {
  Serial.begin(9600);
  pinMode(trigerpin1, OUTPUT);
  pinMode(echo1, INPUT);
  pinMode(trigerpin2, OUTPUT);
  pinMode(echo2, INPUT);
  pinMode(trigerpin3, OUTPUT);
  pinMode(echo3, INPUT);
  pinMode(trigerpin4, OUTPUT);
  pinMode(echo4, INPUT);

  myservo.attach(servo_pin);   // attaches the serve on pin 9 to the sarve object
  myservo.write (90);
  pinMode(moisture_pin, INPUT);
  pinMode(IR_pin, INPUT);
  pinMode(relay_pin, OUTPUT);
  digitalWrite(relay_pin, LOW);
 
  pinMode(motor_pin1, OUTPUT);
  pinMode(motor_pin2, OUTPUT);
  pinMode(motor_pin3, OUTPUT);
  pinMode(motor_pin4, OUTPUT);

  digitalWrite(motor_pin1, HIGH);
  digitalWrite(motor_pin2, LOW);
  digitalWrite(motor_pin3, HIGH);
  digitalWrite(motor_pin4, LOW);

  noInterrupts();
  TCCR2A = 0;
  TCCR2B |= (1 << CS22) | (1 << CS21) | (1 << CS20);
  TCNT2 = timer2_counter;
  TIMSK2 &= ~(1 << TOIE2);
  timer2_counter = 240;
  interrupts();
  delay(1000);

}
void loop()
{
  ultrasonic1();
  ultrasonic2();
  ultrasonic3();
  ultrasonic4();

  if ((distance2 < 15) && (flag == 0))
  {
    Backward();
    delay(1000);
    Right();
    delay(1000);
  }
  else if ((distance2 > 15) && (flag == 0))
  {
    Forward();
  }

  if ((distance1 < 15) && (flag == 0))
  {
    TIMSK2 |= (1 << TOIE2);
   // Serial.println("distance1");

  }
  else if ((distance1 > 15) && (flag == 0))
  {
    digitalWrite(relay_pin, LOW);
  }
  if ((distance3 >= 9) && (flag == 0))    //dry
  {
    container1 = "EMPTY";
    //Serial.print("EMPTY");
  }
  else if ((distance3 <= 8) && (distance3 >= 4)&& (flag == 0))  //dry
  {
    container1 = "MEDIUM";
    //Serial.print("\t");
    //Serial.print("MEDIUM");
  }
  else if ((distance3 <= 3)&& (flag == 0)) 
  {
    container1 = "FULL";
    // Serial.print("\t");
    // Serial.print("FULL");
    flag=1;
     Stop();
     digitalWrite(relay_pin,LOW);

  }

  if ((distance4 >= 9) && (flag == 0)) //wet
  {
    container2 = "EMPTY";

    // Serial.print("EMPTY");
  }
  else if ((distance4 <= 8) && (distance4 >= 4)&& (flag == 0))  //dry
  {
    container2 = "MEDIUM";
    // Serial.print("\t");
    // Serial.print("MEDIUM");
  }
  else if ((distance4 <= 3)&& (flag == 0)) 
  {
    container2 = "FULL";
    // Serial.print("\t");
    // Serial.println("FULL");
     flag=1;
      Stop();
        digitalWrite(relay_pin,LOW);

  }
  if ((digitalRead(IR_pin) == LOW) && (digitalRead(moisture_pin) == HIGH)&& (flag == 0)) 
  {
    myservo.write (45);
    delay(500);
    myservo.write (90);
  }

  else if ((digitalRead(moisture_pin) == LOW) && (digitalRead(IR_pin) == LOW)&& (flag == 0)) 
  {
    myservo.write (125);
    delay(500);
    myservo.write (90);
  }
  else if ((digitalRead(IR_pin) == HIGH) && (digitalRead(moisture_pin) == HIGH)&& (flag == 0)) 
  {
    myservo.write (90);
  }
  count++;
  
  if(count==200)
  {
   url = "/update?api_key=G5AQSOEM2UAVE8IW&";//G5AQSOEM2UAVE8IW
  url += "field1=";
  url += container1;
  url += "&field2=";
  url += container2;
  Serial.println(url);
  count=0;
  }

}

ISR(TIMER2_OVF_vect)
{
  digitalWrite(relay_pin, HIGH);
  TCNT2 = timer2_counter;
  Timer_Second++;
 
  if (Timer_Second == 10000)
  {
    digitalWrite(relay_pin, LOW);
    TIMSK2 &= ~(1 << TOIE2);
    Timer_Second = 0;
  }
}

void ultrasonic1()
{
  digitalWrite(trigerpin1, LOW);
  delayMicroseconds(2);
  digitalWrite(trigerpin1, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigerpin1, LOW);
  duration1 = pulseIn(echo1, HIGH);
  distance1 = duration1 * .034 / 2;
 // Serial.println(distance1);
  //delay(200);
}
void ultrasonic2()
{
  digitalWrite(trigerpin2, LOW);
  delayMicroseconds(2);
  digitalWrite(trigerpin2, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigerpin2, LOW);
  duration2 = pulseIn(echo2, HIGH);
  distance2 = duration2 * .034 / 2;
  // Serial.print("\t");
  // Serial.print(distance2);
  //delay(200);
}
void ultrasonic3()
{
  digitalWrite(trigerpin3, LOW);
  delayMicroseconds(2);
  digitalWrite(trigerpin3, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigerpin3, LOW);
  duration3 = pulseIn(echo3, HIGH);
  distance3 = duration3 * .034 / 2;
  //Serial.print("\t");
  //Serial.print(distance3);
  //delay(200);
}
void ultrasonic4()
{
  digitalWrite(trigerpin4, LOW);
  delayMicroseconds(2);
  digitalWrite(trigerpin4, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigerpin4, LOW);
  duration4 = pulseIn(echo4, HIGH);
  distance4 = duration4 * .034 / 2;
  //Serial.print("\t");
  //Serial.println(distance4);
 // delay(200);
}
void Forward()
{
  digitalWrite(motor_pin1, HIGH);
  digitalWrite(motor_pin2, LOW);
  digitalWrite(motor_pin3, HIGH);
  digitalWrite(motor_pin4, LOW);
}
void Backward()
{
  digitalWrite(motor_pin1, LOW );
  digitalWrite(motor_pin2, HIGH);
  digitalWrite(motor_pin3, LOW);
  digitalWrite(motor_pin4, HIGH);
}
void Right()
{
  digitalWrite(motor_pin1, HIGH);
  digitalWrite(motor_pin2, LOW);
  digitalWrite(motor_pin3, LOW);
  digitalWrite(motor_pin4, HIGH);
}
void Stop()
{
  digitalWrite(motor_pin1, LOW);
  digitalWrite(motor_pin2, LOW);
  digitalWrite(motor_pin3, LOW);
  digitalWrite(motor_pin4, LOW);
}
