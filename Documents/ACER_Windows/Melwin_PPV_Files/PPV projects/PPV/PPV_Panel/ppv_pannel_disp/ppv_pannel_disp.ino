#include <EEPROM.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
LiquidCrystal_I2C lcd(0x27,20,4);
#include <DS3231M.h>
DS3231M_Class DS3231M;

int Ph1,Ph2,Ph3,Ph4,Ph5;
int dl;
int AM = 7;
int menu = 2;
int up = 3;
int down = 4;
int back = 5;
int select = 6;
int S_f = 0;
int Y = 1;
int Flag = 0;
int M, U, D, B, S, A;
int j;
int R;
int C = 0;
int sts=0;  
int St;
int n,o;
int r,t;
int Dec_Sec_1;
int Dec_Sec_2;
int Dec_Sec_3;
int Dec_Sec_4; 
int Dec_Sec_5;
int Sh1, Sm1;
int Sh2, Sm2;
int Sh3, Sm3;
int Sh32, Sm32;
int Sh4, Sm4;
int Sh5, Sm5;
int Sec32;
int statemp;
int Stated=0;
int Sl=0;
int ns=0;
int sn=0;
int Counter;
int Count=0;
int hr, mn, sec;
int yr,mon,dy;
int HR, MN, SEC;
int YR, MON, DY;
int Mod1,Mod2,Mod3,Mod4,Mod5;
int EDTA1,EDTA2,EDTA3,EDTA4,EDTA5;
int EPCOT1, ERWPb1, ERWPb2;
int EPCOT2, ERWPb3, ERWPb4;
int EPCOT3, ERWPb5, ERWPb6;
int EPCOT4, ERWPb7, ERWPb8;
int EPCOT5, ERWPb9, ERWPb10;
int SET_FLAG=1;
int SEC1;
int SEC2;
int SEC3;
int SEC32;
int SEC4;
int SEC5;
long int previous=0;
long int Time;
int unit1 = 0;
int unit2 = 0;
int unit3 = 0;
int unit4 = 0;
int unit5 = 0;
int unit6 = 0;
int unit7 = 0;
int unit8 = 0;
int unit9 = 0;
int unit10 = 0;
int get1;
int get2;
int get3;
int get4;
int get5;
int get6;
int get7;
int get8;
int get9;
int get10;
int ESh1;  
int ESm1;
int ESh2;  
int ESm2;
int ESh32;
int ESm32;
int ESh3;  
int ESm3;
int ESh4;  
int ESm4;
int ESh5;  
int ESm5;
int F1, F2, F3, F4, F5;
int T1, T2, T3, T4, T50;
int REV_FLAG;
unsigned long currentMillis1 = 0, prMill_1 = 0;
unsigned long currentMillis2 = 0, prMill_2 = 0;
unsigned long currentMillis3 = 0, prMill_3 = 0;
unsigned long currentMillis4 = 0, prMill_4 = 0;
unsigned long currentMillis5 = 0, prMill_5 = 0;
const long interval =  1000;
float Rating1,Rating2,Rating3,Rating4,Rating5;
int case1,case2,case3,case4,case5;
String ROW2,ROW3,ROW4;
String Mode1;
String Mode2;
String Mode3;
String Mode4;
String Mode5;
String Mode6;
int EmodeF1, EmodeF2, EmodeF3, EmodeF4, EmodeF5;
int EmodeT1, EmodeT2, EmodeT3, EmodeT4, EmodeT5;
int EMode1, EMode2, EMode3, EMode4, EMode5, EMode6;
int Run_Flag;
String modeF1="OFF";
String modeF2="OFF";
String modeF3="OFF";
String modeF4="OFF";
String modeF5="OFF";
String modeT1="OFF";
String modeT2="OFF";
String modeT3="OFF";
String modeT4="OFF";
String modeT5="OFF";
String PCOT1="OFF";
String PCOT2="OFF";
String PCOT3="OFF";
String PCOT4="OFF";
String PCOT5="OFF";
String RWPb1="OFF";
String RWPb2="OFF";
String RWPb3="OFF";
String RWPb4="OFF";
String RWPb5="OFF";
String RWPb6="OFF";
String RWPb7="OFF";
String RWPb8="OFF";
String RWPb9="OFF";
String RWPb10="OFF";
String LCD_msg=" ";
int Sec1;
int Sec2;
int Sec3;
int Sec4;
int Sec5;
int REV1;
int REV2;
int REV3;
int REV4;
int REV5;
int sec_flag1 = 1;
int sec_flag2 = 1;
int sec_flag3 = 1;
int sec_flag4 = 1;
int sec_flag5 = 1;
const int LED11 = 31;
const int LED12 = 32;
const int LED21 = 33;
const int LED22 = 34;
const int LED41 = 35;
const int LED42 = 36;
const int LED51 = 37;
const int LED52 = 38;
const int LED3 = 39;
const int LED6 = 40;

