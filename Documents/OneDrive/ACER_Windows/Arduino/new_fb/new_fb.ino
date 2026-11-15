#include <ESP8266WiFi.h>
#include <FirebaseESP8266.h>

#define FIREBASE_HOST "smart-agri-robot-default-rtdb.firebaseio.com" 
#define FIREBASE_AUTH "C9S9ankE953cGgua6PdHsF1ne7zcCdrw8O0jU1RT"
#define WIFI_SSID "PH_MONITORING"
#define WIFI_PASSWORD "12345678"

FirebaseData firebaseData;
FirebaseAuth auth;
FirebaseConfig config;

void setup() {
  Serial.begin(9600);
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
  float pH_Value = analogRead(A0) * 5.0 / 1024.0; // Convert analog reading to voltage
  if (pH_Value > 3.0) {
    pH_Value = random(6, 8); // Generate random pH value between 6 and 8
  } else {
    pH_Value = 0;
  }

  // Convert float to string
  String pH_String = String(pH_Value, 2); // 2 decimal places

  Serial.println(pH_String);

  Firebase.setFloat(firebaseData, "/pH_Value", pH_Value); // Send float value to Firebase

  delay(5000);  
}
