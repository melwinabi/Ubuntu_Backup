void setup(){
   pinMode(7,OUTPUT);//R
  pinMode(8,OUTPUT);
  pinMode(9,OUTPUT);//L
  pinMode(10,OUTPUT);
}

void loop(){
   digitalWrite(9,HIGH);//RT
  digitalWrite(10,LOW);
  digitalWrite(7,LOW);
  digitalWrite(8,HIGH);
}
 
