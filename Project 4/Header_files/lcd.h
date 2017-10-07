#ifndef LCD_H
#define LCD_H

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


void LCDDelay(int a);
void LCDbusywait();
void LCDCommand(char a);
void LCDgotoaddr(unsigned char addr);
void LCDgotoxy(unsigned char row, unsigned char column);
void LCDInit();
int LCDgetaddr();
void LCDputch(char a);
void LCDputstr(char *a);
void DDRAMDump();
void CGRAMDump();
void RTLCD();
void LCDDemo();
#endif
