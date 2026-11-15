
const int LED = 5;

char switchstate=1;


void setup()  {//Here the code only runs once.
Serial.begin(9600);
pinMode(LED,  OUTPUT);

}bbb
void loop() {
while(Serial.available()>0){ 
 //code to be  executed only when Serial.available()>0

switchstate = Serial.read();


Serial.print(switchstate);

Serial.print("haii");

delay(15);

if(switchstate  == "hello"){
 digitalWrite(5, HIGH);
 
}
else if(switchstate == '0'){
 digitalWrite(5, LOW);
}
}
}
