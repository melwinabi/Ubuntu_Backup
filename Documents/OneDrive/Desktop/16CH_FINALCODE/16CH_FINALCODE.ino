#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <EEPROM.h>
LiquidCrystal_I2C lcd(0x27, 20, 4);
// CD4067 pin assignments
const int cd4067SigPin = A0;  // CD4067 signal pin connected to A0
const int cd4067S0 = 5;       // CD4067 control pin S0
const int cd4067S1 = 4;       // CD4067 control pin S1
const int cd4067S2 = 3;       // CD4067 control pin S2
const int cd4067S3 = 2;       // CD4067 control pin S3
// Rotary encoder pins
const int clkPin = 6; // Connected to CLK
const int dtPin = 7;  // Connected to DATA
const int swPin = 8;  // Connected to SW (push button)
const int buzzerPin = 9;
const int plcrelay = 10;
int sensorValues[16] = {0}; 
int sensorPins[16] = {0};
int lastClkState = LOW;
int currentSelection = 0; 
int selectedSensor = 0;
int page = 0;
int ClickBreak = 1;
bool showSaveMessage = false;
bool settingMode = false;
const unsigned long LONG_PRESS_DURATION = 3000;
unsigned long buttonPressStart = 0;
bool buttonPressed = false;
bool buzzerTriggered = false;
byte CusArrow[] = {B11000,B11100,B11110,B11111,B11111,B11110,B11100,B11000};
byte Alarm[] = {B00100,B01110,B01110,B01110,B11111,B00000,B00100,B00000};
unsigned long lastDebounceTime = 0;
const unsigned long debounceDelay = 50;
//****************************************************************************************************************************
void setup() 
{ 
  pinMode(cd4067S0, OUTPUT);
  pinMode(cd4067S1, OUTPUT);
  pinMode(cd4067S2, OUTPUT);
  pinMode(cd4067S3, OUTPUT);
  pinMode(clkPin, INPUT);
  pinMode(dtPin, INPUT);
  pinMode(swPin, INPUT_PULLUP);
  pinMode(buzzerPin, OUTPUT);
  pinMode(plcrelay, OUTPUT); 
  lcd.init();
  lcd.backlight();
  lcd.clear();
  lcd.createChar(0, CusArrow);
  lcd.createChar(1, Alarm); 
  showWelcomeScreen();
  loadSensorValuesFromEEPROM();
  delay(2000);
  Serial.begin(9600);
  Serial.println("Begin");
}
//****************************************************************************************************************************
void loop() {
  if (settingMode) {
    handleSensorSetting();
  } else {
    displayAnalogValues();
    checkButtonPress();
    checkBuzzerReset();
  }
}
//****************************************************************************************************************************
void checkButtonPress() {
  // Read button state
  if (digitalRead(swPin) == LOW) {
    // Button is pressed
    if (!buttonPressed) {
      buttonPressStart = millis();
      buttonPressed = true;
    } else if (millis() - buttonPressStart >= LONG_PRESS_DURATION) {
      // Long press detected (enter condition)
      if (!settingMode) {
        enterSettingMode();
        delay(500); // Brief delay to prevent immediate re-trigger
      }
      buttonPressed = false; // Reset after long press
    }
  } else {
    // Button is released
    if (buttonPressed && millis() - buttonPressStart < LONG_PRESS_DURATION) {
      // Short press detected (exit condition)
      if (settingMode && page == 3 && selectedSensor == 15) {
        exitSettingMode();
      }
      buttonPressed = false; // Reset after short press
    }
  }
}
//****************************************************************************************************************************
void enterSettingMode() {
  settingMode = true;
  lcd.clear();
  lcd.setCursor(2, 1);
  lcd.print("Entering Setting");
  lcd.setCursor(7, 2);
  lcd.print("Mode...");
  delay(2000); // Display message for 2 seconds
  lcd.clear();
  displayValues();
  displayArrow();
}
//****************************************************************************************************************************
void handleSensorSetting() {
  int clkState = digitalRead(clkPin);
  int dt = digitalRead(dtPin);

  if (clkState == 0 && dt == 1) 
  {
    Serial.print("Call | ");
    Serial.print("ClkState : ");
    Serial.print(clkState);
    Serial.print(" | dt : ");
    Serial.print(dt);
    Serial.print(" | LastState : ");
    Serial.println(lastClkState);

    if (currentSelection == 0) {
      selectedSensor = (selectedSensor + 1) % 16;
      page = selectedSensor / 4;
      Serial.print("1 Page : ");
      Serial.print(page);
      Serial.print("Selected : ");
      Serial.print(selectedSensor);
      Serial.print("Current Selection : ");
      Serial.println(currentSelection);
    } else {
      adjustSensorValue(1);
    }
    constrainValues();
    displayValues();
    displayArrow();
  }
  else if (clkState == 1 && dt == 0)
  {
    if (currentSelection == 0) {
      selectedSensor = (selectedSensor + 15) % 16;
      page = selectedSensor / 4;
      Serial.print("2 Page : ");
      Serial.print(page);
      Serial.print("Selected : ");
      Serial.print(selectedSensor);
      Serial.print("Current Selection : ");
      Serial.println(currentSelection);
    } else {
      adjustSensorValue(-1);
    }
    constrainValues();
    displayValues();
    displayArrow();
  }

  if (digitalRead(swPin) == LOW) {
    delay(200);
    if (currentSelection == 1) {
      showSaveMessage = true;
      lcd.clear();
      lcd.setCursor(1, 1);
      lcd.print("New Value Updated");
      lcd.setCursor(3, 2);
      lcd.print("for Sensor ");
      lcd.print(selectedSensor + 1);
      delay(1000);
      showSaveMessage = false;
    }
    currentSelection = !currentSelection;
    displayValues();
    displayArrow();
  }

  if (digitalRead(swPin) == LOW && page == 3 && selectedSensor == 15) {
    if (!buttonPressed) {
      buttonPressStart = millis();
      buttonPressed = true;
    } else if (millis() - buttonPressStart >= LONG_PRESS_DURATION) {
      exitSettingMode();
      buttonPressed = false;
    }
  } else {
    if (buttonPressed && millis() - buttonPressStart < LONG_PRESS_DURATION) {
      buttonPressed = false;
    }
  }
}
//***********************************************************************************
void exitSettingMode() 
{
  settingMode = false;
  lcd.clear();
  lcd.setCursor(3, 1);
  lcd.print("Exiting Setting");
  lcd.setCursor(7, 2);
  lcd.print("Mode...");
  delay(2000); // Display message for 2 seconds
  lcd.clear();
}
//***********************************************************************************
void showWelcomeScreen() 
{
  lcd.clear();
  lcd.setCursor(6, 0);
  lcd.print("Welcome");
  lcd.setCursor(4, 1);
  lcd.print("Voyager Marine");
  lcd.setCursor(0, 2);
  lcd.print("Services (I) Pvt.Ltd");
  lcd.setCursor(4, 3);
  lcd.print("Chennai-600001");
}
//***********************************************************************************
void adjustSensorValue(int direction) 
{
  sensorValues[selectedSensor] += direction;  // Adjust the currently selected sensor
  Serial.print("direction : ");
  Serial.println(direction);
}
//***********************************************************************************
void constrainValues() {
  for (int i = 0; i < 16; i++) {
    sensorValues[i] = constrain(sensorValues[i], 0, 100);
    EEPROM.write(i, sensorValues[i]); // Write each threshold to EEPROM
  }
}
//***********************************************************************************
void displayValues() {
  if (showSaveMessage) {
    return; // Do not update the sensor values if the save message is being displayed
  }
  lcd.clear();  // Clear the screen before updating
  char sensorLabel[5];
  for (int i = 0; i < 4; i++) {
    int sensorIndex = page * 4 + i; // Calculate the sensor index (0-15)
    sprintf(sensorLabel, "%02d:", sensorIndex + 1);
    lcd.setCursor(1, i);
    lcd.print("Sensor");
    lcd.print(sensorLabel);
    lcd.print(sensorValues[sensorIndex]);
    lcd.print("%   "); // Clear leftover characters
  }
  if (page == 3 && selectedSensor == 15) {
    lcd.setCursor(16, 0);
    lcd.print("Long");
    lcd.setCursor(15, 1);
    lcd.print("Press");
    lcd.setCursor(17, 2);
    lcd.print("to");
    lcd.setCursor(16, 3);
    lcd.print("Exit");
  }
}
//***********************************************************************************
void displayArrow() 
{
  for (int i = 0; i < 4; i++) 
   {
    lcd.setCursor(0, i);
    }
  // Display the arrow to indicate selection
  lcd.setCursor(0, selectedSensor % 4);
  lcd.write(byte(0));
}
//***********************************************************************************
void displayAnalogValues() {
  static unsigned long lastDisplayUpdate = 0;
  const unsigned long displayInterval = 1000;
  static bool showFirst8Channels = true;
  static bool buzzerActive[16] = {false}; // Track which sensors have activated the buzzer
  char buffer[16];

  if (millis() - lastDisplayUpdate >= displayInterval) {
    lcd.clear();

    if (showFirst8Channels) {
      for (int i = 0; i < 1; i++) {
        int sensorValue = readSensor(i);   // Get sensor value
        int percentage = map(sensorValue, 0, 1023, 0, 1023); // Convert to percentage

        sprintf(buffer, "S%02d: ", i + 1);
        lcd.setCursor((i / 4) * 10 + 1, i % 4);
        lcd.print(buffer);
        lcd.print(sensorValue);
        //lcd.print("%");

        // Trigger buzzer if threshold is reached
        if (percentage >= sensorValues[i] && !buzzerActive[i]) {
          lcd.setCursor((i / 4) * 10 + 5, i % 4);
          lcd.write(byte(1)); // Display an alert
          buzzerTriggered = true;
          digitalWrite(buzzerPin, HIGH);
          digitalWrite(plcrelay, HIGH);  // Trigger buzzer
          buzzerActive[i] = true; // Mark that the buzzer is active for this sensor
        }

        // Turn off buzzer if swPin is LOW
        if (buzzerActive[i] && digitalRead(swPin) == LOW) {
          buzzerTriggered = false;
          digitalWrite(buzzerPin, LOW);
          digitalWrite(plcrelay, LOW); // Reset buzzer
        }

        // Check if the sensor has settled down
        if (buzzerActive[i] && percentage < sensorValues[i]) {
          buzzerActive[i] = false; // Reset alert state if below threshold
        }
      }
    } else {
      for (int j = 8; j < 16; j++) {
        int sensorValue = readSensor(j);  // Get sensor value
        int percentage = map(sensorValue, 0, 1023, 0, 1023); // Convert to percentage

        sprintf(buffer, "S%02d: ", j + 1);
        lcd.setCursor(((j - 8) / 4) * 10 + 1, (j - 8) % 4);
        lcd.print(buffer);
        lcd.print(percentage);
        //lcd.print("%");

        // Trigger buzzer if threshold is reached
        if (percentage >= sensorValues[j] && !buzzerActive[j]) {
          lcd.setCursor(((j - 8) / 4) * 10 + 5, (j - 8) % 4);
          lcd.write(byte(1)); // Display an alert
          buzzerTriggered = true;
          digitalWrite(buzzerPin, HIGH);
          digitalWrite(plcrelay, HIGH);  // Trigger buzzer
          buzzerActive[j] = true; // Mark that the buzzer is active for this sensor
        }

        // Turn off buzzer if swPin is LOW
        if (buzzerActive[j] && digitalRead(swPin) == LOW) {
          buzzerTriggered = false;
          digitalWrite(buzzerPin, LOW);
          digitalWrite(plcrelay, LOW); // Reset buzzer
        }

        // Check if the sensor has settled down
        if (buzzerActive[j] && percentage < sensorValues[j]) {
          buzzerActive[j] = false; // Reset alert state if below threshold
        }
      }
    }

    //showFirst8Channels = !showFirst8Channels;
    lastDisplayUpdate = millis();
  }
}
//********************************************************************************************************************************
void checkBuzzerReset() {
  // This function can be kept as is, or removed if no longer needed
  if (digitalRead(swPin) == LOW) {
    buzzerTriggered = false;
    digitalWrite(buzzerPin, LOW);
    digitalWrite(plcrelay, LOW); // Reset buzzer
  }
}
//****************************************************************************************************************************
void loadSensorValuesFromEEPROM() 
{
  for (int i = 0; i < 16; i++) {
    sensorValues[i] = EEPROM.read(i);
  }
}
//****************************************************************************************************************************
void selectCD4067Channel(int channel) {
  digitalWrite(cd4067S0, bitRead(channel, 0));
  digitalWrite(cd4067S1, bitRead(channel, 1));
  digitalWrite(cd4067S2, bitRead(channel, 2));
  digitalWrite(cd4067S3, bitRead(channel, 3));
}
//****************************************************************************************************************************
int readSensor(int sensorIndex) {
  selectCD4067Channel(sensorIndex);   // Select the desired sensor channel (0-15)
  delay(5);  // Short delay to allow CD4067 to switch channels
  int sensorValue = analogRead(cd4067SigPin);  // Read the analog value
  return sensorValue;
}
//****************************************************************************************************************************
