char state; 

void setup()
{
  Serial.begin(9600);
  pinMode(13, OUTPUT);
  int i;
for(i=2;i<=13;i++)
{
pinMode(i,OUTPUT);  
} 
   
}

void loop()
{
  int i;
  if (Serial.available() > 0)
  { 
   char state = Serial.read();
    if (state== 'A') { 
digitalWrite(3,HIGH);
digitalWrite(5,HIGH);
digitalWrite(7,HIGH);
digitalWrite(9,HIGH);
digitalWrite(11,HIGH);
digitalWrite(13,HIGH);
Serial.print("LED 3,5,7,9,11,13, ARE ON");  
      
      
    } else {
      digitalWrite(i, LOW);
      Serial.println("ALL LED - off");
    }
if(state== 'B')
{
digitalWrite(2,HIGH);
digitalWrite(4,HIGH);
digitalWrite(6,HIGH);
digitalWrite(8,HIGH);
digitalWrite(10,HIGH);
digitalWrite(12,HIGH);
Serial.println("LED 4,6,8,10,12,ARE ON");
}
else{
  digitalWrite(i,LOW);
}
if(state== '3')
{
digitalWrite(i,LOW);
Serial.println("ALL LED ARE OFF");

  }
}
}
