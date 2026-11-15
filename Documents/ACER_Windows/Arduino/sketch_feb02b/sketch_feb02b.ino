void setup() 
{
  Serial.begin(115200);
  Serial2.begin(9600);  
  sendCommand("AT\r\n",90);
  sendCommand("AT+CGDCONT=1,\"IP\",\"airtelgprs.com\"\r\n", 0);
  sendCommand("AT+CGACT=1,1\r\n", 90);
  sendCommand("AT+CSQ\r\n", 90);
  sendCommand("AT+QMTCLOSE=0\r\n", 90);
  sendCommand("AT+QMTOPEN=0,\"198.38.81.119\",1883\r\n", 90);
  sendCommand("AT+QMTCONN=0,\"Cenmark_Valve_Control\"\r\n", 90);
  sendCommand("AT+QMTPUB=0,0,0,0,\"RX_Valve_test\",\"Valve Open\"\r\n", 90);
  sendCommand("AT+QMTCFG=\"recv/mode\",0,0,1\r\n", 90);
  sendCommand("AT+QMTSUB=0,1,\"Cenmark_PPV\",1\r\n", 90);

}

void loop() 
{
  if (Serial2.available()>0)
  {
    String string = Serial2.readString()
    Serial.println(string);
  }

}

void sendCommand(String command, int waitTime) {
  Serial.print("Sending: ");
  Serial.println(command);
  Serial2.print(command); // Send command to GSM module
  delay(waitTime);                // Wait for response
}  
