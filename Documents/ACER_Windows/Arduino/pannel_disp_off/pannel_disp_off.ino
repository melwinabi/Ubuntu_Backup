#include <LiquidCrystal.h>
#include <EEPROM.h>
LiquidCrystal lcd(2, 3, 4, 5, 6, 7);
int Ph1,Ph2,Ph3,Ph4,Ph5;
int menu = 12;
int up = 11;
int down = 10;
int back = 9;
int select = 8;
int S_f = 0;
int Y = 1;
int Flag = 0;
int M, U, D, B, S;
int j;
int R;
int C = 0;
int sts=0;  
int St;
int n,o;
int r;
int statemp;
int Stated=0;
int Sl=0;
int ns=0;
int sn=0;
int Mod1,Mod2,Mod3,Mod4,Mod5;
int EDTA1,EDTA2,EDTA3,EDTA4,EDTA5;
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
int F1, F2, F3, F4, F5;
int T1, T2, T3, T4, T5;
float read_val=12.8;
void TaskButton(void *pvParameters);
void TaskLCD(void *pvParameters);
void Menu();
void Menu2();
void curs(int c);
long int case1,case2,case3,case4,case5;
String ROW2,ROW3,ROW4;
String Mode1;
String Mode2;
String Mode3;
String Mode4;
String Mode5;
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
  lcd.clear();
  pinMode(menu, INPUT_PULLUP);
  pinMode(up, INPUT_PULLUP);
  pinMode(down, INPUT_PULLUP);
  pinMode(back, INPUT_PULLUP);
  pinMode(select, INPUT_PULLUP);
  EEPROM.get(10, get1);
  EEPROM.get(12, get2);
  EEPROM.get(13, get3);
  EEPROM.get(15, get4);
  EEPROM.get(16, get5);
  EEPROM.get(18, get6);
  EEPROM.get(19, get7);
  EEPROM.get(21, get8);
  EEPROM.get(22, get9);
  EEPROM.get(24, get10);
  unit1 = get1;
  unit2 = get2;
  unit3 = get3;
  unit4 = get4;
  unit5 = get5;
  unit6 = get6;
  unit7 = get7;
  unit8 = get8;
  unit9 = get9;
  unit10 = get10;
  EEPROM.get(25, F1);
  EEPROM.get(26, T1);
  EEPROM.get(27, F2);
  EEPROM.get(28, T2);
  EEPROM.get(29, F3);
  EEPROM.get(30, T3);
  EEPROM.get(31, F4);
  EEPROM.get(32, T4);
  EEPROM.get(33, F5);
  EEPROM.get(34, T5);
}

