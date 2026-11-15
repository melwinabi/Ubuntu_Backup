#include <SoftwareSerial.h>

SoftwareSerial mySerial(4, 6); // RX, TX

#define RS485CommControl 5
#define RS485Transmit    HIGH
#define RS485Receive     LOW

float volt_r, volt_y, volt_b, current_r, current_y, current_b, total_kw, pf, frq, rn_hr, total_kwh;
int for_operation = 0, index;
uint8_t response[212];

void setup() 
{
  Serial.begin(9600);
  mySerial.begin(9600);
  pinMode(RS485CommControl, OUTPUT);
  digitalWrite(RS485CommControl, RS485Receive);
  Serial.println("Started");
  sendEMRequest();
}

void loop() 
{ 
  if (mySerial.available()) 
  {
    index = 0;
    while (mySerial.available() && index < 212) 
    {
      response[index] = mySerial.read();
      Serial.print(response[index], HEX);
      Serial.print(" ");
      index++;
    }
    Serial.println(" ");
    Serial.print("INDEX : ");
    Serial.println(index);
    delay(50);
    // if(for_operation == 0)
    // {

    // }
    // else if (for_operation == 1) // Energy Meter Reading
    // {
    //   EMReading();
    //   delay(3000);
    //   sendEMRequest();
    // }
    
    
  }
  //sendFMRequest();
  sendEMRequest();
  delay(1000);
}
void EMReading()
{
  if (index >= 195) 
    {
      volt_r = convertHexToFloat(response[5], response[6], response[3], response[4]);
      volt_y = convertHexToFloat(response[9], response[10], response[7], response[8]);
      volt_b = convertHexToFloat(response[13], response[14], response[11], response[12]);

      current_r = convertHexToFloat(response[37], response[38], response[35], response[36]);
      current_y = convertHexToFloat(response[41], response[42], response[39], response[40]);
      current_b = convertHexToFloat(response[45], response[46], response[43], response[44]);

      total_kw = convertHexToFloat(response[89], response[90], response[87], response[88]);

      pf = convertHexToFloat(response[113], response[114], response[111], response[112]);

      frq = convertHexToFloat(response[117], response[118], response[115], response[116]);

      rn_hr = convertHexToFloat(response[157], response[158], response[155], response[156]);

      total_kwh = convertHexToFloat(response[189], response[190], response[187], response[188]);

      Serial.print("PHASE R (V): ");
      Serial.println(volt_r, 2);
      Serial.print("PHASE Y (V): ");
      Serial.println(volt_y, 2);
      Serial.print("PHASE B (V): ");
      Serial.println(volt_b, 2);

      Serial.print("PHASE R (I): ");
      Serial.println(current_r, 2);
      Serial.print("PHASE Y (I): ");
      Serial.println(current_y, 2);
      Serial.print("PHASE B (I): ");
      Serial.println(current_b, 2);

      Serial.print("TOTAL KW: ");
      Serial.println(total_kw, 2);
      Serial.print("PF: ");
      Serial.println(pf, 2);
      Serial.print("FREQUENCY: ");
      Serial.println(frq, 2);
      Serial.print("RUN HR: ");
      Serial.println(rn_hr, 2);
      Serial.print("TOTAL KWH: ");
      Serial.println(total_kwh, 2);
    }
    else 
    {
      Serial.println("Insufficient data received.");
    }
}

void sendFMRequest() 
{
  for_operation = 0;
  Serial.println("FM Request Sent");
  digitalWrite(RS485CommControl, RS485Transmit);
  delay(10);
  uint8_t request[] = {0x02, 0x03, 0x00, 0x00, 0x00, 0x04, 0x44, 0x3A};
  mySerial.write(request, sizeof(request));
  delay(10);
  digitalWrite(RS485CommControl, RS485Receive);
  delay(10);
}

void sendEMRequest() 
{
  for_operation = 1;
  Serial.println("EM Request Sent");
  digitalWrite(RS485CommControl, RS485Transmit);
  delay(10);

  uint8_t request[] = {0x01, 0x04, 0x00, 0x00, 0x00, 0x7D, 0x30, 0x2B};
  mySerial.write(request, sizeof(request));
  delay(10);
  digitalWrite(RS485CommControl, RS485Receive);
  delay(10);
}



float convertHexToFloat(uint8_t byte1, uint8_t byte2, uint8_t byte3, uint8_t byte4) 
{
  uint32_t combined = ((uint32_t)byte1 << 24) | ((uint32_t)byte2 << 16) | ((uint32_t)byte3 << 8) | byte4;
  union 
  {
    uint32_t i;
    float f;
  } data;
  data.i = combined;
  if (isfinite(data.f)) 
  {
    return data.f;
  } else 
  {
    Serial.println("Float value is invalid (ovf).");
    return NAN;
  }
}
