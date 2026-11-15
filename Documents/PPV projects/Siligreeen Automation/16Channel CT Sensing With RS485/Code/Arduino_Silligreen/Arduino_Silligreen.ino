#include <SoftwareSerial.h>
#include <math.h>

// RX=2, TX=3
SoftwareSerial mySerial(2, 3);

// === MUX pin mapping ===
#define SIG_A 8
#define SIG_B 9
#define SIG_C 10
#define SIG_D 11
#define EN    12

const int muxSIG = A0; // Multiplexer output to Arduino ADC

// === CT Parameters (adjust for your CT and burden resistor) ===
float Vref = 5.0;        // ADC reference voltage
int N = 1000;            // CT ratio (e.g., 1000:1)
float R_BURDEN = 33.0;   // Burden resistor in ohms

// === Other measurement variables ===
float Three_Phase_Current = 13.0; // v13 (dummy value, replace with calc)
float Three_Phase_Voltage = 14.0; // v14
float Power_Factor = 15.0;        // v15
float KiloWatts = 16.0;           // v16
float Flow_Meter = 17.0;          // v17

// === Setup ===
void setup() {
  Serial.begin(115200);
  mySerial.begin(115200);

  pinMode(SIG_A, OUTPUT);
  pinMode(SIG_B, OUTPUT);
  pinMode(SIG_C, OUTPUT);
  pinMode(SIG_D, OUTPUT);
  pinMode(EN, OUTPUT);

  digitalWrite(EN, LOW); // Enable mux (active LOW)

  Serial.println("CT + URL system start");
}

// === Select mux channel ===
void setChannel(int channel) {
  digitalWrite(SIG_A, channel & 0x01);
  digitalWrite(SIG_B, (channel >> 1) & 0x01);
  digitalWrite(SIG_C, (channel >> 2) & 0x01);
  digitalWrite(SIG_D, (channel >> 3) & 0x01);
}

// === Read RMS current from mux channel ===
float readCurrent(int channel) {
  int adc;
  setChannel(channel);
  delay(5);

  long sumSq = 0;
  int samples = 800;  // adjust for speed vs accuracy
  
  for (int i = 0; i < samples; i++) {
    adc = analogRead(muxSIG);
    int centered = adc - 512; // remove mid-bias
    sumSq += (long)centered * centered;
  }
  Serial.print("Channel [");
  Serial.print(channel);
  Serial.print("] > ");
  Serial.println(adc);
  float meanSq = (float)sumSq / samples;
  float adcRMS = sqrt(meanSq);

  float vRMS = (adcRMS * Vref) / 1024.0;
  float iRMS = (vRMS / R_BURDEN) * N;
  return iRMS;
}

void loop() {
  float CT[16];

  // Read all 16 CT channels
  for (int ch = 0; ch < 16; ch++) {
    CT[ch] = readCurrent(ch);
  }

  // Phase averages
  float Three_Phase_One = (CT[0] + CT[1] + CT[2]) / 3.0;
  float Three_Phase_Two = (CT[3] + CT[4] + CT[5]) / 3.0;

  // Map CT7–CT16 → v3…v12
  float v3  = CT[6];
  float v4  = CT[7];
  float v5  = CT[8];
  float v6  = CT[9];
  float v7  = CT[10];
  float v8  = CT[11];
  float v9  = CT[12];
  float v10 = CT[13];
  float v11 = CT[14];
  float v12 = CT[15];

  // === Build URL string ===
  String Serial_Send = 
    "http://sili.ppvserver.com/api/Siligreen/Data?deviceid=200" +
    String("&v1=")  + String(Three_Phase_One, 2) +
    String("&v2=")  + String(Three_Phase_Two, 2) +
    String("&v3=")  + String(v3, 2) +
    String("&v4=")  + String(v4, 2) +
    String("&v5=")  + String(v5, 2) +
    String("&v6=")  + String(v6, 2) +
    String("&v7=")  + String(v7, 2) +
    String("&v8=")  + String(v8, 2) +
    String("&v9=")  + String(v9, 2) +
    String("&v10=") + String(v10, 2) +
    String("&v11=") + String(v11, 2) +
    String("&v12=") + String(v12, 2) +
    String("&v13=") + String(Three_Phase_Current, 2) +
    String("&v14=") + String(Three_Phase_Voltage, 2) +
    String("&v15=") + String(Power_Factor, 2) +
    String("&v16=") + String(KiloWatts, 2) +
    String("&v17=") + String(Flow_Meter, 2);

  // === Output ===
  Serial.println(Serial_Send); // Debug USB serial
  mySerial.println(Serial_Send); // Send on pins 2,3

  delay(2000); // every 2 sec
}
