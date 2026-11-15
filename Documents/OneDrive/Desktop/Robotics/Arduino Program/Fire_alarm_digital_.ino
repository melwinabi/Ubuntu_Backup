void setup(){
pinMode(6,INPUT);
pinMode(7,OUTPUT);//led
pinMode(8,OUTPUT);//bzr
}
void loop()
{
if(digitalRead(6) ==HIGH) 
{
  digitalWrite(7,HIGH);
  delay(500);
  digitalWrite(8,HIGH);
  delay(500);
  digitalWrite(7,LOW);
  delay(500);
  digitalWrite(8,LOW);
  delay(500); 
}

}
