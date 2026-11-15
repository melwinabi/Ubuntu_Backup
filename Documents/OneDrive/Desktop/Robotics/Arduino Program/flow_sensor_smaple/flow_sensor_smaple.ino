const int = 27; // Example pin, replace with your actual pin
volatile int flowPulseCount = 0;
unsigned long lastFlowTime;
float flowRate;
float flowVolume;

void setup() {
  Serial.begin(115200);
  pinMode(flowSensorPin, INPUT_PULLUP); // Assuming the flow sensor output is connected to a digital pin
  attachInterrupt(digitalPinToInterrupt(flowSensorPin), flowSensorISR, FALLING);
  lastFlowTime = millis();
}

void loop() {
  unsigned long currentTime = millis();
  if (currentTime - lastFlowTime > 1000) { // Calculate flow rate and volume every second
    detachInterrupt(digitalPinToInterrupt(flowSensorPin));
    
    // Calculate flow rate
    flowRate = ((1000.0 / (currentTime - lastFlowTime)) * flowPulseCount) / 7.5; // Flow rate in L/min
    
    // Calculate flow volume
    flowVolume += (flowRate / 60.0) * (currentTime - lastFlowTime); // Flow volume in liters
    
    Serial.print("Flow rate: ");
    Serial.print(flowRate);
    Serial.print(" L/min\tTotal volume: ");
    Serial.print(flowVolume);
    Serial.println(" Liters");
    
    flowPulseCount = 0;
    lastFlowTime = currentTime;
    attachInterrupt(digitalPinToInterrupt(flowSensorPin), flowSensorISR, FALLING);
  }
}

void flowSensorISR() {
  flowPulseCount++;
}
