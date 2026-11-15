#define enA 4
#define enB 9
int ir1=2;
int ir2=3;

void setup(){
Serial.begin(9600);
pinMode(2,INPUT);
pinMode(3,INPUT);
pinMode(5, OUTPUT); // motor 1
pinMode(6, OUTPUT);
pinMode(7, OUTPUT);//motor 2
pinMode(8, OUTPUT);
pinMode(enA, OUTPUT);
pinMode(enB, OUTPUT);
analogWrite(enA, 200); // Write The Duty Cycle 0 to 255 Enable Pin A for Motor1 Speed 
analogWrite(enB, 200);
}

void loop(){
  while(1){
  int IR1 = digitalRead(ir1); // Set the GPIO as Input
  Serial.print("IR1");
  Serial.print(IR1);
  
  int IR2 = digitalRead(ir2); // Set the GPIO as Input
  Serial.print("  IR2");
  Serial.println(IR2);
  delay(1000);
  
   if ( (IR1="0" )&& (IR2 ="0")){
  
   forward();
   } 
 
  else if (( IR1="1" )&& (IR2 ="1")){
  stop();
  }

}
  }

//
//
void forward(){
  Serial.println(".......MOVING FORWARD.........");
   digitalWrite(5,HIGH);
   digitalWrite(6,LOW);
  digitalWrite(7,HIGH);
   digitalWrite(8,LOW);

}
//
void stop(){
  Serial.println("........STOP..........");
   digitalWrite(5,LOW);
   digitalWrite(6,LOW);
   digitalWrite(7,LOW);
   digitalWrite(8,LOW);

}
