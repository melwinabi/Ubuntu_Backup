#include <SoftwareSerial.h>
#include <Arduino.h>

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

const int muxs0 = 7;
const int muxs1 = 8;
const int muxs2 = 9;
const int muxs3 = 10;
const int muxen = 11; // MUX ENABLE
const int CTmonitor = A0;
const int samples = 200;
const int ADC_RESOLUTION = 1023; // use 1023 for Arduino UNO ADC
const int Treshold = 1;

unsigned long previousMillis = 0;  
const unsigned long interval = 6000; 

unsigned long previousMillis_two = 0;  
const unsigned long interval_two = 3000;

unsigned long previousMillis_three = 0;  
const unsigned long interval_three = 5000;

const float CAL_SLOPE = 0.02496488;     // slope
const float CAL_INTERCEPT = 0.37211691; // intercept

float totaliser_1 = 0, flow_1 = 0, totaliser_2 = 0, flow_2 = 0;
float volt_r = 0, volt_y = 0, volt_b = 0, 
      current_r = 0, current_y = 0, current_b = 0, 
      total_kw = 0, pf = 0, frq = 0, rn_hr = 0, total_kwh = 0;


String status0, status1, status2, status3, status4, status5, status6, status7;
String status8, status9, status10, status11, status12, status13, status14, status15;

const char* channelLabels[16] = {
  "CT1",   // CH0
  "CT2",   // CH1
  "CT3",   // CH2
  "CT4",   // CH3
  "CT5",   // CH4
  "CT6",   // CH5
  "CT7",   // CH6
  "CT8",   // CH7
  "CT9",   // CH8
  "CT10",  // CH9
  "CT11",  // CH10
  "CT12",  // CH11
  "CT13",  // CH12
  "CT14",  // CH13
  "CT15",  // CH14
  "CT16"   // CH15
};

void setup() {
  Serial.begin(115200);
  rs485.begin(9600);
  pinMode(RS485CommControl, OUTPUT);
  digitalWrite(RS485CommControl, RS485Receive);
  pinMode(muxs0, OUTPUT);
  pinMode(muxs1, OUTPUT);
  pinMode(muxs2, OUTPUT);
  pinMode(muxs3, OUTPUT);
  pinMode(muxen, OUTPUT);
  digitalWrite(muxen, LOW); 
  pinMode(A1, OUTPUT);  // PC1
  pinMode(A2, OUTPUT);  // PC2
  delay(100);  
  //Serial.println("Started");
}

void setChannel(int channel) {
  digitalWrite(muxs0, (channel & 0x01) ? HIGH : LOW);
  digitalWrite(muxs1, (channel & 0x02) ? HIGH : LOW);
  digitalWrite(muxs2, (channel & 0x04) ? HIGH : LOW);
  digitalWrite(muxs3, (channel & 0x08) ? HIGH : LOW);
  delay(5); // allow signal to settle
}

