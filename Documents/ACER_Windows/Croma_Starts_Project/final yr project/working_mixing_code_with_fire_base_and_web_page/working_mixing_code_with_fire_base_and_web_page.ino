#include <ESP8266WiFi.h>
#include <LiquidCrystal_I2C.h>
#include <Wire.h>
#include <FirebaseESP8266.h>
#define FIREBASE_HOST "smart-agri-robot-default-rtdb.firebaseio.com" 
#define FIREBASE_AUTH "C9S9ankE953cGgua6PdHsF1ne7zcCdrw8O0jU1RT" 
#define WIFI_SSID "MelwinAbi" 
#define WIFI_PASSWORD "Melwin@321"
#define Relay_1 D3
#define Relay_2 D4
#define Relay_3 D5
#define Relay_4 D6
#define Active  D7
FirebaseData firebaseData;
FirebaseAuth auth;
FirebaseConfig config;
int i=0;
int lcdColumns = 16;
int lcdRows = 2;
LiquidCrystal_I2C lcd(0x27,16,2);  
void setup() {
Serial.begin(9600);
lcd.init();
lcd.backlight();
lcd.clear();
lcd.setCursor(0,0);
lcd.print("HAPPY FARMING");
Serial.println("Serial communication started\n\n");
WiFi.begin(WIFI_SSID, WIFI_PASSWORD); 
Serial.print("Connecting to ");
Serial.print(WIFI_SSID);
while (WiFi.status() != WL_CONNECTED) {
Serial.print(".");
digitalWrite(Active,HIGH);
delay(500);
}
Serial.println();
Serial.print("Connected to ");
Serial.println(WIFI_SSID);
Serial.print("IP Address is : ");
Serial.println(WiFi.localIP()); 
//Firebase.begin(FIREBASE_HOST, FIREBASE_AUTH); 
config.database_url = FIREBASE_HOST;
config.signer.tokens.legacy_token = FIREBASE_AUTH;
Firebase.begin(&config, &auth);
Firebase.reconnectWiFi(true);
pinMode(Relay_1,OUTPUT);
pinMode(Relay_2,OUTPUT);
pinMode(Relay_3,OUTPUT);
pinMode(Relay_4,OUTPUT);
pinMode(Active,OUTPUT);
digitalWrite(Active,LOW);


delay(1000);
}

void loop ()
{
  if(Firebase.getString(firebaseData, "/STATUS_A"))
  {
    
    String STATUS = firebaseData.stringData();
    if(STATUS.toInt() == 1){
      Serial.println("onA");
      Type_A();
      
      
    }
    else if(STATUS.toInt() == 0){
       Serial.println("oFFA");
       Type_o();
    }
  }

  if(Firebase.getString(firebaseData, "/STATUS_B"))
  {
    
    String STATUS = firebaseData.stringData();
    if(STATUS.toInt() == 1){
      Serial.println("onB");
      Type_B();
    }
    else if(STATUS.toInt() == 0){
       Serial.println("oFFB");
       Type_o();
    }
  }

  if(Firebase.getString(firebaseData, "/STATUS_C"))
  {
    
    String STATUS = firebaseData.stringData();
    if(STATUS.toInt() == 1){
      Serial.println("onC");
      Type_C();
    }
    else if(STATUS.toInt() == 0){
       Serial.println("oFFC");
       Type_o();
    }
  }

  if(Firebase.getString(firebaseData, "/STATUS_D"))
  {
    
    String STATUS = firebaseData.stringData();
    if(STATUS.toInt() == 1){
      Serial.println("onD");
      Type_D();
    }
    else if(STATUS.toInt() == 0){
       Serial.println("oFFD");
       Type_o();
    }
  }
  if(Firebase.getString(firebaseData, "/ON_OFF"))
  {
    
    String STATUS = firebaseData.stringData();
    if(STATUS.toInt() == 1){
      Serial.println("SEND SIGNAL TO ARDUINO");
      lcd.clear();
      lcd.setCursor(0,0);
      lcd.print("SPRAYING");
      digitalWrite(Active,HIGH);
    }
    else if(STATUS.toInt() == 0){
       Serial.println("oFFD");
       digitalWrite(Active,LOW);
       lcd.clear();
       lcd.setCursor(0,0);
       lcd.print("PROCESSING.....");
      
     
    }else{
       digitalWrite(Active,LOW); 
       lcd.clear();
       
    }
  }




  
}

void Type_A(){ 
  Serial.print("MIXING SOLUTION TYPE A");
  lcd.clear();
  lcd.setCursor(0,1);
  lcd.print("SOLUTION TYPE A");
  digitalWrite(Relay_1,LOW);
  delay(1000);
  digitalWrite(Relay_2,LOW);
  delay(2000);
  digitalWrite(Relay_3,LOW);
  delay(3500);
  digitalWrite(Relay_4,LOW);
  delay(2800);
}

void Type_B(){
  Serial.print("MIXING SOLUTION TYPE B");
  lcd.clear();
  lcd.setCursor(0,1);
  lcd.print("SOLUTION TYPE B");
  digitalWrite(Relay_3,LOW);
  delay(1000);
  digitalWrite(Relay_4,LOW);
  delay(2000);
  digitalWrite(Relay_2,LOW);
  delay(3500);
  digitalWrite(Relay_1,LOW);
  delay(2800);
}

void Type_C(){
  Serial.print("MIXING SOLUTION TYPE C");
  lcd.clear();
  lcd.setCursor(0,1);
  lcd.print("SOLUTION TYPE C");
  digitalWrite(Relay_4,LOW);
  delay(1000);
  digitalWrite(Relay_3,LOW);
  delay(2000);
  digitalWrite(Relay_2,LOW);
  delay(3500);
  digitalWrite(Relay_1,LOW);
  delay(2800);
}
void Type_D(){
  Serial.print("MIXING SOLUTION TYPE D");
  lcd.clear();
  lcd.setCursor(0,1);
  lcd.print("SOLUTION TYPE D");
  digitalWrite(Relay_2,LOW);
  delay(1000);
  digitalWrite(Relay_1,LOW);
  delay(2000);
  digitalWrite(Relay_4,LOW);
  delay(3500);
  digitalWrite(Relay_3,LOW);
  delay(2800);
}
void Type_o(){
  digitalWrite(Relay_1,HIGH);
  digitalWrite(Relay_2,HIGH);
  digitalWrite(Relay_3,HIGH);
  digitalWrite(Relay_4,HIGH);
}