void setup() 
{
  Serial.begin(9600);
  lcd.begin(20, 4);
  lcd.init();
  lcd.backlight();
  delay(500.);
  lcd.clear();
  pinMode(menu, INPUT_PULLUP);
  pinMode(up, INPUT_PULLUP);
  pinMode(down, INPUT_PULLUP);
  pinMode(back, INPUT_PULLUP);
  pinMode(select, INPUT_PULLUP);
  pinMode(AM, INPUT_PULLUP);
  pinMode(LED11, OUTPUT);
  pinMode(LED12, OUTPUT);
  pinMode(LED21, OUTPUT);
  pinMode(LED22, OUTPUT);
  pinMode(LED41, OUTPUT);
  pinMode(LED42, OUTPUT);
  pinMode(LED51, OUTPUT);
  pinMode(LED52, OUTPUT);
  pinMode(LED3, OUTPUT);
  pinMode(LED6, OUTPUT);
  case1 = EEPROM.read(5);
  case2 = EEPROM.read(6);
  case3 = EEPROM.read(7);
  case4 = EEPROM.read(8);
  case5 = EEPROM.read(9);
  unit1 = EEPROM.read(10);
  unit2 = EEPROM.read(11);
  unit3 = EEPROM.read(12);
  unit4 = EEPROM.read(13);
  unit5 = EEPROM.read(14);
  unit6 = EEPROM.read(15);
  unit7 = EEPROM.read(16);
  unit8 = EEPROM.read(17);
  unit9 = EEPROM.read(18);
  unit10 = EEPROM.read(19);  
  EPCOT1 = EEPROM.read(20);
  ERWPb1 = EEPROM.read(21);
  ERWPb2 = EEPROM.read(22);
  EPCOT2 = EEPROM.read(23);
  ERWPb3 = EEPROM.read(24);
  ERWPb4 = EEPROM.read(25);
  EPCOT3 = EEPROM.read(26);
  ERWPb5 = EEPROM.read(27);
  ERWPb6 = EEPROM.read(28);
  EPCOT4 = EEPROM.read(29);
  ERWPb7 = EEPROM.read(30);
  ERWPb8 = EEPROM.read(31);
  EPCOT5 = EEPROM.read(32);
  ERWPb9 = EEPROM.read(33);
  ERWPb10 = EEPROM.read(34);
  EmodeF1 = EEPROM.read(35); 
  EmodeT1 = EEPROM.read(36);
  EmodeF2 = EEPROM.read(37); 
  EmodeT2 = EEPROM.read(38);
  EmodeF3 = EEPROM.read(39); 
  EmodeT3 = EEPROM.read(40); 
  EmodeF4 = EEPROM.read(41); 
  EmodeT4 = EEPROM.read(42); 
  EmodeF5 = EEPROM.read(43); 
  EmodeT5 = EEPROM.read(44);
  ESh1 = EEPROM.read(45);  
  ESm1 = EEPROM.read(46);
  ESh2 = EEPROM.read(47);  
  ESm2 = EEPROM.read(48);
  ESh3 = EEPROM.read(49);  
  ESm3 = EEPROM.read(50);  
  ESh32 = EEPROM.read(61);  
  ESm32 = EEPROM.read(62);;
  ESh4 = EEPROM.read(51);  
  ESm4 = EEPROM.read(52);
  ESh5 = EEPROM.read(53);  
  ESm5 = EEPROM.read(54);
  EMode1 = EEPROM.read(55);
  EMode2 = EEPROM.read(56);
  EMode3 = EEPROM.read(57);
  EMode4 = EEPROM.read(58);
  EMode5 = EEPROM.read(59);
  EMode6 = EEPROM.read(60); 
  EEPROM.write(63,0);
  Serial.println("Setup OK");
}

void loop() 
{ 
  M = digitalRead(menu);
  U = digitalRead(up);
  D = digitalRead(down);
  B = digitalRead(back);
  S = digitalRead(select); 
  A = digitalRead(AM);     
  delay(275);

  if(M == LOW || U == LOW || D == LOW || B == LOW || S == LOW || A == LOW)
  {
    Counter = 1;
    Count = 0;
  }
  else
  {
    Counter = 0;
  }

  if(Counter == 0)
  {
    Count++;
  }

  if(Count >= 2000) 
  {
    S_f = 0;
  }

  if (S_f == 0) 
  {  
    Run_Flag=1;
    Scrolling_screen(Y);
    Y++;
    if (Y > 5) 
    {
      Y = 1; 
    }
  }

  if(M == LOW)
  {
    if(Flag == 0)
    {      
      Run_Flag=0;
      C = 0;
      LOW_ALL();
      Menu();
    }
    else 
    {
      M=HIGH;
      lcd.clear();
      Exit_Screen(); //in setting
    }
  }

  Back();
  Flag_1();
  Flag_2();
  //.......................
  Flag_3();
  Flag_4();
  Flag_16();
  Flag_17();
  Flag_18();
  Flag_19();
  Flag_20();
  //.......................
  Flag_5();
  Flag_6();
  Flag_22();
  Flag_23();
  Flag_24();
  Flag_25();
  Flag_26();
  //.......................
  Flag_38();
  Flag_32();
  Flag_33();
  Flag_35();
  Flag_36();
  //.......................
  Flag_7();
  Flag_8();
  Flag_27();
  Flag_28();
  Flag_29();
  Flag_30();
  Flag_31();
  //.......................
  Flag_9();
  Flag_10();
  Flag_40(); 
  Flag_41();
  Flag_42();
  Flag_43();
  Flag_44();
  //.......................
  Aut_Man();
  AUT_MAN_Setting();
  //.......................
  Gen();
  Flag_46();
  Flag_47();
  //.......................
  Flag_48(); 
  //.......................
  
//  Serial.print(ERWPb1);
//  Serial.println(ERWPb2);
}
