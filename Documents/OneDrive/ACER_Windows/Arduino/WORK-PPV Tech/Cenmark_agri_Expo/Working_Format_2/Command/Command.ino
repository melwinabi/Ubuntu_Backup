#include <EEPROM.h>

String ReceivedMessage;
int c = 0;
char rd_1[11], rd_2[11], rd_3[11];
String Address_1, Address_2, Address_3;
String addresses[3];

void setup() {
  Serial.begin(9600);
  Serial.println("Address Loading ...");
  LoadData(); 
  
  // Debugging: Print loaded addresses
  Serial.print("Address_1: ");
  Serial.println(Address_1);
  Serial.print("Address_2: ");
  Serial.println(Address_2);
  Serial.print("Address_3: ");
  Serial.println(Address_3);

  addresses[0] = Address_1;
  addresses[1] = Address_2;
  addresses[2] = Address_3;
  
  // Debugging: Print stored addresses array
  for (int i = 0; i < 3; i++) {
    Serial.print("addresses[");
    Serial.print(i);
    Serial.print("]: ");
    Serial.println(addresses[i]);
  }
}

void loop() {
  if (Serial.available() > 0) {
    ReceivedMessage = Serial.readStringUntil(';');
    c = 1;
  } else {
    if (c == 1) {
      Serial.println(ReceivedMessage);
      //Serial.println(ReceivedMessage[0]);
      if (ReceivedMessage[0] == '@') {
        int startIndex = ReceivedMessage.indexOf("@") + 1;
        int endIndex = ReceivedMessage.lastIndexOf(";");
        String message = ReceivedMessage.substring(startIndex, endIndex);
        Serial.print("This is DATA: ");
        Serial.println(message);
        Serial.print("ON/OFF Value: ");
        Serial.println(message[11]);
        String CutMsg = message.substring(0, 10);
        Serial.print("ID: ");
        Serial.println(CutMsg);

        // Compare the cut message with stored addresses
        for (int i = 0; i < 3; i++) {
          if (addresses[i] == CutMsg) {
            Serial.print("Address ");
            Serial.print(i + 1);
            Serial.print(" Called and the Status is: ");
            if (message[11] == '1') {
              Serial.println("ON");
            }else if(message[11] == '0'){
              Serial.println("OFF");
            }else if(message[11] == '?'){
               Serial.println("Reading Velve: "+CutMsg);
            }else {
               Serial.println("Data Invalid");
            }
            break;
          }
        }
      } else if (ReceivedMessage[0] == '#') {
        int startIndex = ReceivedMessage.indexOf("#") + 1;
        int endIndex = ReceivedMessage.lastIndexOf(";");
        String message = ReceivedMessage.substring(startIndex, endIndex);
        Serial.print("This is Command: ");
        Serial.print(message);
      }
      c = 0;
    }
  }
}

void LoadData() {
  for (int j = 0; j < 10; j++) {
    rd_1[j] = EEPROM.read(j);
  }
  rd_1[10] = '\0';
  Address_1 = String(rd_1);
  
  for (int h = 0; h < 10; h++) {
    rd_2[h] = EEPROM.read(h + 10);
  }
  rd_2[10] = '\0';
  Address_2 = String(rd_2);
  
  for (int o = 0; o < 10; o++) {
    rd_3[o] = EEPROM.read(o + 20);
  }
  rd_3[10] = '\0';
  Address_3 = String(rd_3);

  Serial.print("rd_1: ");
  Serial.println(rd_1);
  Serial.print("rd_2: ");
  Serial.println(rd_2);
  Serial.print("rd_3: ");
  Serial.println(rd_3);
}

//#0000000000111111111122222222223333333333444444444455555555556666666666999999999988888888887777777777;
//@3333333333+
