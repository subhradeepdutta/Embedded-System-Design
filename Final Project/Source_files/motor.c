/*
 ***********************************************************************************
 * Description	: This file contains the functions for the motor control operations
 * Date		    : 27 October 2016
 * File name	: motor.c
 *
 ***********************************************************************************
 */
#include "motor.h"
#include "stdlib.h"
#include "lcd.h"
#include "pwm.h"

/*************************************************************************************
Description   : This function is used to decipher the message received from the GSM
                module, and decides the direction of the motor rotation and the speed
                of rotation of motor.

Input         : The message received from the GSM module
Output        : Various control signals to control the direction and speed of the
                motor.
*************************************************************************************/
void motor_driver(char * read)
{
    unsigned char duty_cycle[3];
    unsigned char duty_cycle_val;

    /*******************************DIRECTION CONTROL******************************************/

    /*******************************************************************************************
    Please note that:
    P1.0 (PORT 1 BIT 6 of the microcontroller) is connected to Input 1 of the L293D motor driver
    P1.2 is connected to Input 2 of the L293D motor driver
    ---------------------------------------------
    P1.0    P1.2    Directiion of motor rotation
    ---------------------------------------------
    0       1       Counter clockwise
    1       0       Clockwise
    *******************************************************************************************/

    /*************************************************************************************
    Check if the message received is for clockwise direction control:
    If the message "cw" is received, then need to rotate the motor in clockwise direction
    According to the table above, need to send the following signals:
    P1.0 = 1;
    P1.2 = 0;
    **************************************************************************************/
    if ((read[0] == 'c') || (read[0] == 'C'))
    {
        if (read[1] == 'w')
        {
            previous_duty_cycle = ((255 - CCAP0H) * 100) / 255;
            slow_down_motor(0);
            P1_0 = 1;
            P1_2 = 0;
            speed_up_motor(previous_duty_cycle);
            lcdgotoxy(3, 1);
            lcdputstr("                ");
            lcdgotoxy(3, 1);
            lcdputstr("Motor ON:cw:");
        }
    }

   /*************************************************************************************
    Check if the message received is for counter-clockwise direction control:
    If the message "ccw" is received, then need to rotate the motor in counter-clockwise
    direction.
    According to the table above, need to send the following signals:
    P1.0 = 0;
    P1.2 = 1;
    **************************************************************************************/
    if ((read[0] == 'c') || (read[0] == 'C'))
    {
        if (read[1] == 'c')
        {
            if (read[2] == 'w')
            {
                previous_duty_cycle = ((255 - CCAP0H) * 100) / 255;
                slow_down_motor(0);
                P1_0 = 0;
                P1_2 = 1;
                speed_up_motor(previous_duty_cycle);
                lcdgotoxy(3, 1);
                lcdputstr("                ");
                lcdgotoxy(3, 1);
                lcdputstr("Motor ON:ccw:");
            }
        }
    }

    /*******************************SPEED CONTROL******************************************/
    /**************************************************************************************
    Please note:
    For Speed control, the first two characters of the message has to be "sc", indicating
    speed control, followed by 2 digits that represent the desired duty cycle
    For example,
    sc50    : means 50% duty cycle
    sc95    : means 95% duty cycle
    **************************************************************************************/
    /**************************************************************************************
    The connections:
    The PWM output of PCA is available at P1.3
    P1.3 is connected to Enable 1, that is pin 1 of the L293D motor driver
    **************************************************************************************/
    if ((read[0] == 's') || (read[0] == 'S'))
    {
        if (read[1] == 'c')
        {
            if ((read[2] >= '0') && (read[2] <= '9'))
            {
                /** Display left border */
                lcdgotoxy(4, 1);
                send_data(0);

                /** Display right border */
                lcdgotoxy(4, 12);
                send_data(5);

                /** Read the tens digit
                    For example, if the speed is 68%, read and store '6'
                 */
                duty_cycle[0] = read[2];

                /** Call the function to graphically disply this digit on the LCD */
                display_duty_cycle_tenth(read[2]);

                if ((read[3] >= '0') && (read[3] <= '9'))
                {
                    duty_cycle[1] = read[3];
                    duty_cycle[2] = '\0';

                    display_duty_cycle_ones(read[3], read[2]);

                    duty_cycle_val = atoi(duty_cycle);

                    change_duty_cycle(duty_cycle_val);

                    lcdgotoxy(3, 1);
                    lcdputstr("                ");
                    lcdgotoxy(3, 1);
                    if ((P1_0) && (!P1_2))
                    {
                        lcdputstr("Motor ON:cw:");
                    }
                    else
                    {
                        lcdputstr("Motor ON:ccw:");
                    }
                    lcdputch(read[2]);
                    lcdputch(read[3]);
                    lcdputch('%');
                }
            }
        }
    }
}

