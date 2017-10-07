/*
 ***********************************************************************************
 * Description	: Header file for Delays.c
 * Date		    : 22 November 2016
 * File name	: Delays.h
 ***********************************************************************************
 */

#ifndef _DELAYS_H
#define _DELAYS_H
#include <at89c51ed2.h>
#include <mcs51reg.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

/** Function prototypes */
void Delay(int a);
void FastDelay(uint8_t var)

#endif
