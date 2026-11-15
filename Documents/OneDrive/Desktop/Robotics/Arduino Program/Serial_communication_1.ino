void  setup()
{
  pinMode(3,OUTPUT);
  pinMode(4,OUTPUT);
  Serial.begin(9600);
}
void loop()
{
  if (Serial.available ()>0)
  {
   int state = Serial.read();
   if(state == 'A')
   {
    digitalWrite(3,HIGH);
    digitalWrite(4,HIGH);
    
  Serial.println("LED ON"); 
   }
  if(state == 'B')
  {
    
  digitalWrite(3,LOW);
    digitalWrite(4,LOW);
  Serial.println("LED OFF");
  }
  }
  delay(1000);
  
  

}
