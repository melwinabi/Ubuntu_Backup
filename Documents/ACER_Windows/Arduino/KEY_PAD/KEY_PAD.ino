#include <Keypad.h>

const byte ROWS = 4;
const byte COLS = 4;
char hexaKeys[ROWS][COLS] = 
                      {
                        {'0','1','2','3'},
                        {'4','5','6','7'},
                        {'8','9','A','B'},
                        {'C','D','E','F'}
                      };
byte rowPins[ROWS] =  {41, 42, 44, 46};
byte colPins[COLS] =  {45, 43, 38, 40};

Keypad customKeypad = Keypad( makeKeymap(hexaKeys), rowPins, colPins, ROWS, COLS); 

void setup()
{
Serial.begin(9600);
}

void loop()
{
  char customKey = customKeypad.getKey();
  if (customKey)
  {
    Serial.println(customKey);
  }
}
