/*
 ***********************************************************************************
 * Description	: Header file for main.c
 * Date		    : 22 November 2016
 * File name	: main.h
 *
 ***********************************************************************************
 */
#ifndef _MAIN_H
#define _MAIN_H

#include <at89c51ed2.h>
#include <mcs51reg.h>
#include <stdio.h>
#include <stdlib.h>
#include "lcd.h"
#include "delay.h"
#include "pwm.h"
#include "motor.h"

/** Macros for new line and carriage return */
#define NEW_LINE 0x0a
#define CARRIAGE_RETURN 0x0d

/** Function prototypes */
void put_string(unsigned char *str);
void motor_driver(char * read);

#endif // _MAIN_H

