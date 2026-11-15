 double temp;
void setup() 
{
  Serial.begin(9600);
  int Sensorpin =A0;
  
 }

void loop() 
{
  double temp = analogRead(A0);
  temp = double(temp)/1024;
  temp = temp*5;
  temp = temp+ 0.5;
  temp = temp *100;
  Serial.println("temperature");
  Serial.print(temp);
   if(Serial.available()>0)
  {
    if(temp > 70)
    {
      Serial.println("temperature high");
    }
    else if (temp < 70)
    {
      Serial.println("temperature low");
    }
    
    "clscr";
    

  }
  
}
