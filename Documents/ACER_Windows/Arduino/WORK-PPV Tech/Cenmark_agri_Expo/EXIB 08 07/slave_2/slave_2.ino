#include <SoftwareSerial.h>
const int SOL_1 = 5;
const int SOL_2 = 6;
const int DI    = 11;

const int TEMP  = A1;
const int HUM   = A0;

const int SEN_1 = 7;
const int SEN_2 = 8;
const int SEN_3 = 9;
const int SEN_4 = 10;

//const int LORA_RST = 4;

int DI_STATUS = 0;
int SEN_1_STATUS = 0;
int SEN_2_STATUS = 0;
int SEN_3_STATUS = 0;
int SEN_4_STATUS = 0;

int TEMP_VALUE = 0;
int HUM_VALUE = 0;

String ReceivedMessage;

SoftwareSerial LORA(3, 2); // RX, TX

static char recv_buf[512];
static int led = 13;
int counter = 0;
String Data;

static int at_send_check_response(char *p_ack, int timeout_ms, char *p_cmd, ...)
{
    int ch;
    int num = 0;
    int index = 0;
    int startMillis = 0;
    va_list args;
    memset(recv_buf, 0, sizeof(recv_buf));
    va_start(args, p_cmd);
    LORA.print(p_cmd);
    Serial.print(p_cmd);
    va_end(args);
    delay(200);
    startMillis = millis();
 
    if (p_ack == NULL)
        return 0;
 
    do
    {
        while (LORA.available() > 0)
        {
            ch = LORA.read();
            recv_buf[index++] = ch;
            Serial.print((char)ch);
            delay(2);
        }
 
        if (strstr(recv_buf, p_ack) != NULL)
            return 1;
 
    } while (millis() - startMillis < timeout_ms);
    Serial.println();
    return 0;
}
void setup() 
{
  pinMode(SOL_1, OUTPUT);
  pinMode(SOL_2, OUTPUT);
  pinMode(DI,INPUT);
  pinMode(SEN_1, INPUT);
  pinMode(SEN_2, INPUT);
  pinMode(SEN_3, INPUT);
  pinMode(SEN_4, INPUT);
  Serial.begin(9600);
  LORA.begin(9600);
  //Serial.print("LORA LOCAL TEST\r\n");
  LORA.println("RESET");
  delay(5000);
  Serial.println("LORA RESETED");
  LORA.println("AT");
  delay(500);
  LORA.println("AT+MODE=TEST");
  delay(500);
  LORA.println("AT+TEST=RXLRPKT");
  delay(500);
//  LORA.println("AT+TEST=TXLRPKT");
//  delay(500);
}

void loop() 
{ 
  if (LORA.available()) 
  {
    //char c = LORA.read();
    ReceivedMessage = LORA.readStringUntil("\n");
    Serial.println("Received:" + ReceivedMessage);
  }
  if (ReceivedMessage.startsWith("TEST: RX \"")) 
  {
    int startIndex = ReceivedMessage.indexOf("\"") + 1;
    int endIndex = ReceivedMessage.lastIndexOf("\"");
    String message = ReceivedMessage.substring(startIndex, endIndex);
    Serial.println("Extracted message: " + message);
   if(message == "0123321030"){
      Serial.println("Valve Three Turn OFF");
      digitalWrite(SOL_1,LOW);
      LORA.println("AT+TEST=TXLRPKT,4456030");
      delay(100);
      LORA.println("AT+TEST=RXLRPKT");
      delay(100);
  }if(message=="0123321031"){
      Serial.println("Valve Three Turn ON");
      digitalWrite(SOL_1,HIGH);
      LORA.println("AT+TEST=TXLRPKT,456031");
      delay(100);
      LORA.println("AT+TEST=RXLRPKT");
      delay(100);
  }if(message=="0123321040"){
      Serial.println("Valve Four Turn OFF");
      digitalWrite(SOL_2,LOW);
      LORA.println("AT+TEST=TXLRPKT,4456040");
      delay(100);
      LORA.println("AT+TEST=RXLRPKT");
      delay(100);
  }if(message=="0123321041"){
      Serial.println("Valve Four Turn ON");
      digitalWrite(SOL_2,HIGH);
      LORA.println("AT+TEST=TXLRPKT,4456041");
      delay(100);
      LORA.println("AT+TEST=RXLRPKT");
      delay(100);
}
}
}
/*0123321010
 *0123321020
 *0123321030
 *0123321040  
 */
