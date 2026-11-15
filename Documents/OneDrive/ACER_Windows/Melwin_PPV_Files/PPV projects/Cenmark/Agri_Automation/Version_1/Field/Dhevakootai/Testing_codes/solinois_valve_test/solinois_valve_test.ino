int VO_1 = 27;
int VO_2 = 26;

void setup() {
  pinMode(VO_1, OUTPUT);
  pinMode(VO_2, OUTPUT);
}

void loop() {
  digitalWrite(VO_2, LOW);
  delay(250);
  digitalWrite(VO_1, HIGH);
  delay(500);
  digitalWrite(VO_1, LOW);
  delay(2000);
  digitalWrite(VO_1, LOW);
  delay(250);
  digitalWrite(VO_2, HIGH);
  delay(500);//chg
  digitalWrite(VO_2, LOW);
  delay(2000);
}
