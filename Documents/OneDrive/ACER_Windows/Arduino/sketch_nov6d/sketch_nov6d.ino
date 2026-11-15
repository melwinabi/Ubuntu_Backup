#include <SoftwareSerial.h>
#include <Arduino.h>

SoftwareSerial rs485(2,3); // RX, TX

#define RS485CommControl 4
#define RS485Transmit HIGH
#define RS485Receive LOW

uint8_t response[125];  // Combined response buffer for EM data
int indexEM = 0;

unsigned long previousMillis_three = 0;
const unsigned long interval_three = 6000;//5mnts

unsigned long previousMillis_two = 0;
const unsigned long interval_two = 5000;

float Average_Current = 0;
float Average_volt = 0;
float Average_Pf = 0;
float KW  = 0, KWh = 0;
//..................................................................................................
String deviceID = "10001";  // 
//..................................................................................................

void setup() {
  Serial.begin(115200);
  rs485.begin(9600);
  pinMode(RS485CommControl, OUTPUT);
  digitalWrite(RS485CommControl, RS485Receive);
  delay(100);
}

void loop() {
  unsigned long currentMillis_second = millis();

  if (currentMillis_second - previousMillis_three >= interval_three) {
    previousMillis_three = currentMillis_second;
    sendEMRequest();
    Serial.println(Send_String());
  }

  if (currentMillis_second - previousMillis_two >= interval_two) {
    previousMillis_two = currentMillis_second;
    sendEMRequest();
  }

  if (rs485.available()) {
    indexEM = 0;
    while (rs485.available() && indexEM < 60) {
      response[indexEM] = rs485.read();
      //Serial.println(response[indexEM]);
      indexEM++;
    }
    delay(50);
    EMReading();
  }

  delay(200);
}

void sendEMRequest() {
  digitalWrite(RS485CommControl, RS485Transmit);
  delay(10);

  uint8_t request[] = {0x01, 0x04, 0x00, 0x00, 0x00, 0x18, 0xF0, 0x00};
  rs485.write(request, sizeof(request));

  delay(10);
  digitalWrite(RS485CommControl, RS485Receive);
  delay(10);
 // Serial.println("Request Send");
}

void EMReading() {
  if (indexEM > 50) {

    Average_volt = convertHexToFloat(response[33], response[34], response[31], response[32]);
    Average_Current = convertHexToFloat(response[49], response[50], response[47], response[48]);

    Average_Pf  = convertHexToFloat(response[113], response[114], response[111], response[112]);

    KW = convertHexToFloat(response[89], response[90], response[87], response[88]);
    KWh = convertHexToFloat(response[121], response[122], response[119], response[120]);

    Serial.print(" ##*Average_volt (V): ");
    Serial.println(Average_volt, 2);
    Serial.print(" ##*Average_current (I): ");
    Serial.println(Average_Current, 2);
    Serial.print(" ##*Average_Pf : ");
    Serial.println(Average_Pf, 2);
    Serial.print(" ##*KW : ");
    Serial.println(KW, 2);
    Serial.print(" ##*KWh  : ");
    Serial.println(KWh, 2);
   // Serial.println("##*EM Reading Finished");
  } 
  else {
    //Serial.println("Insufficient data received.");
  }
}

float convertHexToFloat(uint8_t byte1, uint8_t byte2, uint8_t byte3, uint8_t byte4) 
{
  uint32_t combined = ((uint32_t)byte1 << 24) | ((uint32_t)byte2 << 16) |
                      ((uint32_t)byte3 << 8) | byte4;
  union {
    uint32_t i;
    float f;
  } data;
  data.i = combined;
  if (isfinite(data.f)) {
    return data.f;
  } else {
    return NAN;
  }
}

String Send_String() 
{
  String url = "https://api.smartpruner.in/api/DataInsert/SaveDataInTable?SerialNo=" + String(deviceID) + "&values=";
  url += "V1=" + String(Average_volt, 2);
  // url += "~V2=" + String(Average_Current, 2);
  // url += "~V3=" + String(Average_Pf, 2);
  // url += "~V4=" + String(KW, 2);
  // url += "~V5=" + String(KWh, 2);  
  return url;
}


// String Send_String()
// {
//   String url = "http://sili.ppvserver.com/api/Siligreen/Data?deviceid=107";

//   url += "&v1="  + String(Average_volt, 2);
//   url += "&v2="  + String(Average_Current, 2);
//   url += "&v3=" + String(Average_Pf, 2);
//   url += "&v4=" + String(KW, 2);
//   url += "&v5=" + String(KWh, 2);  
//   return url;
//   //Serial.println(url);
// }


//https://api.smartpruner.in/api/DataInsert/SaveDataInTable?SerialNo=10001&values=V1=65.83~V2=65.83~V3=65.83~V4=65.83~V5=65.83 
//https://api.smartpruner.in/api/DataInsert/SaveDataInTable?SerialNo=10001&values=V1=0.00~V2=0.00~V3=0.00~V4=0.00~V5=0.00