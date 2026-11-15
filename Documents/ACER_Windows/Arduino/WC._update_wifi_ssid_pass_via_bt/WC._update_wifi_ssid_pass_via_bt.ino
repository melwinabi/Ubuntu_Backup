#include <EEPROM.h>
#include "BluetoothSerial.h"
#include "WiFi.h"
BluetoothSerial bt;
#define EEPROM_SIZE 64
String buffer_in;
unsigned long previousMillis = 0; 
byte val;       
int addr = 0;
byte indS=0;
byte indP=0;
String stream;
byte len=0;
String temp;
String temp2;
unsigned int interval=30000; 
void setup() {
  Serial.begin(9600);
  EEPROM.begin(EEPROM_SIZE);
  Serial.println("Bluetooth Device is Ready to Pair");
  Serial.println("Waiting For Wifi Updates 30 seconds");
  bt.begin("ESP32");//bt name
  while(!check_wifiUpdate()==true)
  {
  }
  Serial.println("The Stored Wifi credetial are : ");
  for(int i=0;i<50;i++){
    val=EEPROM.read(i);
    stream+=(char)val;
  if((val==10) && (indS==0))
{
  indS=i;
  Serial.println("indS"+(String)i);
}
  else if(val==10 && indP==0)
  {
    indP=i;
    break;
  Serial.println("indP"+(String)i);
    }
}
 temp=stream.substring(0,indS);
 temp=temp.substring(5,indS);

 temp2=stream.substring(indS+1,indP);
 temp2=temp2.substring(5,indP-indS);

 int i=temp.length();
 int j=temp2.length();
 char ssid[i];
 char pass[j]; 
 temp.toCharArray(ssid,i);
 temp2.toCharArray(pass,j);

 Serial.println("Stored SSID");
 Serial.println(ssid);
 Serial.println("Stored PASS");
 Serial.println(pass);
 WiFi.mode(WIFI_STA);
 WiFi.begin(ssid, pass);
 if (WiFi.waitForConnectResult() != WL_CONNECTED) 
 {
    Serial.println("WiFi Failed");
    while(1) {
            delay(1000);
        }
    }
    else 
    {
      Serial.print("Wifi Connected to ");
      Serial.println(ssid);

}
}

void loop() {

}


boolean check_wifiUpdate()
{
  unsigned long currentMillis = millis();

  if (currentMillis - previousMillis >= interval) {
    previousMillis = currentMillis;
   

    Serial.println("30 Seconds Over");

    return true;
  }

    else if (bt.available()) 
  {
    interval=50000;
    buffer_in = bt.readStringUntil('\n'); //Read what we recevive 
    //Serial.println("Received:"); Serial.println(buffer_in);
    delay(20);
 

 if(buffer_in.charAt(0)=='S')
{
 for(int i=0;i<buffer_in.length();i++)
 {val=(byte)(buffer_in.charAt(i));
 //Serial.println("val "+val);

  EEPROM.write(addr, val);
  //Serial.println(val);
   addr++;
}
//Serial.print("New ");
//Serial.print(buffer_in);
EEPROM.write(addr, 10);
addr++;
EEPROM.commit();     
   bt.println("SSID Stored");
  }

 else if(buffer_in.charAt(0)=='P')
{
 for(int i=0;i<buffer_in.length();i++)
 {val=(byte)(buffer_in.charAt(i));
 //Serial.println("val "+val);
  EEPROM.write(addr, val);
  //Serial.println(val);
  addr++;
  
}
//Serial.print("New ");
//Serial.print(buffer_in);
EEPROM.write(addr, 10);
EEPROM.commit();  
bt.println("Password Stored"); 
 return true;
  }  
 
  return false;
  }
else
{ return false;
  }
}//working
