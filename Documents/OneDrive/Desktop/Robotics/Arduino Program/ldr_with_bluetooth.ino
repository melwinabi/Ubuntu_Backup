int ldr = 7;
void setup(){
Serial.begin(9600);
pinMode(7,INPUT);
pinMode(8,OUTPUT); 
}
void loop(){
if(digitalRead(7)==HIGH){
Serial.println("LIGHT ON ");
delay(2000);
digitalWrite(8,HIGH);  
}
else{
Serial.println("LIGHT OFF "); 
delay(2000); 
digitalWrite(8,LOW); 
}
}
