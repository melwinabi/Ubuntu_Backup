int ldr=A0;
void setup(){
pinMode(9,OUTPUT);//led
pinMode(10,OUTPUT);//intrrupt led  
pinMode(2,INPUT);//intrupyt IN
//attachInterrupt(digitalPinToInterrupt(2), blink,FALLING);
}
void loop(){
digitalWrite(9,HIGH);
delay(1000);
digitalWrite(9,LOW);
delay(1000);

if(ldr<200)
{
digitalWrite(10,HIGH);  
}
}