void loop(){
      M = digitalRead(menu);
      U = digitalRead(up);
      D = digitalRead(down);
      B = digitalRead(back);
      S = digitalRead(select);      
      delay(275);   
//....................................Menu 
      if (M == LOW ) 
      { 
        C=0;      
        Menu();
      }
//....................................Flag1 & Flag2      
//...................................dOWN     
      if (Flag == 1 && D == LOW)
      { 
        C++;      
        curs(C);              
        if (C > 3 && Flag == 1) 
        {
          C=0;
          Menu2();
          curs(C);          
        }
      }

      else if (Flag == 2 && D == LOW) 
      {
        C++;
        curs(C);
        if (C > 1)
        {
          C = 0;
          curs(C);
        }
      }
//...................................BACK     
      if (Flag == 2 && B == LOW)
      {  
        C = 0;
        Menu();
      }

      if (Flag == 1 && C == 3 && S == LOW) 
      {

      }
//...................................up     
      else if (Flag == 1 && U == LOW) 
      {
        C--;
        curs(C);
        if (C <= 0) 
        {
          C = 0;
          curs(C);
        }
      }
      else if(Flag==2&&U==LOW)
      {
          C--;
          curs(C);
       if(C<=-1&&U==LOW)
       {
          C=3;
          Menu();
       }
     }     
//...................................SELECTTIONA
      if (C == 0 &&Flag == 1 && S == LOW&&sts==0)
      {
        C=0;
        statemp=1;
        set_A1();
      }      
      if(Flag==3&&D==LOW){
        C++;      
        curs(C);              
        if (C == 4 && Flag == 3) {
          set_A2();
          curs(0);  
      }
    }
//...................................UP
      if (Flag == 3 && U == LOW) 
      { 
        C--;
        curs(C);
        if (C < 0) 
        {
          C=0;
          curs(C);
        }
      }
      else if (Flag == 4 && U == LOW) {
        set_A1();
      }
//...................................UP  
//...................................SELECTTIONA.............................
  ////////////////neat end
//...................................SELECTTIONB
if (C == 1 &&Flag == 1 && S == LOW) {
        C=0;
        set_B1();
       }
      if(Flag==5&&D==LOW){
        C++;      
        curs(C);              
       if (Flag == 5&&C == 4) {
          set_B2();
          curs(0);  
      }
    }
//...............................UP.................
 
      if (Flag == 5 && U == LOW) { 
        C--;
        curs(C);
        if (C < 0) {
          C=0;
          curs(C);
        }
      }
      if (Flag == 6 && U == LOW) {
        set_B1();
      }
 //...................................UP......................   
//..............................SELECTTIONB.................. 

//..............................SELECTTIONC.................. 
if (C == 2 &&Flag == 1 && S == LOW) {
        C=0;
        set_C1();
       }
      if(Flag==7&&D==LOW){
        C++;      
        curs(C);              
       if (C == 4 && Flag == 7) {
          set_C2();
          curs(0);  
      }
    }
//...............................UP.................
 
      if (Flag == 7 && U == LOW) { 
        C--;
        curs(C);
        if (C < 0) {
          C=0;
          curs(C);
        }
      }
      if (Flag == 8 && U == LOW) {
        set_C1();
      }
//..............................SELECTTIONC.................. 


//..............................SELECTTIOND.................. 
if (C == 0 &&Flag == 2 && S == LOW) {
        C=0;
        set_D1();
       }
      if(Flag==9&&D==LOW){
        C++;      
        curs(C);              
       if (C == 4 && Flag == 9) {
          set_D2();
          curs(0);  
      }
    }
//...............................UP.................
 
      if (Flag == 9 && U == LOW) { 
        C--;
        curs(C);
        if (C < 0) {
          C=0;
          curs(C);
        }
      }
      if (Flag == 10 && U == LOW) {
        set_D1();
      }
//..............................SELECTTIOND.................. 
//..............................SELECTTIONE.................. 
if (C == 1 &&Flag == 2 && S == LOW) {
        clockD();
       }
//...........................UP.................
 
      if (Flag == 11 && U == LOW) { 
        C--;
        curs(C);
        if (C < 0) {
          C=0;
          curs(C);
        }
      }
      if (Flag == 12 && U == LOW) {
        set_C1();
      }
//..............................SELECTTIONE.................. 
//..............................SELECTTIONF.................. 
   
//........................................Menu....................
//.........................phase set.........................

//.........................phase set.........................
//....................................SETTING VIEW MP.............................
    if(statemp==1){
       case1=EEPROM.read(5);
       case2=EEPROM.read(6);
       case3=EEPROM.read(7);
       case4=EEPROM.read(8);
       case5=EEPROM.read(9);
    }
    //..........................................................................
     if(Flag==3&&C==0&&sts==1&&S==LOW){
       case1;
       Set_view1(1);
     }
     if(Flag==3&&C==1&&S==LOW){
       case2;
       Set_view1(2);
     }
     if(Flag==3&&C==2&&S==LOW){
       case3;
       Set_view1(3);
     }  
     if(Flag==3&&C==3&&S==LOW){
       case4;
       Set_view1(4);
     }
     if(Flag==4&&C==4&&sts==1&&S==LOW){
       case5;
       Set_view1(5);
     }

  if(Flag==16&&C==1&&sts==1&&S==LOW){  
      if(Ph1==1){
        Ph1=3;
      }
      else{
        Ph1=1;
      }
      case1=Ph1;
      Set_view1(1);
      EEPROM.write(5,Ph1);
     }
     if(Flag==17&&C==1&&sts==1&&S==LOW){  
      if(Ph2==1){
        Ph2=3;
      }
      else{
        Ph2=1;
      }
      case2=Ph2;
      Set_view1(2);
      EEPROM.write(6,Ph2);
     }
     if(Flag==18&&C==1&&sts==1&&S==LOW){  
      if(Ph3==1){
        Ph3=3;
      }
      else{
        Ph3=1;
      }
      case3=Ph3;
      Set_view1(3);
      EEPROM.write(7,Ph3);
     }
     if(Flag==19&&C==1&&sts==1&&S==LOW){  
      if(Ph4==1){
        Ph4=3;
      }
      else{
        Ph4=1;
      }
      case4=Ph4;
      Set_view1(4);
      EEPROM.write(8,Ph4);
     // Serial.println(case4); 
     }
     if(Flag==20&&C==1&&sts==1&&S==LOW){  
      if(Ph5==1){
        Ph5=3;
      }
      else{
        Ph5=1;
      }
      case5=Ph5;
      Set_view1(5);
      EEPROM.write(9,Ph5);
     }
//....................................SETTING VIEW MP............................. 
//......................Flag5 & Flag6
//....................................SETTING VIEW MR
   if(Flag==5&&C==0&&sts==1&&S==LOW){
    lcd.clear();  
    Set_view2(1);     
   }
   else if(Flag==5&&C==1&&S==LOW){
    lcd.clear();
    Set_view2(2);     
   }
   else if(Flag==5&&C==2&&S==LOW){
    lcd.clear();
    Set_view2(3);     
   }
   else if(Flag==5&&C==3&&S==LOW){
    lcd.clear();
    Set_view2(4);     
   }
   if(Flag==6&&C==4&&sts==1&&S==LOW){
    lcd.clear();
    Set_view2(5);     
   }  
//......................................RAW WATER PUMB....................
     if(Flag==22&&sts==1&&S==LOW){
      
      Sl++;
      if(Sl==1){
        R=4;
      }
      else if(Sl==2){
        R=7;
      }
      else if(Sl==3){
        R=23;
      }
      else if(Sl>3){
        Sl=3;
      }
      Curs_2(R);
     }
    if(Flag==22&&R==4&&U==LOW){
      unit1--;
      if(unit1<0){
        unit1=15;
      }      
      Set_view2(1);
    }

    if(Flag==22&&R==4&&D==LOW){
      unit1++;
      if(unit1>15){
        unit1=0;
      }
      Set_view2(1);
    }
    

    else if(Flag==22&&R==7&&U==LOW){
      unit2--;
    if(unit2<0){
      unit2=9;     
     }
     Set_view2(1);
    }
     

    else if(Flag==22&&R==7&&D==LOW){
      unit2++;
      if(unit2>9){
        unit2=0;
      }
      Set_view2(1);
    }

    float Sense1 =  unit1+ (unit2 / 10.0);
    if(Sense1==9.6){
      
    }
    if(Flag==22&&R==23&&sts==1&&S==LOW){
      EEPROM.write(10,unit1);
      EEPROM.write(12,unit2);
      delay(50);
      set_B1();  
      Sl=0; 
      R=4;      
    }   
//....................................RAW WATER PUMB.............................
//.............AIR BLOWER
     if(Flag==23&&sts==1&&S==LOW){      
      Sl++;
      if(Sl==1){
        R=4;
      }
      else if(Sl==2){
        R=7;
      }
      else if(Sl==3){
        R=23;
      }
      else if(Sl>3){
        Sl=3;
      }
      Curs_2(R);
     }
    if(Flag==23&&R==4&&U==LOW){
      unit3--;
      if(unit3<0){
        unit3=15;
      }      
      Set_view2(2);
    }

    if(Flag==23&&R==4&&D==LOW){
      unit3++;
      if(unit3>15){
        unit3=0;
      }
      Set_view2(2);
    }
    

    else if(Flag==23&&R==7&&U==LOW){
      unit4--;
    if(unit4<0){
      unit4=9;     
     }
     Set_view2(2);
    }
     

    else if(Flag==22&&R==7&&D==LOW){
      unit4++;
      if(unit4>9){
        unit4=0;
      }
      Set_view2(2);
    }

    float Sense2 =  unit3+ (unit4 / 10.0);
    if(Sense2==9.6){
      
    }
    if(Flag==23&&R==23&&sts==1&&S==LOW){
      EEPROM.write(13,unit3);
      EEPROM.write(15,unit4);
      delay(50);
      set_B1();  
      Sl=0; 
      R=4;      
    }
 //.............AIR BLOWER...............   
 //.............RECIRCULATION PUMB
     if(Flag==24&&sts==1&&S==LOW){      
      Sl++;
      if(Sl==1){
        R=4;
      }
      else if(Sl==2){
        R=7;
      }
      else if(Sl==3){
        R=23;
      }
      else if(Sl>3){
        Sl=3;
      }
      Curs_2(R);
     }
    if(Flag==24&&R==4&&U==LOW){
      unit5--;
      if(unit5<0){
        unit5=15;
      }      
      Set_view2(3);
    }

    if(Flag==24&&R==4&&D==LOW){
      unit5++;
      if(unit5>15){
        unit5=0;
      }
      Set_view2(3);
    }
    

    else if(Flag==24&&R==7&&U==LOW){
      unit6--;
    if(unit6<0){
      unit6=9;     
     }
     Set_view2(3);
    }
     

    else if(Flag==24&&R==7&&D==LOW){
      unit6++;
      if(unit6>9){
        unit6=0;
      }
      Set_view2(3);
    }

    float Sense3 =  unit5+ (unit6 / 10.0);
    if(Sense3==9.6){
      
    }
    if(Flag==24&&R==23&&sts==1&&S==LOW){
      EEPROM.write(16,unit5);
      EEPROM.write(18,unit6);
      delay(50);
      set_B1();  
      Sl=0; 
      R=4;      
    }
//............................RECIRCULATION PUMB.............  
//............................FILTER FEED PUMB
     if(Flag==25&&sts==1&&S==LOW){      
      Sl++;
      if(Sl==1){
        R=4;
      }
      else if(Sl==2){
        R=7;
      }
      else if(Sl==3){
        R=23;
      }
      else if(Sl>3){
        Sl=3;
      }
      Curs_2(R);
     }
    if(Flag==25&&R==4&&U==LOW){
      unit7--;
      if(unit7<0){
        unit7=15;
      }      
      Set_view2(4);
    }

    if(Flag==25&&R==4&&D==LOW){
      unit7++;
      if(unit7>15){
        unit7=0;
      }
      Set_view2(4);
    }
    

    else if(Flag==25&&R==7&&U==LOW){
      unit8--;
    if(unit8<0){
      unit8=9;     
     }
     Set_view2(4);
    }
     

    else if(Flag==25&&R==7&&D==LOW){
      unit8++;
      if(unit8>9){
        unit8=0;
      }
      Set_view2(4);
    }

    float Sense4 =  unit7+ (unit8 / 10.0);
    if(Sense4==9.6){
      
    }
    if(Flag==25&&R==23&&sts==1&&S==LOW){
      EEPROM.write(19,unit7);
      EEPROM.write(21,unit8);
      delay(50);
      set_B1();  
      Sl=0; 
      R=4;      
    }
//.....................FILTER FEED PUMB.............  
//.....................TREATED WATER PUMB...........
       if(Flag==26&&sts==1&&S==LOW){      
      Sl++;
      if(Sl==1){
        R=4;
      }
      else if(Sl==2){
        R=7;
      }
      else if(Sl==3){
        R=23;
      }
      else if(Sl>3){
        Sl=3;
      }
      Curs_2(R);
     }
    if(Flag==26&&R==4&&U==LOW){
      unit9--;
      if(unit9<0){
        unit9=15;
      }      
      Set_view2(5);
    }

    if(Flag==26&&R==4&&D==LOW){
      unit9++;
      if(unit9>15){
        unit9=0;
      }
      Set_view2(5);
    }
    

    else if(Flag==26&&R==7&&U==LOW){
      unit10--;
    if(unit10<0){
      unit10=9;     
     }
     Set_view2(5);
    }
     

    else if(Flag==26&&R==7&&D==LOW){
      unit10++;
      if(unit10>9){
        unit10=0;
      }
      Set_view2(5);
    }

    float Sense5 =  unit9+ (unit10 / 10.0);
    if(Sense5==9.6){
      
    }
    if(Flag==26&&R==23&&sts==1&&S==LOW){
      EEPROM.write(22,unit7);
      EEPROM.write(24,unit8);
      delay(50);
      set_B2();  
      Sl=0; 
      R=4;      
    }
//...................TREATED WATER PUMB...........
//............Pumb Mode Flag 7
//..........................RWP
if (Flag == 7 && C == 0 && sts == 1 && S == LOW) {
    lcd.clear();
    C = 1;
    curs(C); 
    if (F1 == 1) {
        modeF1 = "ON";
    } else {
        modeF1 = "OFF";
    }
    if (T1 == 1) {
        modeT1 = "ON"; 
    } else {
        modeT1 = "OFF";
    }
    Set_view3(1);     
}

if (Flag == 7 && C == 1 && S == LOW) {
    lcd.clear();
    C = 1;
    curs(C);
    if (F2 == 1) {
        modeF2 = "ON";
    } else {
        modeF2 = "OFF";
    }
    if (T2 == 1) {
        modeT2 = "ON"; 
    } else {
        modeT2 = "OFF";
    }
    Set_view3(2);     
}

else if (Flag == 7 && C == 2 && S == LOW) {
    lcd.clear();
    C = 1;
    curs(C);
    if (F3 == 1) {
        modeF3 = "ON";
    } else {
        modeF3 = "OFF";
    }
    if (T3 == 1) {
        modeT3 = "ON"; 
    } else {
        modeT3 = "OFF";
    }
    Set_view3(3);     
}

else if (Flag == 7 && C == 3 && S == LOW) {
    lcd.clear();
    C = 1;
    curs(C);    
    if (F4 == 1) {
        modeF4 = "ON";
    } else {
        modeF4 = "OFF";
    }
    if (T4 == 1) {
        modeT4 = "ON"; 
    } else {
        modeT4 = "OFF";
    }
    Set_view3(4);     
}

if (Flag == 8 && C == 4 && sts == 1 && S == LOW) {
    lcd.clear();
    C = 1;
    curs(C);
    if (F5 == 1) {
        modeF5 = "ON";
    } else {
        modeF5 = "OFF";
    }
    if (T5 == 1) {
        modeT5 = "ON"; 
    } else {
        modeT5 = "OFF";
    }
    Set_view3(5);     
}

//............................
 if(Flag==27&&C==1&&D==LOW){
  C=2;
  curs(C);
 }
 if(Flag==27&&C==2&&U==LOW){
  C=1;
  curs(C);
 }
 if(Flag==27&&C==1&&sts==0&&S==LOW){
  if(modeF1=="ON"){
    modeF1="OFF";
    EEPROM.write(0,25);
  }else{
    modeF1="ON";
    EEPROM.write(1,25);
  }
  Set_view3(1);
 }
 if(Flag==27&&C==2&&S==LOW){
  if(modeT1=="ON"){
    modeT1="OFF";
    EEPROM.write(0,26);
  }else{
    modeT1="ON";
    EEPROM.write(1,26);
  }
  Set_view3(1);
 }
if(Flag==28&&C==1&&D==LOW){
  C=2;
  curs(C);
 }
 if(Flag==28&&C==2&&U==LOW){
  C=1;
  curs(C);
 }
 if(Flag==28&&C==1&&sts==0&&S==LOW){
  if(modeF2=="ON"){
    modeF2="OFF";
    EEPROM.write(0,27);
  }else{
    modeF2="ON";
    EEPROM.write(1,27);
  }
  Set_view3(2);
 }
 if(Flag==28&&C==2&&S==LOW){
  if(modeT2=="ON"){
    modeT2="OFF";
    EEPROM.write(0,28);
  }else{
    modeT2="ON";
    EEPROM.write(1,28);
  }
  Set_view3(2);
 }

if(Flag==29&&C==1&&D==LOW){
  C=2;
  curs(C);
 }
 if(Flag==29&&C==2&&U==LOW){
  C=1;
  curs(C);
 }
 if(Flag==29&&C==1&&sts==0&&S==LOW){
  if(modeF3=="ON"){
    modeF3="OFF";
    EEPROM.write(0,29);
  }else{
    modeF3="ON";
    EEPROM.write(1,29);
  }
  Set_view3(3);
 }
 if(Flag==29&&C==2&&S==LOW){
  if(modeT3=="ON"){
    modeT3="OFF";
    EEPROM.write(0,30);
  }else{
    modeT3="ON";
    EEPROM.write(1,30);
  }
  Set_view3(3);
 }

 if(Flag==30&&C==1&&D==LOW){
  C=2;
  curs(C);
 }
 if(Flag==30&&C==2&&U==LOW){
  C=1;
  curs(C);
 }
 if(Flag==30&&C==1&&sts==0&&S==LOW){
  if(modeF4=="ON"){
    modeF4="OFF";
    EEPROM.write(0,31);
  }else{
    modeF4="ON";
    EEPROM.write(1,31);
  }
  Set_view3(4);
 }
 if(Flag==30&&C==2&&S==LOW){
  if(modeT4=="ON"){
    modeT4="OFF";
    EEPROM.write(0,32);
  }else{
    modeT4="ON";
    EEPROM.write(1,32);
  }
  Set_view3(4);
 }

  if(Flag==31&&C==1&&D==LOW){
  C=2;
  curs(C);
 }
 if(Flag==31&&C==2&&U==LOW){
  C=1;
  curs(C);
 }
 if(Flag==31&&C==1&&sts==0&&S==LOW){
  if(modeF5=="ON"){
    modeF5="OFF";
    EEPROM.write(0,33);
  }else{
    modeF5="ON";
    EEPROM.write(1,33);
  }
  Set_view3(4);
 }
 if(Flag==31&&C==2&&S==LOW){
  if(modeT5=="ON"){
     modeT5="OFF";
     EEPROM.write(0,34);
    
  }else{
    modeT5="ON";
    EEPROM.write(1,34);
  }
  Set_view3(5);
 }
  F1= EEPROM.read(25);
 // Serial.println(F1);
//....................................SETTING VIEW PM..............
//....................................SETTING VIEW GENERAL
  if(Flag==2&&C==1&&S==LOW){
   clockD();
  }
//.....................................SETTING VIEW GENERAL.............

//.....................................SETTING VIEW PCO
 if(Flag==9&&C==0&&sts==0&&S==LOW){
   lcd.clear();
   C=1;
   curs(C);
   Set_view4(1);
 }
  if(Flag==9&&C==1&&S==LOW){
    lcd.clear();
    C=1;
    curs(C);
   Set_view4(2);
 }
  if(Flag==9&&C==2&&S==LOW){
   lcd.clear();
   C=1;
   curs(C);
   Set_view4(3);
 }
  if(Flag==9&&C==3&&S==LOW){
    lcd.clear();
    C=1;
    curs(C);
    Set_view4(4);
 }
  if(Flag==10&&C==4&&sts==1&&S==LOW){
    lcd.clear();
    C=1;
    curs(C);
    Set_view4(5);
 }
//.....................................SETTING VIEW PCO RW
  if(Flag==32&&D==LOW){
  ns++;
  if(ns>2){
    ns=0;
  }
  if(ns==0){
    C=1;
    curs(C);
    lcd.setCursor(10,2);
    lcd.print(" ");
  }
  else if(ns==1){
    C=2;
    curs(C);
    lcd.setCursor(10,2);
    lcd.print(" ");
  }
  else if(ns==2){
    C=3;
    lcd.setCursor(0,2);
    lcd.print(" ");
     lcd.setCursor(10,2);
    lcd.print(">");    
  }
}
  if(Flag==32&&U==LOW){
   ns--;
   if(ns<0){
    ns=2;
   }
   if(ns==0){
    C=1;
    curs(C);
    lcd.setCursor(10,2);
    lcd.print(" ");
  }
  else if(ns==1){
    C=2;
    curs(C);
    lcd.setCursor(10,2);
    lcd.print(" ");
  }
  else if(ns==2){
    C=3;
    lcd.setCursor(0,1);
    lcd.print(" ");
    lcd.setCursor(0,2);
    lcd.print(" ");
    lcd.setCursor(10,2);
    lcd.print(">");    
  }
}

if(Flag==32&&C==1&&S==LOW){
  if(PCOT1=="ON"){
     PCOT1="OFF";
  }else{
    PCOT1="ON";
  }
  Set_view4(1);
}

if(Flag==32&&C==2&&S==LOW){
  if(RWPb1=="ON"){
     RWPb1="OFF";
  }else{
    RWPb1="ON";
  }
    Set_view4(1);

}

if(Flag==32&&C==3&&S==LOW){
  if(RWPb2=="ON"){
     RWPb2="OFF";
  }else{
    RWPb2="ON";
  }
    Set_view4(1);
}
//.....................................SETTING VIEW PCO RWP.

//.....................................SETTING VIEW AB
  if(Flag==33&&D==LOW){
  ns++;
  if(ns>2){
    ns=0;
  }
  if(ns==0){
    C=1;
    curs(C);
    lcd.setCursor(10,2);
    lcd.print(" "); 
  }
  else if(ns==1){
    C=2;
    curs(C);
    lcd.setCursor(10,2);
    lcd.print(" ");
  }
  else if(ns==2){
    C=3;
    lcd.setCursor(0,2);
    lcd.print(" ");
     lcd.setCursor(10,2);
    lcd.print(">");    
  }
}
  if(Flag==33&&U==LOW){
   ns--;
   if(ns<0){
    ns=2;
   }
   if(ns==0){
    C=1;
    curs(C);
    lcd.setCursor(10,2);
    lcd.print(" ");
  }
  else if(ns==1){
    C=2;
    curs(C);
    lcd.setCursor(10,2);
    lcd.print(" ");
  }
  else if(ns==2){
    C=3;
    lcd.setCursor(0,1);
    lcd.print(" ");
    lcd.setCursor(0,2);
    lcd.print(" ");
    lcd.setCursor(10,2);
    lcd.print(">");    
  }
}

if(Flag==33&&C==1&&S==LOW){
  if(PCOT2=="ON"){
     PCOT2="OFF";
  }else{
    PCOT2="ON";
  }
   Set_view4(2);

}

if(Flag==33&&C==2&&S==LOW){
  if(RWPb3=="ON"){
     RWPb3="OFF";
  }else{
    RWPb3="ON";
  }
     Set_view4(2);

}

if(Flag==33&&C==3&&S==LOW){
  if(RWPb4=="ON"){
     RWPb4="OFF";
  }else{
    RWPb4="ON";
  }
     Set_view4(2);
}


//.....................................SETTING VIEW RP
 if(Flag==34&&C==1&&sts==1&&S==LOW){
  if(PCOT3=="ON"){
     PCOT3="OFF";
  }else{
    PCOT3="ON";
  }
   Set_view4(3);

  }
 
//.....................................SETTING VIEW FFP
  if(Flag==35&&D==LOW){
  ns++;
  if(ns>1){
    ns=0;
  }
  if(ns==0){
    C=1;
    curs(C);
  }
  else if(ns==1){
    C=2;
    curs(C);
  }
}
  if(Flag==35&&U==LOW){
   ns--;
   if(ns<0){
    ns=1;
   }
   if(ns==0){
    C=1;
    curs(C);
  }
  else if(ns==1){
    C=2;
    curs(C);
  }
}
if(Flag==35&&C==1&&S==LOW){
  if(PCOT4=="ON"){
     PCOT4="OFF";
  }else{
    PCOT4="ON";
  }
   Set_view4(4);

}

if(Flag==35&&C==2&&S==LOW){
  if(RWPb7=="ON"){
     RWPb7="OFF";
  }else{
    RWPb7="ON";
  }
     Set_view4(4);

}

//.....................................SETTING VIEW TWP
    if(Flag==36&&D==LOW){
  ns++;
  if(ns>2){
    ns=0;
  }
  if(ns==0){
    C=1;
    curs(C);
    lcd.setCursor(10,2);
    lcd.print(" ");
  }
  else if(ns==1){
    C=2;
    curs(C);
    lcd.setCursor(10,2);
    lcd.print(" ");
  }
  else if(ns==2){
    C=3;
    lcd.setCursor(0,2);
    lcd.print(" ");
     lcd.setCursor(10,2);
    lcd.print(">");    
  }
}
  if(Flag==36&&U==LOW){
   ns--;
   if(ns<0){
    ns=2;
   }
   if(ns==0){
    C=1;
    curs(C);
    lcd.setCursor(10,2);
    lcd.print(" ");
  }
  else if(ns==1){
    C=2;
    curs(C);
    lcd.setCursor(10,2);
    lcd.print(" ");
  }
  else if(ns==2){
    C=3;
    lcd.setCursor(0,1);
    lcd.print(" ");
    lcd.setCursor(0,2);
    lcd.print(" ");
    lcd.setCursor(10,2);
    lcd.print(">");    
  }
}

if(Flag==36&&C==1&&S==LOW){
  if(PCOT5=="ON"){
     PCOT5="OFF";
  }else{
    PCOT5="ON";
  }
   Set_view4(5);

}

if(Flag==36&&C==2&&S==LOW){
  if(RWPb9=="ON"){
     RWPb9="OFF";
  }else{
    RWPb9="ON";
  }
     Set_view4(5);

}

if(Flag==36&&C==3&&S==LOW){
  if(RWPb10=="ON"){
     RWPb10="OFF";
  }else{
    RWPb10="ON";
  }
     Set_view4(5);
}







//Serial.println(ns);
//....................................SETTING VIEW....................
//.......................................aut man
      if(B==LOW){
         C=0;
         Stated=1;
         A_M1();
      }
      if(D==LOW&&Flag==14){
        C++;
        curs(C);
        if(C>3){
           C=0;
           A_M2();         
        }       
      }
      if(U==LOW&&Flag==14){
        C--;
        if(C<0){
          C=0;
        }
        curs(C);
      }
      if(U==LOW&&C==0&&Flag==15){
        C=3;
        A_M1();
      }
     ///.....................Toggle AU MAN...........
       if(Stated==1){
       int Mod1=EEPROM.read(0);
       int Mod2=EEPROM.read(1);
       int Mod3=EEPROM.read(2);
       int Mod4=EEPROM.read(3);
       int Mod5=EEPROM.read(4);  
        if (Mod1 == 1) {
        Mode1 = "AUT";
       } else {
        Mode1 = "MAN";
       }

       if (Mod2 == 1) {
       Mode2 = "AUT";
       } else {
       Mode2 = "MAN";
       }

       if (Mod3 == 1) {
       Mode3 = "AUT";
       } else {
       Mode3 = "MAN";
       }

       if (Mod4 == 1) {
       Mode4 = "AUT";
       } else {
       Mode4 = "MAN";
       }
      if (Mod5 == 1) {
       Mode5 = "AUT";
       } else {
       Mode5 = "MAN";
       }
       A_M1();
       delay(3000);
       Stated=2;
       //Serial.println(Mode1); 
      }    
     if(Stated==2){
      if(S==LOW&&C==0&&Flag==14){
        if(Mode1=="AUT"){
          Mode1="MAN";
          EDTA1=0;
        }else{
          Mode1="AUT";
          EDTA1=1;
        }
        EEPROM.write(0,EDTA1);
        A_M1();
      }

       if(S==LOW&&C==1&&Flag==14){
        if(Mode2=="AUT"){
          Mode2="MAN";
          EDTA2=0;
        }else{
          Mode2="AUT";
          EDTA2=1;
        }
        EEPROM.write(1,EDTA2);
        A_M1();
      }

       if(S==LOW&&C==2&&Flag==14){
        if(Mode3=="AUT"){
          Mode3="MAN";
          EDTA3=0;
        }else{
          Mode3="AUT";
          EDTA3=1;
        }
        EEPROM.write(2,EDTA3);
        A_M1();
      }

       if(S==LOW&&C==3&&Flag==14){
        if(Mode4=="AUT"){
          Mode4="MAN";
          EDTA4=0;
        }else{
          Mode4="AUT";
          EDTA4=1;
        }
        EEPROM.write(3,EDTA4);
        A_M1();
      }

       if(S==LOW&&C==0&&Flag==15){
        if(Mode5=="AUT"){
          Mode5="MAN";
          EDTA5=0;
        }else{
          Mode5="AUT";
          EDTA5=1;
        }
        EEPROM.write(4,EDTA5);
        A_M2();
      }
     }
     int READ1;
        
//     /////.....................Toggle AU MAN...........
//      
////.......................................aut man.................................
    // Serial.print(Flag,"  ");
    //Serial.println(C);
                                                                            
       if(sts==0){
         sts=1;
        }else{
          sts=0;
        }
      delay(50);
  }
  
