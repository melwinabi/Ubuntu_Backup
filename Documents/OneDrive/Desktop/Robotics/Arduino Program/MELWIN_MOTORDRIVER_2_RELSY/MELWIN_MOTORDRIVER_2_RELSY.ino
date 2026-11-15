void setup(){
  Serial.begin(9600);
  pinMode(2, OUTPUT);
  pinMode(3, OUTPUT);
  pinMode(4, OUTPUT);
}
void loop(){
   while (Serial.available())
  {
    char inChar = (char)Serial.read();
    Serial.println(inChar);
    if (inChar == 'F'){
      digitalWrite(2, LOW);
      digitalWrite(3, HIGH);
    }
       if (inChar == 'B'){
        digitalWrite(2, HIGH);
        digitalWrite(3, LOW);
       }
        if (inChar == 'R'){
         digitalWrite(4,HIGH);
         }
         if (inChar == 'r'){
         digitalWrite(4,LOW);
      }

  }
}
