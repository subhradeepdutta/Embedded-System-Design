#ifndef EEPROM_H
#define EEPROM_H

#include <at89c51ed2.h>
#include <mcs51reg.h>
#include <stdio.h>
#include <stdlib.h>

//I2C Module Connections
#define SDA P1_1
#define SCL P1_2


void I2CClk();
void I2CDelay(uint8_t var);
void I2CStart();
void I2CStop();
unsigned char I2CRead();
void I2CWrite(unsigned char datavalue);

int I2CAck();
void I2CNack();
void WriteByte(unsigned char address, unsigned char datavalue, unsigned char PageNo);
unsigned char ReadByte(unsigned char address,unsigned char PageNo);
unsigned char HexDigit(unsigned char n);
unsigned char DecDigit(unsigned char n);

void WriteByteDemo();
void ReadByteDemo();
void LCDDisplayDemo();
void HexDump();
void eereset();


#endif
