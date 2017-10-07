/*
 ***********************************************************************************
 * Description	: This file contains the functions generating delays
 * Date		    : 22 November 2016
 * File name	: delay.c
 *
 ***********************************************************************************
 */

#include "pwm.h"
#include "motor.h"


/* Description  : This function initializes the PWM module
 * Input        : NA
 * Return       : NA
 */
void init_pwm()
{
    /* Internal clock, F / 6, No interrupt */
    CMOD = 0x00;

    /* Positive edge capture, PWM mode(Bit 1) */
    CCAPM0 = 0x42;

    /* Turn the counter on */
    CCON = 0x40;

    /* Enable PCA interrupt? */
    IEN0 |= 0xC1;

    /* Default duty cycle is 50% */
    CCAP0H = 127;

    /* By default motor is off */
    motor_off();
}

/* Description  :   This function is used to change the duty cycle of
 *                  pwm to the given % value
 * Input        :   Duty cycle value in percentage
 * Return       :   NA
 */
void change_duty_cycle(unsigned char duty_cycle)
{
    /** Local temporary variable */
    float assign_duty_cycle;

    /***********************************************
    Please Note:
    255 represents 100% duty cycle
    Each percent is 2.55
    We get the input in terms of percentage, to convert
    it to a scale of 255, we multiply by 2.55
    ***********************************************/
    assign_duty_cycle = duty_cycle * 2.55;

    /** Check if the desired duty cycle is less than the
      * desired duty cycle
      */
    if (CCAP0H > assign_duty_cycle)
    {
        /** Call the function to speed up the motor */
        speed_up_motor(assign_duty_cycle);
    }

    /** Check if the desired duty cycle is greater than the
      * desired duty cycle
      */
    else if (CCAP0H < assign_duty_cycle)
    {
        /** Call the function to slow down the motor */
        slow_down_motor(assign_duty_cycle);
    }
}
