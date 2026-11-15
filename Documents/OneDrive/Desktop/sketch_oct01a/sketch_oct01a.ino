const int cd4067SigPin = A0;  // CD4067 signal pin connected to A0
const int cd4067S0 = 5;       // CD4067 control pin S0
const int cd4067S1 = 4;       // CD4067 control pin S1
const int cd4067S2 = 3;       // CD4067 control pin S2
const int cd4067S3 = 2;       // CD4067 control pin S3

void setup() {
  // put your setup code here, to run once:
  digitalWrite(cd4067S0, HIGH);
  digitalWrite(cd4067S1, HIGH);
  digitalWrite(cd4067S2, HIGH);
  digitalWrite(cd4067S3, HIGH);
  Serial.begin(9600);
}

void loop() {
  int sensorValue = analogRead(cd4067SigPin);
  Serial.print("Data :");
  Serial.println(sensorValue);
  delay(500);
  // put your main code here, to run repeatedly:

}