void Menu() {
  S_f = 1;
  Flag = 1;
  lcd.clear();
  curs(C); 
  lcd.setCursor(1, 0);
  lcd.print("Motor Phase");
  lcd.setCursor(1, 1);
  lcd.print("Motor Rating");
  lcd.setCursor(1, 2);
  lcd.print("Pump Mode");
  lcd.setCursor(1, 3);
  lcd.print("Timer");
}

void Menu2() {
  Flag = 2;
  curs(C);
  lcd.clear();
  lcd.setCursor(1, 0);
  lcd.print("Pump Changeover");
  lcd.setCursor(1, 1);
  lcd.print("General");
}

void set_A1() {
  Flag = 3;
  lcd.clear();
  curs(C);
  lcd.setCursor(1, 0);
  lcd.print("Raw Water Pump");
  lcd.setCursor(1, 1);
  lcd.print("Air Blower");
  lcd.setCursor(1, 2);
  lcd.print("Recirculation Pump");
  lcd.setCursor(1, 3);
  lcd.print("Filter Feed Pump");
}

void set_A2() {
  Flag = 4;
  lcd.clear();
  curs(C);
  lcd.setCursor(1, 0);
  lcd.print("Treated Water Pump");
}

void set_B1() {
  Flag = 5;
  lcd.clear();
  curs(C);
  lcd.setCursor(1, 0);
  lcd.print("Raw Water Pump");
  lcd.setCursor(1, 1);
  lcd.print("Air Blower");
  lcd.setCursor(1, 2);
  lcd.print("Recirculation Pump");
  lcd.setCursor(1, 3);
  lcd.print("Filter Feed Pump");
}

