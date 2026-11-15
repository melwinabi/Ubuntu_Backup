void setup(){
  Serial.begin(9600);
  pinMode(2, OUTPUT);
  pinMode(3, OUTPUT);
  pinMode(4, OUTPUT);
  pinMode(5, OUTPUT);
  //........................
  pinMode(6, OUTPUT);
  pinMode(7, OUTPUT);
  pinMode(8, OUTPUT);
  pinMode(9, OUTPUT);
  delay(2000);
}
void loop()
{
  while (Serial.available())
  {
    char inChar = (char)Serial.read();
    if (inChar == 'F'){
      Serial.println(inChar);
      digitalWrite(2, LOW);
      digitalWrite(3, HIGH);
      digitalWrite(4, LOW);
      digitalWrite(5, HIGH);
    }
     if (inChar == 'B'){
      Serial.println(inChar);
      digitalWrite(2, HIGH);
      digitalWrite(3, LOW);
      digitalWrite(4, HIGH);
      digitalWrite(5, LOW);
    }
     if (inChar == 'R'){
      Serial.println(inChar);
      digitalWrite(2, HIGH);
      digitalWrite(3, LOW);
      digitalWrite(4, LOW);
      digitalWrite(5, HIGH);
    }
     if (inChar == 'L'){
      Serial.println(inChar);
      digitalWrite(2, LOW);
      digitalWrite(3, HIGH);
      digitalWrite(4, HIGH);
      digitalWrite(5, LOW);
    }
     if (inChar == 'S'){
      Serial.println(inChar);
      digitalWrite(2, LOW);
      digitalWrite(3, LOW);
      digitalWrite(4, LOW);
      digitalWrite(5, LOW);
    }
     if (inChar == 'U'){
      Serial.println(inChar);
      digitalWrite(6, LOW);
      digitalWrite(7, HIGH);
    } if (inChar == 'D'){
      digitalWrite(6, HIGH);
      digitalWrite(7, LOW);
    }if (inChar == 'G'){
      Serial.println(inChar);
      digitalWrite(8, LOW);
      digitalWrite(9, HIGH);
    }if (inChar == 'g'){
      Serial.println(inChar);
      digitalWrite(8, HIGH);
      digitalWrite(9, LOW);
} 
  }
  }
