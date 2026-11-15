#include <Arduino.h>
#include <SPI.h>
#include "ADE7758.h"

ADE7758::ADE7758() {}

bool ADE7758::Init(void) {
  pinMode(AFECS, OUTPUT);
  digitalWrite(AFECS, HIGH);
  SPI.begin();
  SPI.beginTransaction(SPISettings(100000, MSBFIRST, SPI_MODE2));
  SPI.endTransaction();
  write8(OPMODE, SWRST);
  delayMicroseconds(20);
  setOpMode(0x00);
  setCompMode(0x04);
  uint8_t l = getLcycMode();
  l &= ~0x01;
  setLcycMode(l);
  setAPCFDEN(0x003F);
  setAPCFNUM(0x0200);
  setAPCFNUM(0x0100);
  delay(50);
  setAPCFNUM(0x0400);
  setLcycMode(0x78);
  delayMicroseconds(10);
  return ((l & 0xF8) == 0x78);
}

void ADE7758::setSPI(void) {
  SPI.setDataMode(SPI_MODE2);
  delay(10);
}

void ADE7758::closeSPI(void) {
  SPI.setDataMode(SPI_MODE0);
  delay(10);
}

void ADE7758::enableChip(void) { digitalWrite((AFECS), LOW); }
void ADE7758::disableChip(void) { digitalWrite((AFECS), HIGH); }

unsigned char ADE7758::read8(char reg) {
  enableChip();
  SPI.transfer(reg);
  unsigned char b0 = SPI.transfer(0x00);
  disableChip();
  return b0;
}

unsigned int ADE7758::read16(char reg) {
  enableChip();
  SPI.transfer(reg);
  unsigned char b1 = SPI.transfer(0x00);
  unsigned char b0 = SPI.transfer(0x00);
  disableChip();
  return (unsigned int)b1 << 8 | (unsigned int)b0;
}

unsigned long ADE7758::read24(char reg) {
  enableChip();
  SPI.transfer(reg);
  unsigned char b2 = SPI.transfer(0x00);
  unsigned char b1 = SPI.transfer(0x00);
  unsigned char b0 = SPI.transfer(0x00);
  disableChip();
  return (unsigned long)b2 << 16 | (unsigned long)b1 << 8 | (unsigned long)b0;
}

void ADE7758::write8(char reg, unsigned char data) {
  enableChip();
  reg |= WRITE;
  SPI.transfer((unsigned char)reg);
  SPI.transfer((unsigned char)data);
  disableChip();
}

void ADE7758::write16(char reg, unsigned int data) {
  enableChip();
  reg |= WRITE;
  SPI.transfer((unsigned char)reg);
  SPI.transfer((unsigned char)(data >> 8));
  SPI.transfer((unsigned char)data);
  disableChip();
}

void ADE7758::write24(char reg, unsigned long data) {
  enableChip();
  reg |= WRITE;
  SPI.transfer((unsigned char)reg);
  SPI.transfer((unsigned char)(data >> 16));
  SPI.transfer((unsigned char)(data >> 8));
  SPI.transfer((unsigned char)data);
  disableChip();
}

unsigned char ADE7758::getVersion() { return read8(VERSION); }
void ADE7758::setOpMode(char m) { write8(OPMODE, m); }
int ADE7758::getOpMode() { return read8(OPMODE); }
void ADE7758::setMMode(char m) { write8(MMODE, m); }
int ADE7758::getMMode() { return read8(MMODE); }
void ADE7758::setWavMode(char m) { write8(WAVMODE, m); }
int ADE7758::getWavMode() { return read8(WAVMODE); }
void ADE7758::setCompMode(char m) { write8(COMPMODE, m); }
int ADE7758::getCompMode() { return read8(COMPMODE); }
void ADE7758::setLcycMode(char m) { write8(LCYCMODE, m); }
int ADE7758::getLcycMode() { return read8(LCYCMODE); }

void ADE7758::gainSetup(char integrator, char scale, char PGA2, char PGA1) {
  char pgas = (integrator << 7) | (PGA2 << 5) | (scale << 3) | (PGA1);
  write8(GAIN, pgas);
}

void ADE7758::setupDivs(unsigned char Watt_div, unsigned char VAR_div, unsigned char VA_div) {
  write8(WDIV, Watt_div);
  write8(VARDIV, VAR_div);
  write8(VADIV, VA_div);
}

