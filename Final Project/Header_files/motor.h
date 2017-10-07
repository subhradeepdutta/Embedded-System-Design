/*
 ***********************************************************************************
 * Description	: Header file for motor.c
 * Date		    : 22 November 2016
 * File name	: motor.h
 ***********************************************************************************
 */
#ifndef _MOTOR_H
#define _MOTOR_H
#include <at89c51ed2.h>
#include <mcs51reg.h>

/** Function prototypes */
void motor_driver(char * read);
void display_duty_cycle_ones(unsigned char ones, unsigned char tenth);
void display_duty_cycle_tenth(unsigned char tenth);
void slow_down_motor(float desired_duty_cycle);
void speed_up_motor(float desired_duty_cycle);
void motor_on();
void motor_off();

#endif // _MOTOR_H
