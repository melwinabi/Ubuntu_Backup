void setup()
{
Serial.begin(9600);
pinMode(3,OUTPUT); 
pinMode(4,OUTPUT); 
pinMode(5,OUTPUT); 
pinMode(6,OUTPUT); 
pinMode(7,OUTPUT); 
pinMode(8,OUTPUT); 
  

}

void loop()
{ 
if(Serial.available()>0)
{
char state=Serial.read();

 if (state=='A')
{
digitalWrite(3,HIGH);
Serial.println("LED ON");
}
 if (state=='B')
{
digitalWrite(3,LOW);
Serial.println("LED OFF");
}
 if (state=='C')
{
digitalWrite(4,HIGH);
Serial.println("LED ON");
}
 if (state=='D')
{
digitalWrite(4,LOW);
Serial.println("LED OFF");
}
 if (state=='E')
{
digitalWrite(5,HIGH);
Serial.println("LED ON");
}
 if (state=='F')
{
digitalWrite(5,LOW);
Serial.println("LED OFF");
}
 if (state=='G')
{
digitalWrite(6,HIGH);
Serial.println("LED ON");
}

 if (state=='H')
{
digitalWrite(6,LOW);
Serial.println("LED OFF");
}
if (state=='I')
{
digitalWrite(7,HIGH);
Serial.println("LED ON");
}
 if (state=='J')
{
digitalWrite(7,LOW);
Serial.println("LED OFF");
}
 if (state=='K')
{
digitalWrite(8,HIGH);
Serial.println("LED ON");
}
 if (state=='L')
{
digitalWrite(8,LOW);
Serial.println("LED OFF");
}
if (state=='M')
{
digitalWrite(9,HIGH);
Serial.println("LED ON");
}
if (state=='N')
{
digitalWrite(9,LOW);
Serial.println("LED OFF");
}
if (state=='O')
{
digitalWrite(10,HIGH);
Serial.println("LED ON");
}
if (state=='P')
{
digitalWrite(10,LOW);
Serial.println("LED OFF");
}
}
}



  
