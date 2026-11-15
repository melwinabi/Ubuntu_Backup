
double temp;
float voltage;
int sensorpin =A0;
void setup(){
pinMode(10,OUTPUT);

}
void loop()
{
double temp=analogRead(A0);
// float voltage = (temp * (5.0/1023.0));
//float value = (voltage-0.5)*100;
temp = double(temp)/1024;
temp=temp * 5;
temp=temp + 0.5;
temp=temp * 100;

if (temp >=60)
{
digitalWrite(10,HIGH);

  }
  else
  {
    digitalWrite(10,LOW);
    
    }
 

}