void set_B2() {
  Flag = 6;
  lcd.clear();
  curs(0);
  lcd.setCursor(1, 0);
  lcd.print("Treated Water Pump");
}

void set_C1() {
  Flag = 7;
  lcd.clear();
  curs(C);
  lcd.setCursor(1, 0);
  lcd.print("Raw Water Pump");
  lcd.setCursor(1, 1);
  lcd.print("Air Blower");
  lcd.setCursor(1, 2);
  lcd.print("Recirculation Pump");
  lcd.setCursor(1, 3);
  lcd.print("Filter Feed Pump");
}

void set_C2() {
  Flag = 8;
  lcd.clear();
  curs(C);
  lcd.setCursor(1, 0);
  lcd.print("Treated Water Pump");
}

void set_D1() {
  Flag = 9;
  lcd.clear();
  curs(C);
  lcd.setCursor(1, 0);
  lcd.print("Raw Water Pump");
  lcd.setCursor(1, 1);
  lcd.print("Air Blower");
  lcd.setCursor(1, 2);
  lcd.print("Recirculation Pump");
  lcd.setCursor(1, 3);
  lcd.print("Filter Feed Pump");
}

void set_D2() {
  Flag = 10;
  lcd.clear();
  curs(C);
  lcd.setCursor(1, 0);
  lcd.print("Treated Water Pump");
}

