void setup() 
{
  Serial.begin(115200);
  Serial2.begin(9600);  
  sendCommand("AT\r\n", 900);
  sendCommand("AT+CGDCONT=1,\"IP\",\"airtelgprs.com\"\r\n", 900);
  sendCommand("AT+CGACT=1,1\r\n", 900);
  sendCommand("AT+CSQ\r\n", 900);
  sendCommand("AT+QMTCLOSE=0\r\n", 900);
  sendCommand("AT+QMTOPEN=0,\"198.38.81.119\",1883\r\n", 900);
  sendCommand("AT+QMTCONN=0,\"Cenmark_Valve_Control\"\r\n", 900);
  sendCommand("AT+QMTPUB=0,0,0,0,\"RX_Valve_test\",\"Valve Open\"\r\n", 900);
  sendCommand("AT+QMTCFG=\"recv/mode\",0,0,1\r\n", 900);
  sendCommand("AT+QMTSUB=0,1,\"Cenmark_PPV\",1\r\n", 900);
}

void loop() 
{
  if (Serial2.available() > 0)
  {
    String string = Serial2.readStringUntil('\n');
    Serial.println(string);
  }
}

void sendCommand(String command, int waitTime) 
{
  Serial.print("Sending: ");
  Serial.println(command);
  Serial2.print(command);
  delay(waitTime);
}
