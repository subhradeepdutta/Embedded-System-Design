/*
 ***********************************************************************************
 * Description	: Header File for pwm.c
 * Date		    : 22 November 2016
 * File name	: pwm.h
 *
 ***********************************************************************************
 */

#ifndef _PWM_H
#define _PWM_H
#include <at89c51ed2.h>
#include <mcs51reg.h>

float previous_duty_cycle;

/** Function prototypes */
void init_pwm();
void change_duty_cycle(unsigned char duty_cycle);

#endif // _PWM_H