void A_M1(){
   Flag = 14;
   lcd.clear();
   curs(C);
   lcd.setCursor(1, 0);
   lcd.print("R.W.P : ");
   lcd.setCursor(8, 0);
   lcd.print(Mode1);
   lcd.setCursor(1, 1);
   lcd.print("AR.BL : ");
   lcd.setCursor(8, 1);
   lcd.print(Mode2);
   lcd.setCursor(1, 2);
   lcd.print("R.C.P : ");
   lcd.setCursor(8, 2);
   lcd.print(Mode3);
   lcd.setCursor(1, 3);
   lcd.print("F.F.P : ");
   lcd.setCursor(8, 3);
   lcd.print(Mode4); 
}
void A_M2(){
   Flag = 15;
   lcd.clear();
   curs(C);
   lcd.setCursor(1, 0);
   lcd.print("T.W.P : ");
   lcd.setCursor(8, 0);
   lcd.print(Mode5);
   
}
void Set_view1(int n){
  Flag = 13;
  lcd.clear();
  C=1;
  curs(C);
  if(n==1){
    Flag=16;
    lcd.setCursor(1, 0);
    lcd.print("  RAW WATER PUMB");
    lcd.setCursor(2, 1);
    lcd.print(case1);
    lcd.setCursor(4, 1);
    lcd.print("PHASE");
    lcd.setCursor(1, 3);
    lcd.print("  [SELECT PHASE]");
  }
  if(n==2){
    Flag=17;
    lcd.setCursor(1, 0);
    lcd.print("  AIR BLOWER");
    lcd.setCursor(2, 1);
    lcd.print(case2);
    lcd.setCursor(4, 1);
    lcd.print("PHASE");
    lcd.setCursor(1, 3);
    lcd.print("  [SELECT PHASE]");
  }
  if(n==3){
    Flag=18;
    lcd.setCursor(1, 0);
    lcd.print(" RECIRCULATION PUMB");
    lcd.setCursor(2, 1);
    lcd.print(case3);
    lcd.setCursor(4, 1);
    lcd.print("PHASE");
    lcd.setCursor(1, 3);
    lcd.print("  [SELECT PHASE]");
  }
  if(n==4){
    Flag=19;
    lcd.setCursor(1, 0);
    lcd.print(" FILTER FEED PUMB");
    lcd.setCursor(2, 1);
    lcd.print(case4);
    lcd.setCursor(4, 1);
    lcd.print("PHASE");
    lcd.setCursor(1, 3);
    lcd.print("  [SELECT PHASE]");
  }
  if(n==5){
    Flag=20;
    lcd.setCursor(0, 0);
    lcd.print(" TREATED WATER PUMB");
    lcd.setCursor(2, 1);
    lcd.print(case5);
    lcd.setCursor(4, 1);
    lcd.print("PHASE");
    lcd.setCursor(1, 3);
    lcd.print("  [SELECT PHASE]");
  }
}
void Set_view2(int o) {
    Flag = 21; 
    if (o == 1) {
        Flag = 22;
        lcd.setCursor(1, 0);
        lcd.print("  RAW WATER PUMB");    
        lcd.setCursor(4, 1);
        lcd.print(unit1);
        if(unit1<10){
           lcd.setCursor(5, 1); 
           lcd.print(" ");
        }
        lcd.setCursor(6, 1);
        lcd.print(".");
        lcd.setCursor(7, 1);
        lcd.print(unit2);
        lcd.setCursor(8, 1);
        lcd.print(" ");    
        lcd.setCursor(9, 1);
        lcd.print("Amps");   
        lcd.setCursor(1, 3);
        lcd.print("[Select to Feed]");
    }

    if (o == 2) {
        Flag = 23;
        lcd.setCursor(1, 0);
        lcd.print("  AIR BLOWER");
        lcd.setCursor(4, 1);
        lcd.print(unit3);
        if (unit3 < 10) {
            lcd.print(" ");
        }
        lcd.setCursor(6, 1);
        lcd.print(".");
        lcd.setCursor(7, 1);
        lcd.print(unit4); 
        lcd.setCursor(8, 1);
        lcd.print(" ");   
        lcd.setCursor(9, 1);
        lcd.print("Amps");   
        lcd.setCursor(1, 3);
        lcd.print("[Select to Feed]");
    }

    if (o == 3) {
        Flag = 24;
        lcd.setCursor(1, 0);
        lcd.print(" RECIRCULATION PUMB");    
        lcd.setCursor(4, 1);
        lcd.print(unit5);
        if (unit5 < 10) {
            lcd.print(" ");
        }
        lcd.setCursor(6, 1);
        lcd.print(".");
        lcd.setCursor(7, 1);
        lcd.print(unit6);
        lcd.setCursor(8, 1);
        lcd.print(" ");    
        lcd.setCursor(9, 1);
        lcd.print("Amps");   
        lcd.setCursor(1, 3);
        lcd.print("[Select to Feed]");
    }

    if (o == 4) {
        Flag = 25;
        lcd.setCursor(1, 0);
        lcd.print(" FILTER FEED PUMB");  
        lcd.setCursor(4, 1);
        lcd.print(unit7);
        if (unit7 < 10) {
            lcd.print(" ");
        }
        lcd.setCursor(6, 1);
        lcd.print(".");
        lcd.setCursor(7, 1);
        lcd.print(unit8);
        lcd.setCursor(8, 1);
        lcd.print(" ");    
        lcd.setCursor(9, 1);
        lcd.print("Amps");   
        lcd.setCursor(1, 3);
        lcd.print("[Select to Feed]");
    }

    if (o == 5) {
        Flag = 26;
        lcd.setCursor(1, 0);
        lcd.print(" TREATED WATER PUMB");    
        lcd.setCursor(4, 1);
        lcd.print(unit9);
        if (unit9 < 10) {
            lcd.print("  ");
        }
       lcd.setCursor(6, 1);
        lcd.print(".");
        lcd.setCursor(7, 1);
        lcd.print(unit10);
        lcd.setCursor(8, 1);
        lcd.print(" ");    
        lcd.setCursor(9, 1);
        lcd.print("Amps");   
        lcd.setCursor(1, 3);
        lcd.print("[Select to Feed]");
    }
}
void Set_view3(int m){
  if(m==1){
  Flag=27;  
  lcd.setCursor(1, 0);
  lcd.print("RAW WATER PUMB");
  lcd.setCursor(2, 1);
  lcd.print("Float: ");
  lcd.setCursor(9,1);
  lcd.print(modeF1);
  if(modeF1=="ON"){
     lcd.setCursor(11,1);
     lcd.print(" ");
  }
  lcd.setCursor(2, 2);
  lcd.print("Timer: ");
  lcd.setCursor(9,2);
  lcd.print(modeT1);
  if(modeT1=="ON"){
     lcd.setCursor(11,2);
     lcd.print(" ");
  }
  lcd.setCursor(2,3);
  lcd.print("[SELECT TO FEED]");
  }
  if(m==2){
  Flag=28;
  lcd.setCursor(1, 0);
  lcd.print("RAW WATER PUMB");
  lcd.setCursor(2, 1);
  lcd.print("Float: ");
  lcd.setCursor(9,1);
  lcd.print(modeF2);
  if(modeF2=="ON"){
     lcd.setCursor(11,1);
     lcd.print(" ");
  }
  lcd.setCursor(2, 2);
  lcd.print("Timer: ");
  lcd.setCursor(9,2);
  lcd.print(modeT2);
  if(modeT2=="ON"){
     lcd.setCursor(11,2);
     lcd.print(" ");
  }
  lcd.setCursor(1,3);
  lcd.print("SELECT TO FEED");
  }
  if(m==3){
  Flag=29;  
  lcd.setCursor(1, 0);
  lcd.print("RAW WATER PUMB");
  lcd.setCursor(2, 1);
  lcd.print("Float: ");
  lcd.setCursor(9,1);
  lcd.print(modeF3);
  if(modeF3=="ON"){
     lcd.setCursor(11,1);
     lcd.print(" ");
  }
  lcd.setCursor(2, 2);
  lcd.print("Timer: ");
  lcd.setCursor(9,2);
  lcd.print(modeT3);
  if(modeT3=="ON"){
     lcd.setCursor(11,2);
     lcd.print(" ");
  }
  lcd.setCursor(1,3);
  lcd.print("SELECT TO FEED");
  }
  if(m==4){
  Flag=30;  
  lcd.setCursor(1, 0);
  lcd.print("RAW WATER PUMB");
  lcd.setCursor(2, 1);
  lcd.print("Float: ");
  lcd.setCursor(9,1);
  lcd.print(modeF4);
  if(modeF4=="ON"){
     lcd.setCursor(11,1);
     lcd.print(" ");
  }
  lcd.setCursor(2, 2);
  lcd.print("Timer: ");
  lcd.setCursor(9,2);
  lcd.print(modeT4);
  if(modeT4=="ON"){
     lcd.setCursor(11,2);
     lcd.print(" ");
  }
  lcd.setCursor(1,3);
  lcd.print("SELECT TO FEED");
  }
  if(m==5){
  Flag=31;
  lcd.setCursor(1, 0);
  lcd.print("RAW WATER PUMB");
  lcd.setCursor(2, 1);
  lcd.print("Float: ");
  lcd.setCursor(9,1);
  lcd.print(modeF5);
  if(modeF5=="ON"){
     lcd.setCursor(11,1);
     lcd.print(" ");
  }
  lcd.setCursor(2, 2);
  lcd.print("Timer: ");
  lcd.setCursor(9,2);
  lcd.print(modeT5);
  if(modeT5=="ON"){
     lcd.setCursor(11,2);
     lcd.print(" ");
  }
  lcd.setCursor(1,3);
  lcd.print("SELECT TO FEED");
  }
}