void loop() {
    unsigned long currentMillis = millis();
    if (currentMillis - previousMillis_three >= interval_three) 
    {
      previousMillis_three = currentMillis; 
      Serial.println(Send_String());
    }    
    if (currentMillis - previousMillis_two >= interval_two) 
    {
      previousMillis_two = currentMillis; 
      for (int ch = 0; ch < 16; ch++) {
      setChannel(ch);

      long sum = 0;
      for (int j = 0; j < samples; j++) {
        int rawADC = analogRead(CTmonitor);
        sum += rawADC;
        delayMicroseconds(200);
      }

      float avgADC = (float)sum / samples;
      float amps = CAL_SLOPE * avgADC + CAL_INTERCEPT;

      // Separate if conditions and separate string storage
      if (ch == 0) {
        if (amps > Treshold) status0 = "ON"; else status0 = "OFF";
       // Serial.print(channelLabels[ch]); Serial.print(" | Current: "); Serial.print(amps,3); Serial.print(" | Status: "); Serial.println(status0);
      }
      else if (ch == 1) {
        if (amps > Treshold) status1 = "ON"; else status1 = "OFF";
      //  Serial.print(channelLabels[ch]); Serial.print(" | Current: "); Serial.print(amps,3); Serial.print(" | Status: "); Serial.println(status1);
      }
      else if (ch == 2) {
        if (amps > Treshold) status2 = "ON"; else status2 = "OFF";
      //  Serial.print(channelLabels[ch]); Serial.print(" | Current: "); Serial.print(amps,3); Serial.print(" | Status: "); Serial.println(status2);
      }
      else if (ch == 3) {
        if (amps > Treshold) status3 = "ON"; else status3 = "OFF";
       // Serial.print(channelLabels[ch]); Serial.print(" | Current: "); Serial.print(amps,3); Serial.print(" | Status: "); Serial.println(status3);
      }
      else if (ch == 4) {
        if (amps > Treshold) status4 = "ON"; else status4 = "OFF";
       // Serial.print(channelLabels[ch]); Serial.print(" | Current: "); Serial.print(amps,3); Serial.print(" | Status: "); Serial.println(status4);
      }
      else if (ch == 5) {
        if (amps > Treshold) status5 = "ON"; else status5 = "OFF";
       // Serial.print(channelLabels[ch]); Serial.print(" | Current: "); Serial.print(amps,3); Serial.print(" | Status: "); Serial.println(status5);
      }
      else if (ch == 6) {
        if (amps > Treshold) status6 = "ON"; else status6 = "OFF";
        //Serial.print(channelLabels[ch]); Serial.print(" | Current: "); Serial.print(amps,3); Serial.print(" | Status: "); Serial.println(status6);
      }
      else if (ch == 7) {
        if (amps > Treshold) status7 = "ON"; else status7 = "OFF";
        //Serial.print(channelLabels[ch]); Serial.print(" | Current: "); Serial.print(amps,3); Serial.print(" | Status: "); Serial.println(status7);// end
      }
      else if (ch == 8) {
        if (amps > Treshold) status8 = "ON"; else status8 = "OFF";
        ////Serial.print(channelLabels[ch]); Serial.print(" | Current: "); Serial.print(amps,3); Serial.print(" | Status: "); Serial.println(status8);
      }
      else if (ch == 9) {
        if (amps > Treshold) status9 = "ON"; else status9 = "OFF";
        //Serial.print(channelLabels[ch]); Serial.print(" | Current: "); Serial.print(amps,3); Serial.print(" | Status: "); Serial.println(status9);
      }
      else if (ch == 10) {
        if (amps > Treshold) status10 = "ON"; else status10 = "OFF";
       // Serial.print(channelLabels[ch]); Serial.print(" | Current: "); Serial.print(amps,3); Serial.print(" | Status: "); Serial.println(status10);
      }
      else if (ch == 11) {
        if (amps > Treshold) status11 = "ON"; else status11 = "OFF";
        //Serial.print(channelLabels[ch]); Serial.print(" | Current: "); Serial.print(amps,3); Serial.print(" | Status: "); Serial.println(status11);
      }
      else if (ch == 12) {
        if (amps > Treshold) status12 = "ON"; else status12 = "OFF";
       // Serial.print(channelLabels[ch]); Serial.print(" | Current: "); Serial.print(amps,3); Serial.print(" | Status: "); Serial.println(status12);
      }
      else if (ch == 13) {
        if (amps > Treshold) status13 = "ON"; else status13 = "OFF";
       // Serial.print(channelLabels[ch]); Serial.print(" | Current: "); Serial.print(amps,3); Serial.print(" | Status: "); Serial.println(status13);
      }
      else if (ch == 14) {
        if (amps > Treshold) status14 = "ON"; else status14 = "OFF";
       // Serial.print(channelLabels[ch]); Serial.print(" | Current: "); Serial.print(amps,3); Serial.print(" | Status: "); Serial.println(status14);
      }
      else if (ch == 15) {
        if (amps > Treshold) status15 = "ON"; else status15 = "OFF";
       // Serial.print(channelLabels[ch]); Serial.print(" | Current: "); Serial.print(amps,3); Serial.print(" | Status: "); Serial.println(status15);
      }
    }
    //Serial.println("-----------------------------------------------------------------------------------------------------------------------------");
  }
  if (currentMillis - previousMillis >= interval) {// t0 30
    previousMillis = currentMillis; 
    if(ReqRecFM == 0)
    {
      i++;
      if( i > 5)
      {
        //Serial.println("Send em failed");
        ReqRecFM = 1;
        i = 0;
      }
      RequestFM = 0;
      sendEMRequest();
      //Serial.println("Send em");
    }
    else 
    {
      j++;
      if( j > 5)
      {
        //Serial.println("Send fm failed");
        ReqRecFM = 0;
        j = 0;
      }
      RequestFM = 1;
      sendFMRequest();
      delay(3000);
     // Serial.println("Send fm");
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
        // Serial.print(responseFM[indexEM], HEX);
        // Serial.print(" ");
        indexEM++;
    }
    delay(50);
    EMReading();
    }
    }
  }
  delay(100);
}

