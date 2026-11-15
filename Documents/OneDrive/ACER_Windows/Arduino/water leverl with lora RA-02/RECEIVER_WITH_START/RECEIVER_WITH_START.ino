 #include <SPI.h>
#include <LoRa.h>
const int buttonPin1 = 5; // 11
const int buttonPin2 = 6; // 12

const int ledPin_sts =  8; // 14
const int ledPin1 =  A0; // 23
const int ledPin2 =  A1; // 24
const int ledPin3 =  A2; // 25
const int ledPin4 =  A3; // 26
const int ledPin5 =  A4; // 27
const int ledPin6 =  A5; // 28

const int relay =  4; // 6

int buttonState1 = 0;
int buttonState2 = 0;

int a[10];
int b[10];
int packetSize;

void setup() {
  Serial.begin(9600);
  pinMode(ledPin_sts, OUTPUT);
  pinMode(ledPin1, OUTPUT);
  pinMode(ledPin2, OUTPUT);
  pinMode(ledPin3, OUTPUT);
  pinMode(ledPin4, OUTPUT);
  pinMode(ledPin5, OUTPUT);
  pinMode(ledPin6, OUTPUT);
  
  pinMode(relay, OUTPUT);
  
  pinMode(buttonPin1, INPUT);
  pinMode(buttonPin2, INPUT);
  while (!Serial);

  Serial.println("LoRa Receiver");

  if (!LoRa.begin(433E6)) {
    Serial.println("Starting Communication failed!");
    while (1);
    digitalWrite(ledPin2, LOW);
    digitalWrite(ledPin5, HIGH);
    delay(500);
    digitalWrite(ledPin2, HIGH);
    digitalWrite(ledPin5, LOW);
    delay(500);
  }
  LoRa.setTxPower(17);
  LoRa.setSPIFrequency(900); 
}

void loop() 
{
  buttonState1 = digitalRead(buttonPin1);
  buttonState2 = digitalRead(buttonPin2);
  LoRa.beginPacket();
  LoRa.print("7");
  LoRa.endPacket();
  delay(20);
  a[0]=0;a[1]=0;a[2]=0;a[3]=0;
  packetSize = LoRa.parsePacket();
  if (packetSize) 
  {
      while (LoRa.available()) 
      {
        for ( int i = 0; i <= packetSize; i++)
        {
          a[i] = ((int)LoRa.read());
        }
      }
  }
  Serial.print("A value ");
  Serial.println(a[0]);
  delay(2000);
  LoRa.beginPacket();
  LoRa.print("9");
  LoRa.endPacket();
  delay(20);
  b[0]=0;b[1]=0;b[2]=0;b[3]=0;
  packetSize = LoRa.parsePacket();
  if (packetSize) 
  {
      while (LoRa.available()) 
      {
        for ( int i = 0; i <= packetSize; i++)
        {
          b[i] = ((int)LoRa.read());
        }
      }
  }
  Serial.print("B value ");
  Serial.println(b[0]);
  delay(2000);
  if ( a[0] == 53 )
  {
    Serial.println("Enter Device 1");
    if ( a[1] == 48 && a[2] == 48 && a[3] == 48 )
    {
      Serial.println("Tank 1 Empty");
      digitalWrite(ledPin1, HIGH);
      digitalWrite(ledPin2, HIGH);
      digitalWrite(ledPin3, HIGH);
    }
    else if ( a[1] == 49 && a[2] == 48 && a[3] == 48 )
    {
      Serial.println("Tank 1 Low");
      digitalWrite(ledPin1, HIGH);
      digitalWrite(ledPin2, LOW);
      digitalWrite(ledPin3, HIGH);
    }
    else if ( a[1] == 49 && a[2] == 49 && a[3] == 48 )
    {
      Serial.println("Tank 1 Medium");
      digitalWrite(ledPin1, HIGH);
      digitalWrite(ledPin2, HIGH);
      digitalWrite(ledPin3, LOW);
    }
    else if ( a[1] == 49 && a[2] == 49 && a[3] == 49 )
    {
      Serial.println("Tank 1 Full");
      digitalWrite(ledPin1, LOW);
      digitalWrite(ledPin2, HIGH);
      digitalWrite(ledPin3, HIGH);
    }
    else
    {
      Serial.println("Tank 1 Invalid");
      digitalWrite(ledPin1, LOW);
      digitalWrite(ledPin2, LOW);
      digitalWrite(ledPin3, LOW);
    }
  }
  if ( b[0] == 49 )
  {
    Serial.println("Enter Device 2");
    if ( b[1] == 48 && b[2] == 48 && b[3] == 48 )
    {
      Serial.println("Tank 2 Empty");
      digitalWrite(ledPin4, HIGH); // GREEN
      digitalWrite(ledPin5, HIGH); // RED
      digitalWrite(ledPin6, HIGH); // BLUE
    }
    else if ( b[1] == 49 && b[2] == 48 && b[3] == 48 )
    {
      Serial.println("Tank 2 Low");
      digitalWrite(ledPin4, HIGH);
      digitalWrite(ledPin5, LOW);
      digitalWrite(ledPin6, HIGH);
    }
    else if ( b[1] == 49 && b[2] == 49 && b[3] == 48 )
    {
      Serial.println("Tank 2 Medium");
      digitalWrite(ledPin4, HIGH);
      digitalWrite(ledPin5, HIGH);
      digitalWrite(ledPin6, LOW);
    }
    else if ( b[1] == 49 && b[2] == 49 && b[3] == 49 )
    {
      Serial.println("Tank 2 Full");
      digitalWrite(ledPin4, LOW);
      digitalWrite(ledPin5, HIGH);
      digitalWrite(ledPin6, HIGH);
    }
    else
    {
      Serial.println("Tank 2 Invalid");
      digitalWrite(ledPin4, LOW);
      digitalWrite(ledPin5, LOW);
      digitalWrite(ledPin6, LOW);
    }
  }
  else
    {
      Serial.println("Tank 1 and 2 Invalid");
      digitalWrite(ledPin1, HIGH);
      digitalWrite(ledPin2, LOW);
      digitalWrite(ledPin3, LOW);
      digitalWrite(ledPin4, HIGH);
      digitalWrite(ledPin5, LOW);
      digitalWrite(ledPin6, LOW);
    }
}
