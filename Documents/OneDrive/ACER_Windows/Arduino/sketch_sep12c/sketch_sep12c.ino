#include <SoftwareSerial.h>

SoftwareSerial rs485(6, 4); // RX, TX

#define RS485CommControl 5
#define RS485Transmit    HIGH
#define RS485Receive     LOW

float volt_r, volt_y, volt_b, current_r, current_y, current_b, total_kw, pf, frq, rn_hr, total_kwh;
int for_operation = 0, indexEM;
uint8_t responseFM[60];

void setup() 
{
  Serial.begin(9600);
  rs485.begin(9600);
  pinMode(RS485CommControl, OUTPUT);
  digitalWrite(RS485CommControl, RS485Receive);
  Serial.println("Started");
  sendEMRequest();
}

void loop() 
{ 
  if (rs485.available()) 
  {
    indexEM = 0;
    while (rs485.available() && indexEM < 60) 
    {
      responseFM[indexEM] = rs485.read();
      Serial.print(responseFM[indexEM], HEX);
      Serial.print(" ");
      indexEM++;
    }
    EMReading();
    Serial.println(" ");
    Serial.print("indexEM : ");
    Serial.println(indexEM);
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
  if (indexEM >= 50) 
    {
      volt_r = convertHexToFloat(responseFM[5], responseFM[6], responseFM[3], responseFM[4]);
      volt_y = convertHexToFloat(responseFM[9], responseFM[10], responseFM[7], responseFM[8]);
      volt_b = convertHexToFloat(responseFM[13], responseFM[14], responseFM[11], responseFM[12]);

      current_r = convertHexToFloat(responseFM[33], responseFM[34], responseFM[35], responseFM[36]);
      current_y = convertHexToFloat(responseFM[37], responseFM[38], responseFM[39], responseFM[40]);
      current_b = convertHexToFloat(responseFM[41], responseFM[42], responseFM[43], responseFM[44]);

      total_kw = convertHexToFloat(responseFM[89], responseFM[90], responseFM[87], responseFM[88]);

      pf = convertHexToFloat(responseFM[113], responseFM[114], responseFM[111], responseFM[112]);

      frq = convertHexToFloat(responseFM[117], responseFM[118], responseFM[115], responseFM[116]);

      rn_hr = convertHexToFloat(responseFM[157], responseFM[158], responseFM[155], responseFM[156]);

      total_kwh = convertHexToFloat(responseFM[189], responseFM[190], responseFM[187], responseFM[188]);
      Serial.println("\r\n");
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
  uint8_t request[] = {0x01, 0x04, 0x00, 0x00, 0x00, 0x7D, 0x30, 0x2B};
  rs485.write(request, sizeof(request));
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

  uint8_t request[] = {0x01 ,0x04 ,0x00 ,0x00 ,0x00 ,0x18 ,0xF0 ,0x00}; 
  rs485.write(request, sizeof(request));
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