void sendFMRequest() {
  //Serial.println("FM Request Sent");
  digitalWrite(A1, HIGH);
  digitalWrite(A2, HIGH);
  delay(1000);
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
  float totaliser_2_previous;
  if (indexFM >= 45 && responseFM[1] == 0x03) 
  {
    // Read registers from indexFM 3 onward (3 = byte count start)
    uint16_t total2_lsw   = (responseFM[3+28] << 8) | responseFM[3+29]; // 40017
    uint16_t total2_msw   = (responseFM[3+30] << 8) | responseFM[3+31]; // 40018
    uint16_t flow1_lsw    = (responseFM[3+24] << 8) | responseFM[3+25]; // 40015
    uint16_t flow1_msw    = (responseFM[3+26] << 8) | responseFM[3+27]; // 40016

    flow_1      = combineRegisters(flow1_msw, flow1_lsw) * 0.01;
    totaliser_2 = combineRegisters(total2_msw, total2_lsw) * 0.01;
    if(totaliser_2 == 0)
    {
       //totaliser_2 = totaliser_2_previous;
    }
    else 
    {
     // totaliser_2_previous = totaliser_2;
      digitalWrite(A1, LOW);
      digitalWrite(A2, LOW);
      delay(1000);
    }
    // Serial.print("Totaliser 1: "); Serial.println(totaliser_2, 2);
    // Serial.print("Flow 1:      "); Serial.println(flow_1, 2);
    // Serial.println("");
    ReqRecFM = 0;
  } 
  else 
  {

  }
}


void sendEMRequest() 
{
  //Serial.println("EM Request Sent");
  digitalWrite(RS485CommControl, RS485Transmit);
  delay(10);
  uint8_t request[] = {0x01 ,0x04 ,0x00 ,0x00 ,0x00 ,0x18 ,0xF0 ,0x00}; 
  rs485.write(request, sizeof(request));
  delay(10);
  digitalWrite(RS485CommControl, RS485Receive);
  delay(10);
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
      // Serial.println("\r\n");
      // Serial.print("PHASE R (V): ");
      // Serial.println(volt_r, 2);
      // Serial.print("PHASE Y (V): ");
      // Serial.println(volt_y, 2);
      // Serial.print("PHASE B (V): ");
      // Serial.println(volt_b, 2);

      // Serial.print("PHASE R (I): ");
      // Serial.println(current_r, 2);
      // Serial.print("PHASE Y (I): ");
      // Serial.println(current_y, 2);
      // Serial.print("PHASE B (I): ");
      // Serial.println(current_b, 2);

      // Serial.print("TOTAL KW: ");
      // Serial.println(total_kw, 2);
      // Serial.print("PF: ");
      // Serial.println(pf, 2);
      // Serial.print("FREQUENCY: ");
      // Serial.println(frq, 2);
      // Serial.print("RUN HR: ");
      // Serial.println(rn_hr, 2);
      // Serial.print("TOTAL KWH: ");
      // Serial.println(total_kwh, 2);
      ReqRecFM = 1;
    }
    else 
    {
     // Serial.println("Insufficient data received.");
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
   // Serial.println("Float value is invalid (ovf).");
    return NAN;
  }
}

String Send_String()
{
  String url = "http://sili.ppvserver.com/api/Siligreen/Data?deviceid=200";

  url += "&v1="  + String(volt_r, 2);
  url += "&v2="  + String(volt_y, 2);
  url += "&v3="  + String(volt_b, 2);
  url += "&v4="  + String(current_r, 2);
  url += "&v5="  + String(current_y, 2);
  url += "&v6="  + String(current_b, 2);
  url += "&v7="  + String(total_kw, 2);
  url += "&v8="  + String(pf, 2);
  url += "&v9="  + String(frq, 2);
  url += "&v10=" + String(rn_hr, 2);
  url += "&v11=" + String(total_kwh, 2);
  url += "&v12=" + String(status0);
  url += "&v13=" + String(status1);
  url += "&v14=" + String(status12);
  url += "&v15=" + String(status8);
  url += "&v16=" + String(status0);
  url += "&v17=" + String(status0);
  url += "&v18=" + String(status0);
  url += "&v19=" + String(flow_1);
  url += "&v20=" + String(totaliser_2);
  return url;
  //Serial.println(url);
}
