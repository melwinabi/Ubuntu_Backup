void setup(){
  int i,j;
  for (i=7;i<=13;i++)
  {
   pinMode(i,OUTPUT);
  }
  for (j=0;j<=6;j++)
  {
  pinMode(j,INPUT);
  digitalWrite(j,HIGH);
  }
}
void loop(){
 int i,j,k,l;
if(digitalRead(0)==LOW){
  digitalWrite(13,HIGH);
}
else if(digitalRead(0)==HIGH){
  digitalWrite(13,LOW);
}

 if (digitalRead(2)==LOW){
  for(k=8;k<=10;k++){
 digitalWrite(k,HIGH);
  }
 } 
 else if (digitalRead(2)==HIGH)
 {
  for(k=8;k<=10;k++){
 digitalWrite(k,LOW);
  }
}  
if(digitalRead(3)==LOW){
  digitalWrite(7,HIGH);
}
else if(digitalRead(3)==HIGH){
  digitalWrite(7,LOW);
  }
   

  
  for(l=11;l<=12;l++)
  {
 if (digitalRead(5)==LOW)
 digitalWrite(l,HIGH); 
 else if (digitalRead(5)==HIGH)
 digitalWrite(l,LOW);
  }
  }
  
  
