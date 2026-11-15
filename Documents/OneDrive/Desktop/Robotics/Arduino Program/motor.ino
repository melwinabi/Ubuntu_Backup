void setup(){
pinMode(3,OUTPUT); 
pinMode(4,OUTPUT);//led
pinMode(5,OUTPUT);
pinMode(6,OUTPUT);

}

void loop()
{

  digitalWrite(3,HIGH);
  digitalWrite(4,LOW);
  delay(500);
  digitalWrite(4,HIGH);
  digitalWrite(5,LOW);
  delay(500); 
  digitalWrite(5,HIGH);
  digitalWrite(6,LOW);
  delay(500);
  digitalWrite(5,HIGH);
  digitalWrite(6,LOW);
  delay(500); 
}
