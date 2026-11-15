#include <SoftwareSerial.h>
SoftwareSerial LORA(3, 2); // RX, TX

String receivedMessage;
String Data;
String data;
String Nodee_id = "";
String valve_id = "";
String MasterID = "3002852";
String Sub1;
String Sub2;
const int SOL_1 = 5;
const int SOL_2 = 6;
int state = 0;
int flag_1 = 0;
int COUNT = 0;

void setup() {
  pinMode(SOL_1, OUTPUT);
  pinMode(SOL_2, OUTPUT);
  digitalWrite(SOL_1, LOW);
  digitalWrite(SOL_2, LOW);

  Serial.begin(9600);
  LORA.begin(9600);
  delay(100);

  sendAT("AT");
  sendAT("AT+MODE=TEST");
  sendAT("AT+TEST=RXLRPKT");

  Serial.println("LoRa Receiver Ready (Slave_1)");
}

void loop() {
  if (LORA.available()) {
    delay(50); // ensure complete message is received
    receivedMessage = LORA.readStringUntil('\n');
    receivedMessage.trim();

    Serial.println("Raw Received: " + receivedMessage); // log full line

    int stIndex = receivedMessage.indexOf('"');
    if (stIndex != -1) {
      String hexData;
      int eIndex = receivedMessage.indexOf('"', stIndex + 1);

      if (eIndex != -1) {
        hexData = receivedMessage.substring(stIndex + 1, eIndex);
      } else {
        hexData = receivedMessage.substring(stIndex + 1);
        Serial.println("Warning: Closing quote missing. Attempting recovery.");
      }

      hexData.trim();

      Data = decodeHex(hexData);
      Data.trim();

      Serial.println("Decoded message: " + Data);

      if (Data.length() == 0) {
        Serial.println("Data was empty after decoding");
        return;
      }

      if (Data.startsWith("010")) {
        state = 1;
        Serial.println("Command: ON");
      } else if (Data.startsWith("000")) {
        state = 0;
        Serial.println("Command: OFF");
      } else if (Data.startsWith("@@*TTOTFF")) {
        Total_off();
        return;
      }

      Data = Data.substring(4);  // remove "010*" or "000*"
      parseReceivedMessage(Data);
      Data = "";
    } else {
      Serial.println("Invalid message format or missing quotes");
    }
  }

  Sub1 = "";
  Sub2 = "";
}

void parseReceivedMessage(String msg) {
  Serial.print("Raw msg in parseReceivedMessage: ");
  Serial.println(msg);

  if (msg.startsWith("1002")) {
    if (msg.length() > 13) {
      int starIndex = msg.indexOf('*');
      if (starIndex != -1) {
        String part1 = msg.substring(0, starIndex);
        String part2 = msg.substring(starIndex + 1);

        Serial.print("part1 >> ");
        Serial.println(part1);
        Serial.print("part2 >> ");
        Serial.println(part2);

        if (part1.indexOf('.') != -1) {
          Serial.println("part1 has dot");
          flag_1 = 1;
          handleWithDot(part1);
        } else {
          Serial.println("part1 no dot");
          flag_1 = 0;
          on_off(part1);
        }

        if (part2.indexOf('.') != -1) {
          Serial.println("part2 has dot");
          flag_1 = 1;
          handleWithDot(part2);
        } else {
          Serial.println("part2 no dot");
          flag_1 = 0;
          on_off(part2);
        }
      }
    } else {
      if (msg.indexOf('.') != -1) {
        flag_1 = 1;
        handleWithDot(msg);
      } else {
        flag_1 = 0;
        on_off(msg);
      }
    }
  } else {
    Serial.println("Unrecognized data prefix.");
  }
}

void handleWithDot(String Data) {
  int dotIndex = Data.indexOf('.');
  if (dotIndex != -1) {
    Sub1 = Data.substring(0, dotIndex);
    Sub2 = Data.substring(dotIndex + 1);
    on_off(Sub1);
    Serial.print("Before dot: ");
    Serial.println(Sub1);
    Serial.print("After dot: ");
    Serial.println(Sub2);
  }
}

void on_off(String valve) {
  Serial.print("Valve >> ");
  Serial.println(valve);
  Nodee_id = valve;

  if (valve == "100288001") {
    valve_id = "01";
    if (state == 1) {
      COUNT++;
      digitalWrite(SOL_1, HIGH);
      delay(1000);
      Serial.println("Valve 1 ON");
      ack(valve);
    } else {
      COUNT--;
      digitalWrite(SOL_1, LOW);
      delay(1000);
      Serial.println("Valve 1 OFF");
      ack(valve);
    }
  } else if (valve == "100288002") {
    valve_id = "02";
    if (state == 1) {
      COUNT++;
      digitalWrite(SOL_2, HIGH);
      delay(1000);
      Serial.println("Valve 2 ON");
      ack(valve);
    } else {
      COUNT--;
      digitalWrite(SOL_2, LOW);
      delay(1000);
      Serial.println("Valve 2 OFF");
      ack(valve);
    }
  } else {
    Serial.println("Unknown valve ID");
  }
}

void ack(String Resender) {
  delay(5000);
  String Symbol = "";
  String STS = "";
  String cond = "";

  if (flag_1 == 1) {
    Symbol = "$&$";
    cond = "*" + Sub2;
  } else {
    Symbol = "%$%";
    cond = "";
  }

  if (state == 1) {
    STS = "010";
  } else {
    STS = "000";
  }

  if (COUNT < 1 || COUNT > 2) {
    COUNT = 0;
  }

  String lorxack = String(COUNT) + "*##*" + Symbol + cond + "*" + MasterID + "*" + STS + "*" + Nodee_id;
  Serial.println("ACK string: " + lorxack);
  lorxack = encodeToHex(lorxack);
  Lora_Tx(lorxack);
  delay(1000);
}

void Total_off() {
  digitalWrite(SOL_1, LOW);
  delay(1000);
  digitalWrite(SOL_2, LOW);
  delay(1000);
  Serial.println("Total OFF command received. All valves OFF.");
}

void Lora_Tx(String message) {
  Serial.println("LORA_Tx >> " + message);
  delay(1000);
  sendAT("AT+TEST=TXLRPKT," + message);
  delay(1000);
  sendAT("AT+TEST=RXLRPKT");
  delay(1000);
  LORA.flush();
}

void sendAT(String cmd) {
  LORA.println(cmd);
  delay(1000);
}

String decodeHex(String hexString) {
  String result = "";
  for (int i = 0; i < hexString.length(); i += 2) {
    if (i + 1 >= hexString.length()) break;
    String hexByte = hexString.substring(i, i + 2);
    char c = (char)strtol(hexByte.c_str(), NULL, 16);
    if (isPrintable(c)) result += c;
  }
  return result;
}

String encodeToHex(String str) {
  String hexString = "";
  for (int i = 0; i < str.length(); i++) {
    if ((uint8_t)str[i] < 16) hexString += "0";
    hexString += String((uint8_t)str[i], HEX);
  }
  return hexString;
}
