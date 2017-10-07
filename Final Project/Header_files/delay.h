/*
 ***********************************************************************************
 * Description	: Header File for delay.c
 * Date		    : 22 November 2016
 * File name	: delay.h
 *
 ***********************************************************************************
 */

#ifndef _DELAY_H
#define _DELAY_H
#include <at89c51ed2.h>
#include <mcs51reg.h>
#include <stdio.h>
#include <stdlib.h>


/** Function prototypes */
void delay_us(unsigned int);
void delay_ms(unsigned int);
void delay_sec(unsigned char);
void timer_ms();

#endif
