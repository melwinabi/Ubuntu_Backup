#include <Wire.h> 
#include <LiquidCrystal_I2C.h>
#include <FastLED.h>

const int RL1  =  32; // 58
const int RL2  =  31; // 59
const int RL3  =  30; // 60
const int RL4  =  39; // 70
const int RL5  =  29; // 71
const int RL6  =  28; // 72
const int RL7  =  27; // 73
const int RL8  =  26; // 74
const int RL9  =  25; // 75
const int RL10 =  24; // 76
const int RL11 =  23; // 77
const int RL12 =  22; // 78

int a =0;
#define LED_PIN     37
#define NUM_LEDS    31
CRGB leds[NUM_LEDS];
LiquidCrystal_I2C lcd(0x27, 20, 4); 
void setup()
{
  pinMode(RL1 , OUTPUT);
  pinMode(RL2 , OUTPUT);
  pinMode(RL3 , OUTPUT);
  pinMode(RL4 , OUTPUT);
  pinMode(RL5 , OUTPUT);
  pinMode(RL6 , OUTPUT);
  pinMode(RL7 , OUTPUT);
  pinMode(RL8 , OUTPUT);
  pinMode(RL9 , OUTPUT);
  pinMode(RL10, OUTPUT);
  pinMode(RL11, OUTPUT);
  pinMode(RL12, OUTPUT);
  FastLED.addLeds<WS2812, LED_PIN, GRB>(leds, NUM_LEDS);
  lcd.init();                      // initialize the lcd 
  // Print a message to the LCD.
  lcd.backlight();
  lcd.setCursor(3,0);
  lcd.print("PPV TECHNOLOGY");
  lcd.setCursor(2,1);
  lcd.print("PRIVATE  LIMITED");
   lcd.setCursor(1,2);
  lcd.print("CHENNAI - 600  077");
   lcd.setCursor(2,3);
  lcd.print("www.ppvgroups.org");
}


void loop()
{
  digitalWrite(RL1, HIGH);
  digitalWrite(RL2, HIGH);
  digitalWrite(RL3, HIGH);
  digitalWrite(RL4, HIGH);
  digitalWrite(RL5, HIGH);
  leds[0] = CRGB(255, 0, 0);
    leds[20] = CRGB(255, 0, 0);
    leds[21] = CRGB(255, 0, 0);
    FastLED.show();
    delay(50);

    leds[0] = CRGB(0, 255, 0);
    leds[20] = CRGB(0, 255, 0);
    leds[21] = CRGB(0, 255, 0);
    FastLED.show();
    delay(10);
    //*********************************** 
    leds[1] = CRGB(255, 0, 0);
    leds[19] = CRGB(255, 0, 0);
    leds[22] = CRGB(255, 0, 0);
    FastLED.show();
    delay(50);

    leds[1] = CRGB(0, 255, 0);
    leds[19] = CRGB(0, 255, 0);
    leds[22] = CRGB(0, 255, 0);
    FastLED.show();
    delay(10);
    //*********************************** 
    leds[2] = CRGB(255, 0, 0);
    leds[18] = CRGB(255, 0, 0);
    leds[23] = CRGB(255, 0, 0);
    FastLED.show();
    delay(50);

    leds[2] = CRGB(0, 0, 255);
    leds[18] = CRGB(0, 0, 255);
    leds[23] = CRGB(0, 0, 255 );
    FastLED.show();
    delay(10);
    //*********************************** 
    leds[3] = CRGB(255, 0, 0);
    leds[17] = CRGB(255, 0, 0);
    leds[24] = CRGB(255, 0, 0);
    FastLED.show();
    delay(50);

    leds[3] = CRGB(0, 255, 0);
    leds[17] = CRGB(0, 255, 0);
    leds[24] = CRGB(0, 255, 0);
    FastLED.show();
    delay(10);
    //*********************************** 
    leds[4] = CRGB(255, 0, 0);
    leds[16] = CRGB(255, 0, 0);
    leds[25] = CRGB(255, 0, 0);
    FastLED.show();
    delay(50);

    leds[4] = CRGB(0, 255, 0);
    leds[16] = CRGB(0, 255, 0);
    leds[25] = CRGB(0, 255, 0);
    FastLED.show();
    delay(10);
    //*********************************** 
    leds[5] = CRGB(255, 0, 0);
    leds[15] = CRGB(255, 0, 0);
    leds[26] = CRGB(255, 0, 0);
    FastLED.show();
    delay(50);

    leds[5] = CRGB(0, 0, 255);
    leds[15] = CRGB(0, 0, 255);
    leds[26] = CRGB(0, 0, 255);
    FastLED.show();
    delay(10);
  digitalWrite(RL1, LOW);
  digitalWrite(RL2, LOW);
  digitalWrite(RL3, LOW);
  digitalWrite(RL4, LOW);
  digitalWrite(RL5, LOW);
      //*********************************** 
    leds[6] = CRGB(255, 0, 0);
    leds[14] = CRGB(255, 0, 0);
    leds[27] = CRGB(255, 0, 0);
    FastLED.show();
    delay(50);

    leds[6] = CRGB(0, 0, 255);
    leds[14] = CRGB(0, 0, 255);
    leds[27] = CRGB(0, 0, 255);
    FastLED.show();
    delay(10);
    //*********************************** 
    leds[7] = CRGB(255, 0, 0);
    leds[13] = CRGB(255, 0, 0);
    leds[28] = CRGB(255, 0, 0);
    FastLED.show();
    delay(50);

    leds[7] = CRGB(0, 0, 255);
    leds[13] = CRGB(0, 0, 255);
    leds[28] = CRGB(0, 0, 255);
    FastLED.show();
    delay(10);
    //*********************************** 
    leds[8] = CRGB(255, 0, 0);
    leds[12] = CRGB(255, 0, 0);
    leds[29] = CRGB(255, 0, 0);
    FastLED.show();
    delay(50);

    leds[8] = CRGB(0, 0, 255);
    leds[12] = CRGB(0, 0, 255);
    leds[29] = CRGB(0, 0, 255);
    FastLED.show();
    delay(10);
    //*********************************** 
    leds[9] = CRGB(255, 0, 0);
    leds[11] = CRGB(255, 0, 0);
    leds[30] = CRGB(255, 0, 0);
    FastLED.show();
    delay(50);

    leds[9] = CRGB(0, 0, 255);
    leds[11] = CRGB(0, 0, 255);
    leds[30] = CRGB(0, 0, 255);
    FastLED.show();
    delay(10); 
    leds[10] = CRGB(0, 255, 0);
    FastLED.show();
    delay(20);
    leds[10] = CRGB(0, 0, 255);
    FastLED.show();
    delay(20);
  lcd.setCursor(0,3);
  lcd.print("                       ");
  lcd.setCursor(10,3);
  lcd.print(a);
  a = a+1;
}
