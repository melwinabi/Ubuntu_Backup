#include <EEPROM.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include "RTClib.h"
RTC_DS3231 rtc;
LiquidCrystal_I2C lcd(0x27,20,4);
int Ph1,Ph2,Ph3,Ph4,Ph5;
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
int Sh1, Sm1;
int Sh2, Sm2;
int Sh3, Sm3;
int Sh4, Sm4;
int Sh5, Sm5;
int statemp;
int Stated=0;
int Sl=0;
int ns=0;
int sn=0;
int Counter;
int Count=0;
int Year;
int Month;
int Date;
int Hour;
int Minute;
int Seconds;
int Mod1,Mod2,Mod3,Mod4,Mod5;
int EDTA1,EDTA2,EDTA3,EDTA4,EDTA5;
int EPCOT1, ERWPb1, ERWPb2;
int EPCOT2, ERWPb3, ERWPb4;
int EPCOT3, ERWPb5, ERWPb6;
int EPCOT4, ERWPb7, ERWPb8;
int EPCOT5, ERWPb9, ERWPb10;
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
int ESh3;  
int ESm3;
int ESh4;  
int ESm4;
int ESh5;  
int ESm5;
int F1, F2, F3, F4, F5;
int T1, T2, T3, T4, T5;
float Rating1,Rating2,Rating3,Rating4,Rating5;
void TaskButton(void *pvParameters);
void TaskLCD(void *pvParameters);
void Menu();
void Menu2();
void curs(int c);
int case1,case2,case3,case4,case5;
String ROW2,ROW3,ROW4;
String Mode1;
String Mode2;
String Mode3;
String Mode4;
String Mode5;
int EmodeF1, EmodeF2, EmodeF3, EmodeF4, EmodeF5;
int EmodeT1, EmodeT2, EmodeT3, EmodeT4, EmodeT5;
int EMode1, EMode2, EMode3, EMode4, EMode5;
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

void setup() {
  Serial.begin(9600);
  lcd.begin(20, 4);
  lcd.init();
  lcd.backlight();
  Wire.begin();  
  if (!rtc.begin()) {
    Serial.println("Couldn't find RTC");
    while (1);
  }
  if (rtc.lostPower()) {
    Serial.println("RTC lost power, setting the time!");
    rtc.adjust(DateTime(2024, 10, 21, 11, 56, 05));
  }
  lcd.init();
  lcd.backlight();
  delay(500);
  lcd.clear();
  pinMode(menu, INPUT_PULLUP);
  pinMode(up, INPUT_PULLUP);
  pinMode(down, INPUT_PULLUP);
  pinMode(back, INPUT_PULLUP);
  pinMode(select, INPUT_PULLUP);
  pinMode(AM, INPUT_PULLUP);
  case1=EEPROM.read(5);
  case2=EEPROM.read(6);
  case3=EEPROM.read(7);
  case4=EEPROM.read(8);
  case5=EEPROM.read(9);
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
  ESm1=EEPROM.read(46);
  ESh2 = EEPROM.read(47);  
  ESm2=EEPROM.read(48);
  ESh3 = EEPROM.read(49);  
  ESm3=EEPROM.read(50);
  ESh4 = EEPROM.read(51);  
  ESm4=EEPROM.read(52);
  ESh5 = EEPROM.read(53);  
  ESm5=EEPROM.read(54);
  EMode1=EEPROM.read(55);
  EMode2=EEPROM.read(56);
  EMode3=EEPROM.read(57);
  EMode4=EEPROM.read(58);
  EMode5=EEPROM.read(59);
  
}

void loop(){
 M = digitalRead(menu);
 U = digitalRead(up);
 D = digitalRead(down);
 B = digitalRead(back);
 S = digitalRead(select); 
 A = digitalRead(AM);     
 delay(275);
 if(M == LOW || U == LOW || D == LOW || B == LOW || S == LOW || A == LOW)
 {
 Counter=1;
 Count=0;
 }
 else
 {
  Counter=0;
 }
 if(Counter == 0)
 {
  Count++;
 }
 if(Count >= 200)//delay for screen out
 {
  S_f = 0;
 }
 if (S_f == 0) 
 {  
  Scrolling_screen(Y);
  Y++;
  if (Y > 5) 
  {
   Y = 1; 
  }
 }
 if(Flag == 0)
 {
  if(M == LOW)
  {
    C=0;
    Menu();
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
 //......................
 Flag_5();
 Flag_6();
 Flag_22();
 Flag_23();
 Flag_24();
 Flag_25();
 Flag_26();
 //......................
 Flag_38();
 Flag_39();
 Flag_32();
 Flag_33();
 Flag_34();
 Flag_35();
 Flag_36();
//.........................
 Flag_7();
 Flag_8();
 Flag_27();;
 Flag_28();
 Flag_29();
 Flag_30();
 Flag_31();
 //.................... 
 Flag_9();
 Flag_10();
 Flag_40(); 
 Flag_41();
 Flag_42();
 Flag_43();
 Flag_44();
 //.......................
 Aut_Man();
 //.......................
 Gen();

 




























 
 Serial.print("C :");
 Serial.print(C);
 Serial.print("  Flag :");
 Serial.println(Flag);
//int READ1=EEPROM.read(11);  
//Serial.println(ESh1); 
//Serial.println(ESm1);   
//Serial.println(case3);   
//Serial.println(case4);   
//Serial.println(case5);     
//Serial.println(unit1);
// Serial.println(ns);
//Serial.print("EPCOT1: ");
//Serial.println(EPCOT2);
//Serial.print("ERWPb1: ");
//Serial.println(ERWPb3);
//Serial.print("ERWPb2: ");
//Serial.println(Count);

} 