void Set_view4(int p){
  if(p==1){
  Flag=32;  
  lcd.setCursor(1, 0);
  lcd.print("  RAW WATER PUMB"); 
  lcd.setCursor(1, 1);
  lcd.print("Toggle:");
  lcd.setCursor(9,1);
  lcd.print(PCOT1);
  if(PCOT1=="ON"){
     lcd.setCursor(11,1);
     lcd.print(" ");
  }
  lcd.setCursor(1, 2);
  lcd.print("Pb1:");
  lcd.setCursor(6,2);
  lcd.print(RWPb1);
  if(RWPb1=="ON"){
     lcd.setCursor(8,2);
     lcd.print(" ");
  }
  lcd.setCursor(11, 2);
  lcd.print("Pb2:");
  lcd.setCursor(16,2);
  lcd.print(RWPb2);
  if(RWPb2=="ON"){
     lcd.setCursor(18,2);
     lcd.print(" ");
  }
  lcd.setCursor(2,3);
  lcd.print("[SELECT TO FEED]");
  }


    if(p==2){
  Flag=33;  
  lcd.setCursor(1, 0);
  lcd.print("  AIR BLOWER");
  lcd.setCursor(1, 1);
  lcd.print("Toggle:");
  lcd.setCursor(9,1);
  lcd.print(PCOT2);
  if(PCOT2=="ON"){
     lcd.setCursor(11,1);
     lcd.print(" ");
  }
  lcd.setCursor(1, 2);
  lcd.print("Pb1:");
  lcd.setCursor(6,2);
  lcd.print(RWPb3);
  if(RWPb3=="ON"){
     lcd.setCursor(8,2);
     lcd.print(" ");
  }
  lcd.setCursor(11, 2);
  lcd.print("Pb2:");
  lcd.setCursor(16,2);
  lcd.print(RWPb4);
  if(RWPb4=="ON"){
     lcd.setCursor(18,2);
     lcd.print(" ");
  }
  lcd.setCursor(2,3);
  lcd.print("[SELECT TO FEED]");
  }



  if(p==3){
  Flag=34;  
  lcd.setCursor(1, 0);
  lcd.print(" RECIRCULATION PUMB");  
  lcd.setCursor(1, 1);
  lcd.print("State: ");
  lcd.setCursor(8,1);
  lcd.print(PCOT3);
  if(PCOT3=="ON"){
     lcd.setCursor(10,1);
     lcd.print(" ");
  }
  lcd.setCursor(2,3);
  lcd.print("[SELECT TO FEED]");
  }

  if(p==4){
  Flag=35;  
  lcd.setCursor(1, 0);
  lcd.print(" FILTER FEED PUMB");
  lcd.setCursor(1, 1);
  lcd.print("Toggle:");
  lcd.setCursor(9,1);
  lcd.print(PCOT4);
  if(PCOT4=="ON"){
     lcd.setCursor(11,1);
     lcd.print(" ");
  }
  lcd.setCursor(1, 2);
  lcd.print("State : ");
  lcd.setCursor(9,2);
  lcd.print(RWPb7);
  if(RWPb7=="ON"){
     lcd.setCursor(11,2);
     lcd.print(" ");
  }
  lcd.setCursor(2,3);
  lcd.print("[SELECT TO FEED]");
  }
  
  if(p==5){
  Flag=36;  
  lcd.setCursor(1, 0);
  lcd.print(" TREATED WATER PUMB");  
  lcd.setCursor(1, 1);
  lcd.print("Toggle:");
  lcd.setCursor(9,1);
  lcd.print(PCOT5);
  if(PCOT5=="ON"){
     lcd.setCursor(11,1);
     lcd.print(" ");
  }
  lcd.setCursor(1, 2);
  lcd.print("Pb1:");
  lcd.setCursor(6,2);
  lcd.print(RWPb9);
  if(RWPb9=="ON"){
     lcd.setCursor(8,2);
     lcd.print(" ");
  }
  lcd.setCursor(11, 2);
  lcd.print("Pb2:");
  lcd.setCursor(16,2);
  lcd.print(RWPb10);
  if(RWPb10=="ON"){
     lcd.setCursor(18,2);
     lcd.print(" ");
  }
  lcd.setCursor(2,3);
  lcd.print("[SELECT TO FEED]");
  }
}
 
void clockD(){
    lcd.clear();
    lcd.setCursor(1, 1);
    lcd.print("TIME :");
    lcd.setCursor(1, 2);
    lcd.print("DATE : ");
}
void curs(int c) {
  for (int i = 0; i < 4; i++) {
    lcd.setCursor(0, i);
    lcd.print(" ");
  }  
  lcd.setCursor(0, c);
  lcd.print(">");
}
void Curs_2(int r) {
    lcd.setCursor(0, 2);
    lcd.print("                    ");
    lcd.setCursor(r, 2);
    lcd.print("^");
}//clock]
