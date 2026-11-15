#include <SoftwareSerial.h>

SoftwareSerial rs485(6, 4); // RX, TX

#define RS485CommControl 5
#define RS485Transmit    HIGH
#define RS485Receive     LOW

uint8_t responseFM[50];  // Buffer for Modbus responseFM
uint8_t responseEM[60];
int indexFM;
int indexEM;
int RequestFM = 0;
int RequestEM = 0;
int ReqRecEM = 0;
int ReqRecFM = 0;
int i = 0 ,j = 0;
unsigned long previousMillis = 0;  
const unsigned long interval = 5000; 

float totaliser_1, flow_1, totaliser_2, flow_2;
float volt_r, volt_y, volt_b, current_r, current_y, current_b, total_kw, pf, frq, rn_hr, total_kwh;


void setup() {
  Serial.begin(9600);
  rs485.begin(9600);
  pinMode(RS485CommControl, OUTPUT);
  digitalWrite(RS485CommControl, RS485Receive);
  pinMode(A1, OUTPUT);  // PC1
  pinMode(A2, OUTPUT);  // PC2
  Serial.println("Started");
}

void loop() {
  unsigned long currentMillis = millis();
  if (currentMillis - previousMillis >= interval) {// t0 30
    previousMillis = currentMillis; 
    if(ReqRecFM == 0)
    {
      i++;
      if( i > 5)
      {
        Serial.println("Send em failed");
        ReqRecFM = 1;
        i = 0;
      }
      RequestFM = 0;
      sendEMRequest();
      Serial.println("Send em");
    }
    else 
    {
      j++;
      if( j > 5)
      {
        Serial.println("Send fm failed");
        ReqRecFM = 0;
        j = 0;
      }
      RequestFM = 1;
      sendFMRequest();
      delay(3000);
      Serial.println("Send fm");
    }
  }
  if (rs485.available()) {
    if(RequestFM == 1)
    {
      indexFM = 0;
      while (rs485.available() && indexFM < 50) {
        responseFM[indexFM] = rs485.read();
        indexFM++;
      }
      delay(50);
      decodeFT650();
    }
    else {
    {  
      indexEM = 0;
      while (rs485.available() && indexEM < 60) 
      {
        responseFM[indexEM] = rs485.read();
        Serial.print(responseFM[indexEM], HEX);
        Serial.print(" ");
        indexEM++;
    }
    delay(50);
    EMReading();
    }
    }
  }
}

void sendFMRequest() {
  //Serial.println("FM Request Sent");
  digitalWrite(RS485CommControl, RS485Transmit);
  delay(10);
  uint8_t request[] = { 0x01, 0x03, 0x00, 0x00, 0x00, 0x14, 0x45, 0xC5 }; // Read 20 registers
  rs485.write(request, sizeof(request));
  delay(10);
  digitalWrite(RS485CommControl, RS485Receive);
  delay(10);
}

uint32_t combineRegisters(uint16_t msw, uint16_t lsw) {
  return ((uint32_t)msw << 16) | lsw;
}

void decodeFT650() {
  if (indexFM >= 45 && responseFM[1] == 0x03) 
  {
    // Read registers from indexFM 3 onward (3 = byte count start)
    uint16_t total2_lsw   = (responseFM[3+28] << 8) | responseFM[3+29]; // 40017
    uint16_t total2_msw   = (responseFM[3+30] << 8) | responseFM[3+31]; // 40018
    uint16_t flow1_lsw    = (responseFM[3+24] << 8) | responseFM[3+25]; // 40015
    uint16_t flow1_msw    = (responseFM[3+26] << 8) | responseFM[3+27]; // 40016

    flow_1      = combineRegisters(flow1_msw, flow1_lsw) * 0.01;
    totaliser_2 = combineRegisters(total2_msw, total2_lsw) * 0.01;

    Serial.print("Totaliser 1: "); Serial.println(totaliser_2, 2);
    Serial.print("Flow 1:      "); Serial.println(flow_1, 2);
    Serial.println("");
    ReqRecFM = 0;
  } 
  else 
  {
    flow_1      = 0;
    totaliser_2 = 0;
  }
}


void sendEMRequest() 
{
  Serial.println("EM Request Sent");
  digitalWrite(RS485CommControl, RS485Transmit);
  delay(10);

  uint8_t request[] =  {0x01 ,0x04 ,0x00 ,0x00 ,0x00 ,0x18 ,0xF0 ,0x00}; 
  rs485.write(request, sizeof(request));
  delay(10);
  digitalWrite(RS485CommControl, RS485Receive);
  delay(10);
}

