#ifndef CUSTOM_CHAR_H
#define CUSTOM_CHAR_H

#include <at89c51ed2.h>
#include <mcs51reg.h>
#include <stdio.h>
#include <stdlib.h>

//LCD Module Connections
#define RS P1_5
#define RW P1_4
#define DB0 P0_0
#define DB1 P0_1
#define DB2 P0_2
#define DB3 P0_3
#define DB4 P0_4
#define DB5 P0_5
#define DB6 P0_6
#define DB7 P0_7
#define BF P0_7

void CustomChar(unsigned char ccode, unsigned char * row_val, unsigned char loc);
void BatteryLogo();
void CustomCharDemo();
#endif