/* Description  : This function turns the motor ON
 * Input        : NA
 * Return       : NA
 */
void motor_on()
{
    /** Turn on the PWM counter */
    CCON |= 0x40;

    /** Driver will turn off motor if both the inputs are of the same polarity */
    /** Clockwise rotation by default */
    P1_0 = 1;
    P1_2 = 0;

    /** Default duty cycle is 50% */
    CCAP0H = 127;

    lcdgotoxy(3, 1);
    lcdputstr("                ");
    lcdgotoxy(3, 1);
    lcdputstr("Motor ON:cw");
}

/* Description  : This function turns the motor OFF
 * Input        : NA
 * Return       : NA
 */
void motor_off()
{
    /** Turn on the PWM counter */
    CCON &= ~0x40;

    /** Driver will turn off motor if both the inputs are of the same polarity */
    /** Clockwise rotation by default */
    P1_0 = 1;
    P1_2 = 1;
    lcdgotoxy(3, 1);
    lcdputstr("                ");
    lcdgotoxy(3, 1);
    lcdputstr("Motor OFF");
}

/* Description  :   This function displays the digit in the ones place of the duty cycle
                    For example, consider 56% duty cycle, This function extracts the digit
                    '6' and displays it on the LCD
 * Input        : NA
 * Return       : NA
 */
void display_duty_cycle_ones(unsigned char ones, unsigned char tenth)
{
    /* Loop variable */
    unsigned char i = 0;
    unsigned char temp[2] = {ones, '\0'};
    unsigned char temp1[2] = {tenth, '\0'};
    unsigned char ones_val = atoi(temp);
    unsigned char tenth_val = atoi(temp1);


    lcdgotoxy(4, tenth_val + 2);
    my_delay_ms(2);
    /* Logic for displaying the custom character for the ones digit */

    if (ones_val == 0)
    {
        send_data(6);
    }

    else if (ones_val <= 2)
    {
        send_data(0);
    }

    else if (ones_val <= 4)
    {
        send_data(1);
    }

    else if (ones_val <= 6)
    {
        send_data(2);
    }

    else if (ones_val <= 8)
    {
        send_data(3);
    }

    else
    {
        send_data(4);
    }
    my_delay_ms(2);
}

/***********************************************************
Description:
This function takes the duty cycle value in percentage, and
it gradually slows down the motor by 5% at a time till it
reaches the desired duty cycle
************************************************************/
void slow_down_motor(float desired_duty_cycle)
{
    float assign_duty_cycle;

    assign_duty_cycle = (255 - (desired_duty_cycle * 2.55));

    while (CCAP0H < assign_duty_cycle)
    {
        CCAP0H += 12.75;
        my_delay_ms(40);
    }

    CCAP0H = (255 - (desired_duty_cycle * 2.55));
    my_delay_ms(40);
}

/***********************************************************
Description:
This function takes the duty cycle value in percentage, and
it gradually speeds up the motor by 5% at a time till it
reaches the desired duty cycle
************************************************************/
void speed_up_motor(float desired_duty_cycle)
{
    float assign_duty_cycle;

    assign_duty_cycle = (255 - (desired_duty_cycle * 2.55));

    while (CCAP0H > assign_duty_cycle)
    {
        CCAP0H -= 12.75;
        my_delay_ms(40);
    }

    CCAP0H = (255 - (desired_duty_cycle * 2.55));
    my_delay_ms(40);
}

/* Description  :   This function displays the digit in the tens place of the duty cycle
                    For example, consider 56% duty cycle, This function extracts the digit
                    '5' and displays it on the LCD
 * Input        : NA
 * Return       : NA
 */
void display_duty_cycle_tenth(unsigned char tenth)
{
    /* Loop variable */
    unsigned char i = 0;
    unsigned char temp[2] = {tenth, '\0'};
    unsigned char tenth_val = atoi(temp);

    lcdgotoxy(4, 2);
    my_delay_ms(2);
    lcdputstr("          ");
    my_delay_ms(2);

    for (i = 0; i < tenth_val; i++)
    {
        lcdgotoxy(4, i + 2);
        my_delay_ms(2);
        send_data(4);
        my_delay_ms(2);
    }
}
