String AVST;
String data;
String Master_ID = "3002853";
String Return = "";

unsigned long previousMillis = 0;
const unsigned long interval = 30000;  // 30 seconds

String symbol, status, ID, version;
String Data = "";

void setup() {
  Serial.begin(9600);
  Serial1.begin(9600);

  pinMode(29, OUTPUT);
  pinMode(31, OUTPUT);

  digitalWrite(29, LOW);
  digitalWrite(31, LOW);
}

void loop() {

  unsigned long currentMillis = millis();
  // Send message every 60 seconds
  if (currentMillis - previousMillis >= 10000) {
    previousMillis = currentMillis;
    AVST = "##*AVL*" + Master_ID + "*010";
    Serial1.println(AVST);
    Serial.println("Sent: " + AVST);
  }

  // Read GSM data
  if (Serial1.available() > 0) {
    Data = Serial1.readString();
    Data.trim();
    Serial.println("Received: " + Data);
    Data_Processes(Data);
    Data = "";
  }
}


//===================== DATA PARSER ======================//

void Data_Processes(String input) {

  // ---------------- CASE 1 ----------------
  if (input.startsWith("@@*$&$*")) {
    Serial1.println("##*3002853*C@K");
    delay(1000);
    int p1 = input.indexOf('*');
    int p2 = input.indexOf('*', p1 + 1);
    int p3 = input.indexOf('*', p2 + 1);
    int p4 = input.indexOf('*', p3 + 1);

    symbol = input.substring(p1 + 1, p2);
    String deviceID = input.substring(p2 + 1, p3);
    status = input.substring(p3 + 1, p4);

    // Part after p4 contains: 100288001.1
    String lastPart = input.substring(p4 + 1);
    int dot = lastPart.indexOf('.');

    ID = lastPart.substring(0, dot);
    version = lastPart.substring(dot + 1);

    // Print
    Serial.println("symbol = " + symbol);
    Serial.println("status = " + status);
    Serial.println("tableID = " + ID);
    Serial.println("version = " + version);

    if (status == "000") {
      Serial.println("off");
      digitalWrite(31, LOW);
      delay(2000);
      Return = "##*" + symbol + "*" + version + "*" + Master_ID + "*" + status + "*" + ID;
      Serial.println(Return);
      delay(1000);
      Serial1.println(Return);
      delay(1000);
      Serial1.println("##*3002853*2002853*STS*000");
      delay(1000);
      AVST = "##*AVL*" + Master_ID + "*010";
      Serial1.println(AVST); 
      delay(1000);     
    }

    else if (status == "010") {
      Serial.println("on");
      digitalWrite(31, HIGH);
      delay(2000);
      Return = "##*" + symbol + "*" + version + "*" + Master_ID + "*" + status + "*" + ID;
      Serial.println(Return);
      delay(1000);
      Serial1.println(Return);
      delay(1000);
      Serial1.println("##*3002853*2002853*STS*010");
      delay(1000);
      AVST = "##*AVL*" + Master_ID + "*010";
      Serial1.println(AVST);  
      delay(1000);    
    }
  }

  // ---------------- CASE 2 ----------------
  else if (input.startsWith("@@*%$%*3002853*010")) {
    Serial1.println("##*3002853*C@K");
    delay(1000);
    Serial.println("on");
    digitalWrite(31, HIGH);
    delay(2000);
    Serial.println("##*%$%*3002853*010*100288201");
    delay(1000);
    Serial1.println("##*%$%*3002853*010*100288201");
    delay(1000);
    Serial1.println("##*3002853*2002853*STS*010");
    delay(1000);
    AVST = "##*AVL*" + Master_ID + "*010";
    Serial1.println(AVST);
    delay(1000);
  }

  // ---------------- CASE 3 ----------------
  else if (input.startsWith("@@*%$%*3002853*000")) {
    Serial1.println("##*3002853*C@K");
    delay(1000);
    Serial.println("off");
    digitalWrite(31, LOW);
    delay(2000);
    Serial.println("##*%$%*3002853*000*100288201");
    delay(1000);
    Serial1.println("##*%$%*3002853*000*100288201");
    delay(1000);
    Serial1.println("##*3002853*2002853*STS*000");
    delay(1000);
    AVST = "##*AVL*" + Master_ID + "*010";
    Serial1.println(AVST); 
    delay(1000);
  }

  // ---------------- CASE 4 ----------------
  else if (input.startsWith("&&*!!!*")) {
    Serial1.println("##*3002853*C@K");
    delay(1000);
    Serial.println("off");
    digitalWrite(31, LOW);
    delay(000);
    Serial1.println("##*3002853*2002853*STS*000");
    delay(1000);
    AVST = "##*AVL*" + Master_ID + "*010";
    Serial1.println(AVST);
    delay(1000);    
  }
}

// const char* WIFI_SSID = "Melwin";
// const char* WIFI_PASS = "Melwin@123";

// void sendMasterInfo() {
//   String msg =
//     "##*WMfO*" + String(Master_ID) +
//     "*404040968066519*8991009020984748682F*IND airtel*404*40*5*42139*180844554*7*00.0000000*11.1111111*0";

//   mqttClient.publish(Publish_Master.c_str(), msg.c_str(), false);
// }