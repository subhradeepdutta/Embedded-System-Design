/*
 ***********************************************************************************
 * Description	: Header file for lcd.c
 * Date		    : 27 October 2016
 * File name	: lcd.h
 *
 ***********************************************************************************
 */
#ifndef LCD_H
#define LCD_H

#include <at89c51ed2.h>
#include <mcs51reg.h>
#include <stdio.h>
#include <stdlib.h>

/** Pin definitions for LCD data and control */
#define RS  P1_5
#define RW  P1_6
#define EN  P1_7
#define D0  P0_0
#define D1  P0_1
#define D2  P0_2
#define D3  P0_3
#define D4  P0_4
#define D5  P0_5
#define D6  P0_6
#define D7  P0_7
#define BUSY_MASK 0x80
#define LCD_MAP ((xdata unsigned char *)0x8000)

/** Function prototypes */
void lcdinit();
void lcdbusywait();
void lcdgotoaddr(char x);
void lcdgotoxy(char x, char y);
void lcdputch(char a);
void lcdputstr(char *a);
void my_delay_ms(int a);
void send_data(char a);
void send_command(char a);
void clear_display();
void shift_right();
void shift_left();
void goto_next_line();
char read_ddram_addr();
void lcd_create_custom_char(unsigned char char_code, unsigned char pixel_val[]);
unsigned char read_ddram_data();
void graphical_percentage_display();

#endif // LCD_H


