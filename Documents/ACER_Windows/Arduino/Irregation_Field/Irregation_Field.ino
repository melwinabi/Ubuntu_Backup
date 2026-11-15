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

  Serial.print("LORA LOCAL TEST\r\n");
  //at_send_check_response("+AT: OK", 100, "AT\r\n");
  //at_send_check_response("+MODE: TEST", 1000, "AT+MODE=TEST\r\n");
 LORA.println("AT");
 delay(500);
 LORA.println("AT+MODE=TEST");
 delay(500);
 LORA.println("AT+TEST=RXLRPKT");
 delay(500);
}

void loop() 
{ 
  if (LORA.available()) 
  {
    ReceivedMessage = LORA.readStringUntil('\n');
    Serial.println("Received: " + ReceivedMessage);
  }
  // else
  // {
  //   LORA.println("AT+TEST=TXLRPKT,123");
  //   delay(1000); 
  // }
   
//   delay(1000);
}
int getRSSI() {
  // This function assumes your LoRa module can output RSSI
  // You might need to adjust this function according to your LoRa module's specifications
  if (LORA.available()) {
    String rssiString = LORA.readStringUntil('\n');
    if (rssiString.startsWith("RSSI:")) {
      return rssiString.substring(5).toInt();
    }
  }
  return 0; // Return 0 if RSSI is not available
}

  
  // if (LORA.available()) 
  // {
  //   Serial.write(LORA.read());
  // }
  // if (Serial.available()) {
  //   LORA.write(Serial.read());
  // }
  // TEMP_VALUE = analogRead(TEMP);
  // HUM_VALUE = analogRead(HUM);

  // if(TEMP_VALUE > 512)
  // {
  //   digitalWrite(SOL_1, HIGH);
  // }
  // else
  // {
  //   digitalWrite(SOL_1, LOW);
  // }

  // if(HUM_VALUE > 512)
  // {
  //   digitalWrite(SOL_2, HIGH);
  // }
  // else
  // {
  //   digitalWrite(SOL_2, LOW);
  // }

  // SEN_1_STATUS = digitalRead(SEN_4);
  // if(SEN_1_STATUS == LOW)
  // {
  //   digitalWrite(SOL_2, HIGH);
  // }
  // else
  // {
  //   digitalWrite(SOL_2, LOW);
  // }

  // digitalWrite(SOL_1, HIGH);
  // delay(3000);
  // digitalWrite(SOL_1, LOW);
  // delay(3000);
  // digitalWrite(SOL_2, HIGH);
  // delay(3000);
  // digitalWrite(SOL_2, LOW);
  // delay(3000);
