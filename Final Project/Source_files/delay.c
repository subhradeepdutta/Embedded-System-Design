/*
 ***********************************************************************************
 * Description	: This file contains the functions generating delays
 * Date		    : 22 November 2016
 * File name	: delay.c
 *
 ***********************************************************************************
 */

#include "delay.h"

/* Description  : This function generates a delay of 1ms
 * Input        : NA
 * Return       : NA
 */
void timer_ms()
{
    /** Timer 0, Mode 1, 16 bit timer */
    TMOD |= 0x01;

    /** initial value for 1ms */
    TH0= 0xFC;
    TL0 = 0x66;

    /** Enable global and timer 0 interrupts */
    IEN0 |= 0x82;

    /** Start timer */
    TR0 = 1;
}

/* Description  :   This function is used generate delay in us.
 *                  It genarates a approximate delay of 10us for each count,
 *                  if 5000 is passed as the argument then it generates a delay of apprx 50ms.
 * Input        :   unsigned int representing the in us / 10
 * Return       :   NA
 */
void delay_us(unsigned int us_count)
{
    while(us_count != 0)
    {
		 us_count--;
    }
}

/* Description  :   This function is used generate delay in ms.
 *                  It genarates a approximate delay of 1ms for each count,
 *                  if 1000 is passed as the argument then it generates delay of apprx 1000ms(1sec)
 * Input        :   unsigned int representing the in ms
 * Return       :   NA
 */
void delay_ms(unsigned int ms_count)
{
    while(ms_count!=0)
    {
        delay_us(112);	 //delay_us is called to generate 1ms delay
        ms_count--;
    }
}

/* Description  :   This function is used generate delay in sec .
 *                  It genarates a approximate delay of 1sec for each count,
 *                  if 10 is passed as the argument then it generates delay of apprx 10sec
 * Input        :   unsigned int representing the in sec
 * Return       :   NA
 */
void delay_sec(unsigned char sec_count)
{
    while(sec_count!=0)
    {
        delay_ms(1000);	//delay_ms is called to generate 1sec delay
        sec_count--;
    }
}
