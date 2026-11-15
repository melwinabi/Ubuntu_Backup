

const int trigPin = 3; // Trigger us
const int echoPinH = 4; // Echo us
const int echoPinS = 2; //eco side


void setup(){
  Serial.begin(9600);
  pinMode(3,OUTPUT);
  pinMode(4,INPUT);
   pinMode(2,INPUT);
}


void loop() {   // void loop
  
   
   long durationH,distanceH,durationS,distanceS;  //ultra sonic program start
   pinMode(trigPin, OUTPUT);  //wave trig
   digitalWrite(trigPin, LOW);
   delayMicroseconds(2);
   digitalWrite(trigPin, HIGH);
   delayMicroseconds(10);
   digitalWrite(trigPin, LOW); //...
  
 
   pinMode(echoPinH, INPUT);  // formula
   durationH = pulseIn(echoPinH, HIGH);
   distanceH = durationH * 0.0344;  //...


    pinMode(echoPinS, INPUT);  // formula
   durationS = pulseIn(echoPinS, HIGH);
   distanceS = durationS * 0.0344;  //...
   
   Serial.print("cmH");
   Serial.print(distanceH);
   Serial.println("   ");
   delay (500);  // ultra sonic program end

    Serial.print("cmS");
   Serial.print(distanceS);
   Serial.println("   ");
   delay (500);  // ultra sonic program end

   if (distanceH>25){

    s1.write(30);
   }
}
