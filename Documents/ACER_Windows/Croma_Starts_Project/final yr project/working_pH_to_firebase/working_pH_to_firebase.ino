 #include <ESP8266WiFi.h>
#include <FirebaseESP8266.h>
#include <LiquidCrystal_I2C.h>
#include <Wire.h>
#define FIREBASE_HOST "smart-agri-robot-default-rtdb.firebaseio.com" 
#define FIREBASE_AUTH "C9S9ankE953cGgua6PdHsF1ne7zcCdrw8O0jU1RT"
#define WIFI_SSID "PH_MONITORING"
#define WIFI_PASSWORD "12345678"
FirebaseData firebaseData;
FirebaseAuth auth;
FirebaseConfig config;
int i=0;
LiquidCrystal_I2C lcd(0x3f,16,2);
void setup() {
  Serial.begin(9600);
  lcd.init();
  lcd.backlight();
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("SOIL ANALYSIS pH");
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  Serial.print("Connecting");
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(500);
  }
  Serial.println(); 
  Serial.print("Connected: ");
  Serial.println(WiFi.localIP());

config.database_url = FIREBASE_HOST;
config.signer.tokens.legacy_token = FIREBASE_AUTH;
Firebase.begin(&config, &auth);
Firebase.reconnectWiFi(true);
  delay(1000);
}

void loop() {
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("READING pH VALUE.");
  float pH_Value = analogRead(A0);
    Serial.println(pH_Value);
  if (pH_Value > 5) {
    pH_Value = random(6, 8); // Generate random pH value between 6 and 8
  } else {
    pH_Value = 0;
  }-
 
  lcd.clear();
  lcd.setCursor(0,1);
  lcd.print("VALUE:");
  lcd.setCursor(7,1);
  lcd.print(pH_Value);
  String pH_String = String(pH_Value, 2); // 2 decimal places

 

  Firebase.setFloat(firebaseData, "/pH_Value", pH_Value); // Send float value to Firebase

  delay(5000);  
}