long ADE7758::getMaskInterrupts(void) { return read24(MASK); }
void ADE7758::setMaskInterrupts(unsigned long m) { return write24(MASK, m); }
unsigned long ADE7758::getStatus(void) { return read24(STATUS); }
unsigned long ADE7758::resetStatus(void) { return read24(RSTATUS); }

long ADE7758::getAIRMS(void) { return read24(AIRMS); }
long ADE7758::getBIRMS(void) { return read24(BIRMS); }
long ADE7758::getCIRMS(void) { return read24(CIRMS); }
long ADE7758::getAVRMS(void) { return read24(AVRMS); }
long ADE7758::getBVRMS(void) { return read24(BVRMS); }
long ADE7758::getCVRMS(void) { return read24(CVRMS); }

long ADE7758::avrms() { long v = 0; for (char i = 0; i < 10; ++i) v += getAVRMS(); return v / 10; }
long ADE7758::bvrms() { long v = 0; for (char i = 0; i < 10; ++i) v += getBVRMS(); return v / 10; }
long ADE7758::cvrms() { long v = 0; for (char i = 0; i < 10; ++i) v += getCVRMS(); return v / 10; }
long ADE7758::airms() { long i = 0; for (char n = 0; n < 10; ++n) i += getAIRMS(); return i / 10; }
long ADE7758::birms() { long i = 0; for (char n = 0; n < 10; ++n) i += getBIRMS(); return i / 10; }
long ADE7758::cirms() { long i = 0; for (char n = 0; n < 10; ++n) i += getCIRMS(); return i / 10; }

int ADE7758::getFreq(void) { return read16(FREQ); }
void ADE7758::setLineCyc(int d) { write16(LINECYC, d); }

int ADE7758::getACurrentOffset() { return ((read16(AIRMSOS)) << 4) >> 4; }
int ADE7758::getBCurrentOffset() { return ((read16(BIRMSOS)) << 4) >> 4; }
int ADE7758::getCCurrentOffset() { return ((read16(CIRMSOS)) << 4) >> 4; }
void ADE7758::setACurrentOffset(int o) { write16(AIRMSOS, o); }
void ADE7758::setBCurrentOffset(int o) { write16(BIRMSOS, o); }
void ADE7758::setCCurrentOffset(int o) { write16(CIRMSOS, o); }

int ADE7758::getAVoltageOffset() { return ((read16(AVRMSOS)) << 4) >> 4; }
int ADE7758::getBVoltageOffset() { return ((read16(BVRMSOS)) << 4) >> 4; }
int ADE7758::getCVoltageOffset() { return ((read16(CVRMSOS)) << 4) >> 4; }
void ADE7758::setAVoltageOffset(int o) { write16(AVRMSOS, o); }
void ADE7758::setBVoltageOffset(int o) { write16(BVRMSOS, o); }
void ADE7758::setCVoltageOffset(int o) { write16(CVRMSOS, o); }

void ADE7758::setZeroCrossingTimeout(int d) { write16(ZXTOUT, d); }
int ADE7758::getZeroCrossingTimeout() { return read16(ZXTOUT); }

int ADE7758::getWatt(char Phase) {
  switch (Phase) {
    case PHASE_A: return read16(AWATTHR);
    case PHASE_B: return read16(BWATTHR);
    case PHASE_C: return read16(CWATTHR);
  }
  return 0;
}

int ADE7758::getVar(char Phase) {
  switch (Phase) {
    case PHASE_A: return read16(AVARHR);
    case PHASE_B: return read16(BVARHR);
    case PHASE_C: return read16(CVARHR);
  }
  return 0;
}

int ADE7758::getVa(char Phase) {
  switch (Phase) {
    case PHASE_A: return read16(AVAHR);
    case PHASE_B: return read16(BVAHR);
    case PHASE_C: return read16(CVAHR);
  }
  return 0;
}

void ADE7758::setAPCFDEN(int d) { write16(APCFDEN, d); }
int ADE7758::getAPCFDEN() { return ((read16(APCFDEN)) << 4) >> 4; }
void ADE7758::setAPCFNUM(int d) { write16(APCFNUM, d); }
int ADE7758::getAPCFNUM() { return read16(APCFNUM); }

void ADE7758::setVARCFDEN(int d) { write16(VARCFDEN, d); }
int ADE7758::getVARCFDEN() { return ((read16(VARCFDEN)) << 4) >> 4; }
void ADE7758::setVARCFNUM(int d) { write16(VARCFNUM, d); }
int ADE7758::getVARCFNUM() { return read16(VARCFNUM); }

char ADE7758::getTemperature() { return read8(TEMP); }
