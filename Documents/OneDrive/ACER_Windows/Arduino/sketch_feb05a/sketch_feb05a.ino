
const int M0 = 18;  
const int M1 = 19;  

void setup() {
  Serial.begin(9600);        
  Serial2.begin(9600);    

  pinMode(M0, OUTPUT);
  pinMode(M1, OUTPUT);
  digitalWrite(M0, LOW);
  digitalWrite(M1, LOW);
  
  Serial.println("LoRa Transmitter Ready");
}

void loop() {
  String message = "Hello from LoRa TXdrtrddgfdg!";
  
  // Send message via LoRa
  Serial2.println(message);
  Serial.println("Sent: " + message);
  
  delay(2000);  // Wait for 2 seconds before sending the next message
}
