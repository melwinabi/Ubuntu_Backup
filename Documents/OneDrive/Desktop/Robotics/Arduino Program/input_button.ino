void setup() {
  pinMode(2,INPUT);
  digitalWrite(2,HIGH);
  pinMode(13,OUTPUT);
  
  // put your setup code here, to run once:

}

void loop() {
  if(digitalRead(2)==HIGH)
  {
  digitalWrite(13,LOW);
  }

  else
  {
    digitalWrite(13,HIGH);
  }
  }
  
  
