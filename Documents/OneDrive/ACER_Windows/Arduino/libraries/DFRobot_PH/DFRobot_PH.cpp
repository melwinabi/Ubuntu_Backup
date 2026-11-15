/*!
 * @file DFRobot_PH.cpp
 * @brief Modified for ESP32 (3.3V / 12-bit ADC)
 */

#if ARDUINO >= 100
#include "Arduino.h"
#else
#include "WProgram.h"
#endif

#include "DFRobot_PH.h"
#include <EEPROM.h>

#define EEPROM_write(address, p) {int i = 0; byte *pp = (byte*)&(p);for(; i < sizeof(p); i++) EEPROM.write(address+i, pp[i]);}
#define EEPROM_read(address, p)  {int i = 0; byte *pp = (byte*)&(p);for(; i < sizeof(p); i++) pp[i]=EEPROM.read(address+i);}

#define PHVALUEADDR 0x00 

char* DFRobot_PH::strupr(char* str) {
    if (str == NULL) return NULL;
    char *ptr = str;
    while (*ptr != '\0' && *ptr != ' ') {
        *ptr = toupper((unsigned char)*ptr);
        ptr++;
    }
    return str;
}

DFRobot_PH::DFRobot_PH()
{
    this->_temperature    = 25.0;
    this->_phValue        = 7.0;
    this->_acidVoltage    = 2032.44;   // Default for 4.0
    this->_neutralVoltage = 1500.0;    // Default for 7.0
    this->_voltage        = 1500.0;
}

DFRobot_PH::~DFRobot_PH() {}

void DFRobot_PH::begin()
{
    // Load calibration from EEPROM
    EEPROM_read(PHVALUEADDR, this->_neutralVoltage);
    if(isnan(this->_neutralVoltage) || this->_neutralVoltage < 500.0){
        this->_neutralVoltage = 1500.0;
    }
    
    EEPROM_read(PHVALUEADDR+4, this->_acidVoltage);
    if(isnan(this->_acidVoltage) || this->_acidVoltage < 500.0){
        this->_acidVoltage = 2032.44;
    }
}

float DFRobot_PH::readPH(float voltage, float temperature)
{
    // Simplified Linear Interpolation: y = mx + c
    // Calculated based on the calibrated points stored in EEPROM
    float slope = (7.0 - 4.0) / (this->_neutralVoltage - this->_acidVoltage);
    float intercept = 7.0 - slope * this->_neutralVoltage;
    
    this->_phValue = slope * voltage + intercept;
    
    return this->_phValue;
}

void DFRobot_PH::calibration(float voltage, float temperature, char* cmd)
{
    this->_voltage = voltage;
    this->_temperature = temperature;
    String sCmd = String(cmd);
    sCmd.toUpperCase();
    phCalibration(cmdParse(sCmd.c_str()));
}

void DFRobot_PH::calibration(float voltage, float temperature)
{
    this->_voltage = voltage;
    this->_temperature = temperature;
    if(cmdSerialDataAvailable() > 0){
        phCalibration(cmdParse());
    }
}

boolean DFRobot_PH::cmdSerialDataAvailable()
{
    char cmdReceivedChar;
    static unsigned long cmdReceivedTimeOut = millis();
    while(Serial.available()>0){
        if(millis() - cmdReceivedTimeOut > 500U){
            this->_cmdReceivedBufferIndex = 0;
            memset(this->_cmdReceivedBuffer,0,(ReceivedBufferLength));
        }
        cmdReceivedTimeOut = millis();
        cmdReceivedChar = Serial.read();
        if (cmdReceivedChar == '\n' || this->_cmdReceivedBufferIndex==ReceivedBufferLength-1){
            this->_cmdReceivedBufferIndex = 0;
            strupr(this->_cmdReceivedBuffer);
            return true;
        }else{
            this->_cmdReceivedBuffer[this->_cmdReceivedBufferIndex] = cmdReceivedChar;
            this->_cmdReceivedBufferIndex++;
        }
    }
    return false;
}

byte DFRobot_PH::cmdParse(const char* cmd)
{
    if(strstr(cmd, "ENTERPH") != NULL) return 1;
    if(strstr(cmd, "CALPH")   != NULL) return 2;
    if(strstr(cmd, "EXITPH")  != NULL) return 3;
    return 0;
}

byte DFRobot_PH::cmdParse()
{
    if(strstr(this->_cmdReceivedBuffer, "ENTERPH") != NULL) return 1;
    if(strstr(this->_cmdReceivedBuffer, "CALPH")   != NULL) return 2;
    if(strstr(this->_cmdReceivedBuffer, "EXITPH")  != NULL) return 3;
    return 0;
}

void DFRobot_PH::phCalibration(byte mode)
{
    static boolean phCalibrationFinish  = 0;
    static boolean enterCalibrationFlag = 0;
    
    switch(mode){
        case 0:
            if(enterCalibrationFlag) Serial.println(F(">>>Command Error<<<"));
            break;

        case 1:
            enterCalibrationFlag = 1;
            phCalibrationFinish  = 0;
            Serial.println(F("\n>>>Enter PH Calibration Mode<<<"));
            Serial.println(F(">>>Put probe into 4.0 or 7.0 buffer solution<<<"));
            break;

        case 2:
            if(enterCalibrationFlag){
                // ESP32 Voltage Ranges (Adjusted for 3.3V Logic)
                if((this->_voltage > 1000) && (this->_voltage < 1550)){
                    Serial.println(F("\n>>>Buffer Solution: 7.0 Identified. Send EXITPH to Save.<<<"));
                    this->_neutralVoltage = this->_voltage;
                    phCalibrationFinish = 1;
                } 
                else if((this->_voltage > 1600) && (this->_voltage < 2400)){
                    Serial.println(F("\n>>>Buffer Solution: 4.0 Identified. Send EXITPH to Save.<<<"));
                    this->_acidVoltage = this->_voltage;
                    phCalibrationFinish = 1;
                } else {
                    Serial.print(F("\n>>>Buffer Error. Current Voltage: "));
                    Serial.print(this->_voltage);
                    Serial.println(F("mV. Check Solution and Probe.<<<"));
                    phCalibrationFinish = 0;
                }
            }
            break;

        case 3:
            if(enterCalibrationFlag){
                if(phCalibrationFinish){
                    EEPROM_write(PHVALUEADDR, this->_neutralVoltage);
                    EEPROM_write(PHVALUEADDR+4, this->_acidVoltage);
                    EEPROM.commit(); // REQUIRED FOR ESP32
                    Serial.println(F("\n>>>Calibration Successful and Saved<<<"));
                } else {
                    Serial.println(F("\n>>>Calibration Failed<<<"));
                }
                enterCalibrationFlag = 0;
                phCalibrationFinish = 0;
            }
            break;
    }
}