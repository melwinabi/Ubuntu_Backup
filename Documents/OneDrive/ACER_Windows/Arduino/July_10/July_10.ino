#include<SoftwareSerial.h>
String readString;
char c;
int a;
int StringLocation_bf;
int StringLocation_af;
String weight_bf;
String weight_af;
void setup() 
{
  Serial.begin(9600); 

}

void loop() 
{
  
  if (Serial.available()>0) 
  {
    c = Serial.read();
    if (c == '*') 
    {
      StringLocation_bf = readString.indexOf('*');
      weight_bf = readString.substring(0,StringLocation_bf);
      weight_bf.trim();
      Serial.println(weight_bf);
      readString="";
    }
    else
    {
      readString += c;
    }
     
    
}
}
