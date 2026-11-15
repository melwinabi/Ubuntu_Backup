#include "SoftwareSerial.h"

float avgResult = 0, currentInmA = 0;
int ADC_RESOLUTION = 1024;

String ssid ="PPV TECH";
String password="ppv@12345678";
String server = "www.ppvgroups.com";
String uri = "/unitech/save.php?";

SoftwareSerial esp(6, 7);// RX, TX

const int DP_RLY   =  5;
double Irms = 0;
double anx = 0, ab1 = 0, ab2 = 0, rcp = 0, ff1 = 0, ff2 = 0;
int dp = 0;
String  data_all, dt_anx, dt_ab1, dt_ab2, dt_rcp, dt_ff1, dt_ff2, dt_dp;

const int muxs0 = 7;
const int muxs1 = 8;
const int muxs2 = 9;
const int muxs3 = 10;

const int muxen = 11; // MUX ENABLE / 23
const int CTmonitor =  A0;

void setup()
{
  Serial.begin(9600);
  esp.begin(115200);
  pinMode(muxs0, OUTPUT);
  pinMode(muxs1, OUTPUT);
  pinMode(muxs2, OUTPUT);
  pinMode(muxs3, OUTPUT);
  pinMode(muxen, OUTPUT);
  pinMode(DP_RLY, OUTPUT);

  digitalWrite(muxen, LOW); // MUX REFERANCE
  digitalWrite(DP_RLY, LOW);
  reset();
  connectWifi();
}

void reset()
{
  esp.println("AT+RST");
  delay(1000);
  if(esp.find("OK") ) Serial.println("Module Reset");
  else Serial.println("Non Reset");
}

void connectWifi() 
{
  esp.println("AT+RST");
  delay(1000);
  esp.println("AT+CWMODE=1");
  delay(2000);
  String cmd = "AT+CWJAP=\""+ssid+"\",\""+password+"\"";
  esp.println(cmd);
  Serial.println(cmd);
  delay(10000);
  if(esp.find("OK")) 
    {
      Serial.println("Connected!");
    }
  else
    {
      Serial.println("Cannot connect to wifi"); 
      connectWifi();
    }
}
void loop()
{
  digitalWrite(muxs0, LOW);
  digitalWrite(muxs1, LOW);
  digitalWrite(muxs2, LOW);
  digitalWrite(muxs3, LOW);

  for(int i=0;i<10;i++)
  {
    avgResult += GetpeakTopeak(); 
  }
  avgResult = avgResult/10.00;
  currentInmA =  0.0080 * avgResult;
  anx = currentInmA;
  Serial.print("ANX  : ");
  Serial.println(anx);
  avgResult = 0;
  // ******************************
  digitalWrite(muxs0, HIGH);
  digitalWrite(muxs1, LOW);
  digitalWrite(muxs2, LOW);
  digitalWrite(muxs3, LOW);
  
  for(int i=0;i<10;i++)
  {
    avgResult += GetpeakTopeak(); 
  }
  avgResult = avgResult/10.00;
  currentInmA =  0.0080 * avgResult;
  ab1 = currentInmA;
  Serial.print("AB1  : ");
  Serial.println(ab1);
  avgResult = 0; 
  // ******************************
  digitalWrite(muxs0, LOW);
  digitalWrite(muxs1, HIGH);
  digitalWrite(muxs2, LOW);
  digitalWrite(muxs3, LOW);
 
  for(int i=0;i<10;i++)
  {
    avgResult += GetpeakTopeak(); 
  }
  avgResult = avgResult/10.00;
  currentInmA =  0.0080 * avgResult;
  ab2 = currentInmA;
  Serial.print("AB2  : ");
  Serial.println(ab2);
  avgResult = 0;
  // ******************************
  digitalWrite(muxs0, HIGH);
  digitalWrite(muxs1, HIGH);
  digitalWrite(muxs2, LOW);
  digitalWrite(muxs3, LOW);
  
  for(int i=0;i<10;i++)
  {
    avgResult += GetpeakTopeak(); 
  }
  avgResult = avgResult/10.00;
  currentInmA =  0.0080 * avgResult;
  rcp = currentInmA;
  Serial.print("RCP  : ");
  Serial.println(rcp);
  avgResult = 0;
  // ******************************
  digitalWrite(muxs0, LOW);
  digitalWrite(muxs1, LOW);
  digitalWrite(muxs2, HIGH);
  digitalWrite(muxs3, LOW);
  
  for(int i=0;i<10;i++)
  {
    avgResult += GetpeakTopeak(); 
  }
  avgResult = avgResult/10.00;
  currentInmA =  0.0080 * avgResult;
  ff1 = currentInmA;
  Serial.print("FF1  : ");
  Serial.println(ff1);
  avgResult = 0;
  // ******************************
  digitalWrite(muxs0, HIGH);
  digitalWrite(muxs1, LOW);
  digitalWrite(muxs2, HIGH);
  digitalWrite(muxs3, LOW);
  
  for(int i=0;i<10;i++)
  {
    avgResult += GetpeakTopeak(); 
  }
  avgResult = avgResult/10.00;
  currentInmA =  0.0080 * avgResult;
  ff2 = currentInmA;
  Serial.print("FF2  : ");
  Serial.println(ff2);
  avgResult = 0;
  // ******************************
  if(anx < 0.10 ) { anx = 0; }
  if(ab1 < 0.10 ) { ab1 = 0; }
  if(ab2 < 0.10 ) { ab2 = 0; }
  if(rcp < 0.10 ) { rcp = 0; }
  if(ff1 < 0.10 ) { ff1 = 0; }
  if(ff2 < 0.10 ) { ff2 = 0; }
  // ******************************
  if(ff1 > 1 || ff2 > 1)  
  {
    digitalWrite(DP_RLY, HIGH);
    dp = 1;
  }
  else
  {
    digitalWrite(DP_RLY, LOW);
    dp = 0;
  }
  dt_dp  = String(dp);
  dt_anx = String(anx);
  dt_ab1 = String(ab1);
  dt_ab2 = String(ab2);
  dt_rcp = String(rcp);
  dt_ff1 = String(ff1);
  dt_ff2 = String(ff2);
  
  data_all = "status=" + dt_dp + "&a=" + dt_anx + "&b=" + dt_ab1 + "&c=" + dt_ab2 + "&d=" + dt_rcp + "&e=" + dt_ff1 + "&f=" + dt_ff2;
  httppost();
  delay(1000);
}