void EMReading()
{
  if (indexEM >= 50) 
    {

  float val_0x00 = convertHexToFloat(responseEM[5], responseEM[6], responseEM[3], responseEM[4]);
  float val_0x02 = convertHexToFloat(responseEM[9], responseEM[10], responseEM[7], responseEM[8]);
  float val_0x04 = convertHexToFloat(responseEM[13], responseEM[14], responseEM[11], responseEM[12]);
  float val_0x06 = convertHexToFloat(responseEM[17], responseEM[18], responseEM[15], responseEM[16]);
  float val_0x08 = convertHexToFloat(responseEM[21], responseEM[22], responseEM[19], responseEM[20]);
  float val_0x0A = convertHexToFloat(responseEM[25], responseEM[26], responseEM[23], responseEM[24]);
  float val_0x0C = convertHexToFloat(responseEM[29], responseEM[30], responseEM[27], responseEM[28]);
  float val_0x0E = convertHexToFloat(responseEM[33], responseEM[34], responseEM[31], responseEM[32]);
  float val_0x10 = convertHexToFloat(responseEM[37], responseEM[38], responseEM[35], responseEM[36]);
  float val_0x12 = convertHexToFloat(responseEM[41], responseEM[42], responseEM[39], responseEM[40]);
  float val_0x14 = convertHexToFloat(responseEM[45], responseEM[46], responseEM[43], responseEM[44]);
  float val_0x16 = convertHexToFloat(responseEM[49], responseEM[50], responseEM[47], responseEM[48]);
  float val_0x18 = convertHexToFloat(responseEM[53], responseEM[54], responseEM[51], responseEM[52]);
  float val_0x1A = convertHexToFloat(responseEM[57], responseEM[58], responseEM[55], responseEM[56]);
  float val_0x1C = convertHexToFloat(responseEM[61], responseEM[62], responseEM[59], responseEM[60]);
  float val_0x1E = convertHexToFloat(responseEM[65], responseEM[66], responseEM[63], responseEM[64]);
  float val_0x20 = convertHexToFloat(responseEM[69], responseEM[70], responseEM[67], responseEM[68]);
  float val_0x22 = convertHexToFloat(responseEM[73], responseEM[74], responseEM[71], responseEM[72]);
  float val_0x24 = convertHexToFloat(responseEM[77], responseEM[78], responseEM[75], responseEM[76]);
  float val_0x26 = convertHexToFloat(responseEM[81], responseEM[82], responseEM[79], responseEM[80]);
  float val_0x28 = convertHexToFloat(responseEM[85], responseEM[86], responseEM[83], responseEM[84]);
  float val_0x2A = convertHexToFloat(responseEM[89], responseEM[90], responseEM[87], responseEM[88]);
  float val_0x2C = convertHexToFloat(responseEM[93], responseEM[94], responseEM[91], responseEM[92]);
  float val_0x2E = convertHexToFloat(responseEM[97], responseEM[98], responseEM[95], responseEM[96]);
  float val_0x30 = convertHexToFloat(responseEM[101], responseEM[102], responseEM[99], responseEM[100]);
  float val_0x32 = convertHexToFloat(responseEM[105], responseEM[106], responseEM[103], responseEM[104]);
  float val_0x34 = convertHexToFloat(responseEM[109], responseEM[110], responseEM[107], responseEM[108]);
  float val_0x36 = convertHexToFloat(responseEM[113], responseEM[114], responseEM[111], responseEM[112]);
  float val_0x38 = convertHexToFloat(responseEM[117], responseEM[118], responseEM[115], responseEM[116]);
  float val_0x3A = convertHexToFloat(responseEM[121], responseEM[122], responseEM[119], responseEM[120]);
  float val_0x3C = convertHexToFloat(responseEM[125], responseEM[126], responseEM[123], responseEM[124]);
  float val_0x3E = convertHexToFloat(responseEM[129], responseEM[130], responseEM[127], responseEM[128]);
  float val_0x40 = convertHexToFloat(responseEM[133], responseEM[134], responseEM[131], responseEM[132]);

  // Print all values
  Serial.println("Register Values:");
  Serial.print("0x00: "); Serial.println(val_0x00);
  Serial.print("0x02: "); Serial.println(val_0x02);
  Serial.print("0x04: "); Serial.println(val_0x04);
  Serial.print("0x06: "); Serial.println(val_0x06);
  Serial.print("0x08: "); Serial.println(val_0x08);
  Serial.print("0x0A: "); Serial.println(val_0x0A);
  Serial.print("0x0C: "); Serial.println(val_0x0C);
  Serial.print("0x0E: "); Serial.println(val_0x0E);
  Serial.print("0x10: "); Serial.println(val_0x10);
  Serial.print("0x12: "); Serial.println(val_0x12);
  Serial.print("0x14: "); Serial.println(val_0x14);
  Serial.print("0x16: "); Serial.println(val_0x16);
  Serial.print("0x18: "); Serial.println(val_0x18);
  Serial.print("0x1A: "); Serial.println(val_0x1A);
  Serial.print("0x1C: "); Serial.println(val_0x1C);
  Serial.print("0x1E: "); Serial.println(val_0x1E);
  Serial.print("0x20: "); Serial.println(val_0x20);
  Serial.print("0x22: "); Serial.println(val_0x22);
  Serial.print("0x24: "); Serial.println(val_0x24);
  Serial.print("0x26: "); Serial.println(val_0x26);
  Serial.print("0x28: "); Serial.println(val_0x28);
  Serial.print("0x2A: "); Serial.println(val_0x2A);
  Serial.print("0x2C: "); Serial.println(val_0x2C);
  Serial.print("0x2E: "); Serial.println(val_0x2E);
  Serial.print("0x30: "); Serial.println(val_0x30);
  Serial.print("0x32: "); Serial.println(val_0x32);
  Serial.print("0x34: "); Serial.println(val_0x34);
  Serial.print("0x36: "); Serial.println(val_0x36);
  Serial.print("0x38: "); Serial.println(val_0x38);
  Serial.print("0x3A: "); Serial.println(val_0x3A);
  Serial.print("0x3C: "); Serial.println(val_0x3C);
  Serial.print("0x3E: "); Serial.println(val_0x3E);
  Serial.print("0x40: "); Serial.println(val_0x40);

  delay(2000); // wait before reading agai

      ReqRecFM = 1;
    }
    else 
    {
      Serial.println("Insufficient data received.");
    }
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