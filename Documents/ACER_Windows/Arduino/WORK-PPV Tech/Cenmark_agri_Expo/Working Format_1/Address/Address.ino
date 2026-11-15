#include <EEPROM.h>

int StringLocation_bf;
int StringLocation_af;
String weight_bf;
String weight_af;
String Read;
String Step01,Step02;
String wa;
String First_ID;
int d = 0;
int mem = 0;
int k = 0;
int l=0;
String Txt;
char rd_1[11],rd_2[11],rd_3[11];
String Address_1, Address_2, Address_3;
char c;
String readString;

void LoadData();
void setup() 
{
  Serial.begin(9600);
}

void loop() 
{
  
  if (Serial.available() >0) 
  {
    char c = Serial.read();
    Txt += String(c);
    if (c == ';') 
    {
      StringLocation_bf = Read.indexOf('*');
      weight_bf = Read.substring(0, StringLocation_bf);
      StringLocation_af = Read.indexOf('*',StringLocation_bf+1);
      weight_af = Read.substring(StringLocation_bf+1, StringLocation_af);
      Serial.print("Data : ");
      Serial.println(Read);
      Serial.print("K Value : ");
      Serial.println(k);
      Read = "";
      k = 0;
      d = 1;
      mem = 0;
    }
    else if(c == '#')
    { 
      mem = 1;
      k = 0;
    }
     else if (c == '$')
    {
      Serial.println("Address Loading ...");
      LoadData(); 
      Serial.print(Address_1+','+Address_2+','+Address_3);

    }else if(c=='@'){
       Set_mode();
    }
    else
    {
      Read += c;
      if(mem == 1)
      {
        EEPROM.write(k, c);
        k++;
        Read = "";
      }
    }
    }
     
  }
  

  

void LoadData()
{
  for (int j = 0; j<10; j++)
  {
    rd_1 [j] = EEPROM.read(j);
  } 
  rd_1[10] = '\0';
  Address_1 = String(rd_1);
  
  for (int h = 0; h<10; h++)
  {
    rd_2 [h] = EEPROM.read(h+10);
  }
  rd_2[10] = '\0';
  Address_2 = String(rd_2);
  
  for (int o = 0; o<10; o++)
  {
    rd_3 [o] = EEPROM.read(o+20);
  }
  rd_3[10] = '\0';
  Address_3 = String(rd_3);
}
void Set_mode(){
//  Serial.print(Txt);
}