void httppost () 
{
  Serial.println("Network");
  esp.println("AT+CIPSTART=\"TCP\",\"" + server + "\",80");//start a TCP connection.
  if( esp.find("OK")) 
  {
    Serial.println("TCP connection ready");
  } 
  delay(5000);

  String postRequest =
  
  "POST " + uri + " HTTP/1.0\r\n" +
  
  "Host: " + server + "\r\n" +
  
  "Accept: *" + "/" + "*\r\n" +
  
  "Content-Length: " + data_all.length() + "\r\n" +
  
  "Content-Type: application/x-www-form-urlencoded\r\n" +
  
  "\r\n" + data_all;
  
  String sendCmd = "AT+CIPSEND=";//determine the number of caracters to be sent.
  esp.print(sendCmd);
  esp.println(postRequest.length() );
  delay(1000);
  
  if(esp.find(">")) 
  { 
    Serial.println("Sending.."); esp.print(postRequest);
    if( esp.find("SEND OK")) 
    { 
      Serial.println("Packet sent");
      while (esp.available()) 
      {
        String tmpResp = esp.readString();
        Serial.println(tmpResp);
      }
      esp.println("AT+CIPCLOSE");
    }
  }
}
int16_t GetpeakTopeak() 
{
  int16_t ctValueHigh = 0;
  int16_t ctValueLow = ADC_RESOLUTION;

  for (uint8_t i = 0; i < 40; i++) 
  {
    if (analogRead(CTmonitor) > ctValueHigh)
      ctValueHigh = analogRead(CTmonitor);
    if (analogRead(CTmonitor) < ctValueLow)
      ctValueLow = analogRead(CTmonitor);
    delayMicroseconds(500);
  }
  return (ctValueHigh - ctValueLow);
}
