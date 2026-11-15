

#include <SoftwareSerial.h>
SoftwareSerial BTSerial(6,7); // RX | TX




 
void setup() 
{

 Serial.begin(9600);
  BTSerial.begin(9600);  
  

}
 
void loop()
{
 
  
BTSerial.println("haiiii");
    
}
