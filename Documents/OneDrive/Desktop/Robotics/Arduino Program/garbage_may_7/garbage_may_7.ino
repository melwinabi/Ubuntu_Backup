String container1, container2;
int trigerpin1 = 3;
int echo1 = 4;
 int duration1;
int distance1;
void ultrasonic1();
void ultrasonic2();
int trigerpin2 = 6;
int echo2 = 5;
int duration2;
int distance2;
int count;
String url = "" ;
void setup() {
  Serial.begin(9600);
  pinMode(trigerpin1, OUTPUT);
  pinMode(echo1, INPUT);
  pinMode(trigerpin2, OUTPUT);
  pinMode(echo2, INPUT);
}

void loop()
{
  ultrasonic1();
  ultrasonic2();
  if(distance1>=9){
//    Serial.println("BIN 1 EMPTY");
    container1 = "TRASH1_1_EMPTY";
   }
  else if((distance1==4)||(distance1==5)){
//    Serial.println("MEDIUM 1 TRASH");
    container1 = "TRASH_1_MEDIUM";
  }
  else if(distance1==2){
//   Serial.println("TRASH 1 FULL");
   container1 = "TRASH_1_FULL";
}
 if(distance2>=8){
//    Serial.println("BIN 2 EMPTY");
    container2 = "TRASH_2_EMPTY";
  }
  else if((distance2==4)||(distance2==5)){
//    Serial.println("MEDIUM 2 TRASH");
    container2 = "TRASH_2_MEDIUM";
  }
  else if(distance2==2){
//    Serial.println("TRASH 2 FULL");
    container2 = "TRASH_2_FULL";
}
 count++;
  
  if(count>20)
  {
  url = "/update?api_key=RCM0323E10UCR0L6&";//
  url += "field1=";
  url += container1;
  url += "&field2=";
  url += container2;
  Serial.println(url);
  count=0;
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
//  Serial.println(distance1);//9
  delay(200);
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
//  Serial.print("\t");
//  Serial.println(distance2);//8
  delay(200);
}
