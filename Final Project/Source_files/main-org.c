/*
 ***********************************************************************************
 * Description	: This file contains the functions for the motor control using Flex
                  sensor and accelerometer
 * Date		    : 27 October 2016
 * File name	: main-org.c
 *
 ***********************************************************************************/

//Including the required header files for 8051 microcontroller
#include <at89c51ed2.h>
#include <mcs51reg.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include "delays.h"

#define SC P1_0
#define EOC P1_4
#define OE P1_5
#define CLK P1_6
#define ALE P1_7
#define MOTORIN1 P3_4
#define MOTORIN2 P3_5
#define ADDRESS_A P2_0
#define ADDRESS_B P2_1
#define ADDRESS_C P2_2
#define FLEX 0
#define ACCEL 1

//Define global variables

uint8_t ADCdata;
float duty_cycle;
int multiplier;
int interrupt_counter;
int sensor;
/*****************************************************STARTUP CODE********************************************************************/

_sdcc_external_startup()//Startup function that is executed first when the microcontroller is turned on
{

    TI=0;        //Set TI flag to 0
    AUXR|=0x0C;  //Set the XRS1:XRS0 bit to 1 to enable 1 KB of internal extended RAM
    TMOD=0X20;   //Enable Timer 1 Mode 2
    TH1=0xFF;    //Load TH1 with FD for 57600 baud rate
    SCON=0X50;   //Select Mode 1 for full duplex with variable baud rate
    PCON|=0x80;
    TR1=1;       //Start Timer

    return 0;
}
/********************************************************PUTCHAR DEFINITION************************************************************/
void putchar (char c)//Definition fo putchar function to display characters on the serial terminal
{
    SBUF = c;    //SBUF stores the character to be dsiplayed
	while (!TI); //Wait for TI flag
	TI = 0;      //Set TI flag to 0
}
/********************************************************GETCHAR DEFINITION************************************************************/
char getchar()      //Definition of getchar function to fetch input from the user
{
    while(RI==0);   //Wait for RI flag
    RI=0;           //Set RI to 0
    return SBUF;    //Return the value in the buffer
}

/*************************************************************************************
Description   : This function is used to load the register values to intialize PWM
                using PCA module on P1.3 using module 0

Input         : N/A
Output        : N/A
*************************************************************************************/


void PWMInit()
{
    CMOD&=0x79;//Enable PCA Counter during idle mode and set Fclk/6
    CCAPM0|=0x42;// Enable PWM mode and enable comparator
    CCAP0L=duty_cycle;//Set to 0x99 which is 153 in decimal, 153/255=60% which is the Toff so Ton is 40%
    CCAP0H=duty_cycle;//Set to 40% duty cycle initially
    CL=0x00;
    CH=0x00;
}

/*************************************************************************************
Description   : This function is used to turn on the PWM on P1.3

Input         : N/A
Output        : N/A
*************************************************************************************/

void PWMON()
{
    PWMInit();
    CCON|=0x40;//Enable PCA Counter Run Control Bit
    MOTORIN1=1;//Set motor inputs to 1 and 0 for clockwise motion
    MOTORIN2=0;


}
/*************************************************************************************
Description   : This function is used to turn off the PWM on P1.3

Input         : N/A
Output        : N/A
*************************************************************************************/

void PWMOFF()
{
    CCON&=0xBF;//Disable timer
    MOTORIN1=0;//Turn off the motor inputs
    MOTORIN2=0;//Both inputs set to 0
}


/*************************************************************************************
Description   : This function is used to write the new duty cycle values to the
                corresponding registers CCAP0L and CCAP0H
Input         : N/A
Output        : N/A
*************************************************************************************/


void WriteDutyCycleValues()
{
    CCAP0L=duty_cycle;//Set to 0x99 which is 153 in decimal, 153/255=60% which is the Toff so Ton is 40%
    CCAP0H=duty_cycle;
}

/*************************************************************************************
Description   : This function is used to slowly decrease the duty cycle by 5% at a
                time until it reaches 5%
Input         : N/A
Output        : N/A
*************************************************************************************/

void SlowDownMotor()
{
    do
    {
        duty_cycle+=0x0D;
        WriteDutyCycleValues();
        Delay(40);
    }while(duty_cycle<=0xF2);//Keep decrmenting the duty cycle until it reaches 5%
}

/*************************************************************************************
Description   : This function is used to slowly increase the duty cycle by 5% at a
                time until it reaches the desired duty cycle
Input         : target duty cycle
Output        : N/A
*************************************************************************************/


void SpeedUpMotor(int target_duty)
{
    do
    {
        duty_cycle-=0x0D;
        printf_tiny("Duty %d", duty_cycle);
        WriteDutyCycleValues();
        Delay(40);
    }while((duty_cycle>=(target_duty+0x0D))&&(duty_cycle>=0x0D));//Keep incrementing the duty cycle until it hits the target duty cycle
}

/*************************************************************************************
Description   : This function is used to slowly decrease the duty cycle to 5% and then
                gradually increase the duty cycle to the original value but in the
                opposite direction. It is used to reverse the motor rotation
Input         : N/A
Output        : N/A
*************************************************************************************/


void MotorReverse()
{
    int old_duty_cycle;
    old_duty_cycle=duty_cycle;//Store the current duty cycle valie
    SlowDownMotor();//Slow down the motor to 5% from its current duty cycle
    MOTORIN1=!(MOTORIN1);//Toggle the motor inputs to reverse the direction of rotation
    MOTORIN2=!(MOTORIN2);
    SpeedUpMotor(old_duty_cycle);//Speed up the motor again to the original duty cycle but opposite direction
    printf_tiny("\n\rMotor direction reversed\n\r");
}

/*************************************************************************************
Description   : This function is used to set a particular duty cycle for the motor
Input         : desired duty cycle
Output        : N/A
*************************************************************************************/


void SetSpeed(float speed)
{
    duty_cycle=speed;
    WriteDutyCycleValues();
}

/*************************************************************************************
Description   : This function is used to map the output of the flex sensor to varying
                duty cycles after reading them from the ADC
Input         : N/A
Output        : N/A
*************************************************************************************/

//The ADC output for the flex sensor was intially tested to obtain the below used values
void ADCToMotorSpeed()
{
    int percentspeed=0;
        if(ADCdata>=80 && ADCdata<=105)
        multiplier=16;
        else if(ADCdata>=106 && ADCdata<=110)
        multiplier=15;
        else if(ADCdata>=111 && ADCdata<=115)
        multiplier=14;
        else if(ADCdata>=116 && ADCdata<=120)
        multiplier=13;
        else if(ADCdata>=121 && ADCdata<=125)
        multiplier=12;
        else if(ADCdata>=126 && ADCdata<=130)
        multiplier=11;
        else if(ADCdata>=131 && ADCdata<=135)
        multiplier=10;
        else if(ADCdata>=136 && ADCdata<=140)
        multiplier=9;
        else if(ADCdata>=141 && ADCdata<=145)
        multiplier=8;
        else if(ADCdata>=146 && ADCdata<=150)
        multiplier=7;
        else if(ADCdata>=151 && ADCdata<=155)
        multiplier=6;
        else if(ADCdata>=156 && ADCdata<=160)
        multiplier=5;
        else if(ADCdata>=161 && ADCdata<=165)
        multiplier=4;
        else if(ADCdata>=166 && ADCdata<=170)
        multiplier=3;
        else if(ADCdata>=171 && ADCdata<=175)
        multiplier=2;
        else if(ADCdata>=176)
        multiplier=1;
        percentspeed=(21-multiplier)*5;//Calculate the percentage speed based on the multiplier value
        SetSpeed(multiplier*12.75);//12.75 is 5% of 255 so the duty cycle is set accordingly
        printf("\n\r Speed updated to %d %c of duty cycle by Flex Sensor",percentspeed,37);
}


/*************************************************************************************
Description   : This function is used to map the output of the accelerometer to varying
                duty cycles after reading them from the ADC
Input         : N/A
Output        : N/A
*************************************************************************************/
//The ADC output for the accelerometer was intially tested to obtain the below used values
/**Since the Vref(+) and Vref(-) was 5V and 0V respectively and the output at 0g for the accelerometer
is close to 3V so a very small voltage range on the accelerometer is mapped to a wide range on the ADC*/
void ACCLToMotorSpeed()
{
        int percentspeed=0;
        if(ADCdata>=68 && ADCdata<=69)
        multiplier=18;
        else if(ADCdata>=70 && ADCdata<=71)
        multiplier=17;
        else if(ADCdata>=72 && ADCdata<=73)
        multiplier=16;
        else if(ADCdata>=74 && ADCdata<=75)
        multiplier=15;
        else if(ADCdata>=76 && ADCdata<=77)
        multiplier=14;
        else if(ADCdata>=78 && ADCdata<=79)
        multiplier=13;
        else if(ADCdata>=80 && ADCdata<=81)
        multiplier=12;
        else if(ADCdata>=82 && ADCdata<=83)
        multiplier=11;
        else if(ADCdata>=84 && ADCdata<=85)
        multiplier=10;
        else if(ADCdata>=86 && ADCdata<=87)
        multiplier=9;
        else if(ADCdata>=88 && ADCdata<=89)
        multiplier=8;
        else if(ADCdata>=90 && ADCdata<=91)
        multiplier=7;
        else if(ADCdata>=92 && ADCdata<=93)
        multiplier=6;
        else if(ADCdata>=94 && ADCdata<=95)
        multiplier=5;
        else if(ADCdata>=96 && ADCdata<=97)
        multiplier=4;
        else if(ADCdata>=98 && ADCdata<=99)
        multiplier=3;
        else if(ADCdata>=100 && ADCdata<=101)
        multiplier=2;
        else if(ADCdata>=102)
        multiplier=1;
        percentspeed=(21-multiplier)*5;//Calculate the percentage speed based on the multiplier value
        SetSpeed(multiplier*12.75);//12.75 is 5% of 255 so the duty cycle is set accordingly
        printf("\n\r Speed updated to %d %c of duty cycle by Accelerometer",percentspeed,37);
}
/*************************************************************************************
Description   : This function starts timer0 for the first time
Input         : N/A
Output        : N/A
*************************************************************************************/
void timer0() //Start Timer 0
{

    TL0=0x4B;
    TH0=0xFC;
    TR0=1;
}

/*************************************************************************************
Description   : This function is used to intialize the various pins of the ADC before
                a conversion is performed
Input         : N/A
Output        : N/A
*************************************************************************************/


void ADCInit()
{
   ADCdata=0x00;//Intialize to zero
    EOC=1;//Set End of Conversion to high
    ALE=0;//Set Address Latch Enable to low
    OE=0;//Set Output Enable to low
    SC=0;//Set start conversion to low
    P0=0xFF;
}

/*************************************************************************************
Description   : This function is used to generate a clock pulse for the ADC to sample
                data
Input         : N/A
Output        : N/A
*************************************************************************************/

void ADCClk()
{
    CLK=1;//Toggle the clock pin high and low after a short delay
    FastDelay(1);
    CLK=0;
    FastDelay(1);

}

/*************************************************************************************
Description   : This is the ISR for timer 0 running in mode 1. The code inside the ISR
                is executed every 1 second. Every time the code is executed, the ADC
                reads the values from the flex sensor or accelerometer depending upon
                which one is currently activated and then the duty cycle is adjusted
                based on the output from the ADC
Input         : N/A
Output        : N/A
*************************************************************************************/

void timer0interrupt() __interrupt(1) // ISR for the Timer0 mode1
{//Sensor value 0 indicates flex sensor
    if(interrupt_counter==20 && sensor==FLEX)//Execute the ISR code once every second since the timer is set for 50ms so 20 iterations equal 1 second
    {
        interrupt_counter=0;//Variable used to track the number of times the ISR is entered
        ADDRESS_A=0;//Set the address pins of the ADC to 000 for selecting input channel 0
        ADDRESS_B=0;
        ADDRESS_C=0;

        TR0=0;
        TL0=0xFC; //Load 0x4BFC to obtain timing of 50 ms.
        TH0=0x4B;

        TR0=1;
        ALE=0;//Latch the address by providing low to high pulse
        ADCClk();
        FastDelay(5);
        ALE=1;
        ADCClk();
        FastDelay(5);

        ADCClk();
        SC=0;//Start conversion by providing low to high pulse to the SC pin
        ADCClk();
        FastDelay(5);
        ADCClk();
        SC=1;
        ADCClk();
        FastDelay(5);

        ALE=0;
        ADCClk();
        FastDelay(5);
        SC=0;
        ADCClk();

        while (EOC == 0)//Poll for EOC to go high indicating that conversion is complete
        {
        ADCClk();
        }

        OE=1;
        ADCClk();
        FastDelay(5);

        ADCdata=P0;//Store the output in a variable
        ADCClk();
        OE=0;
        ADCClk();

        ADCToMotorSpeed();//Update the duty cycle of the motor based on the current reading
    }
    else if(interrupt_counter==20&&sensor==ACCEL)
    {
        interrupt_counter=0;//Variable used to track the number of times the ISR is entered
        ADDRESS_A=1;//Set the address pins of the ADC to 001 for selecting input channel 1
        ADDRESS_B=0;
        ADDRESS_C=0;
        TR0=0;
        TL0=0xFC; //Load 0x4BFC to obtain timing of 50 ms.
        TH0=0x4B;
        TR0=1;
        ALE=0;//Latch the address by providing low to high pulse
        ADCClk();
        FastDelay(5);
        ALE=1;
        ADCClk();
        FastDelay(5);

        ADCClk();
        SC=0;//Start conversion by providing low to high pulse to the SC pin
        ADCClk();
        FastDelay(5);
        ADCClk();
        SC=1;
        ADCClk();
        FastDelay(5);

        ALE=0;
        ADCClk();
        FastDelay(5);
        SC=0;
        ADCClk();

        while (EOC == 0)//Poll for EOC to go high indicating that conversion is complete
        {
            ADCClk();
        }

        OE=1;
        ADCClk();
        FastDelay(5);

        ADCdata=P0;
        ADCClk();
        OE=0;
        ADCClk();

        ACCLToMotorSpeed();//Update the duty cycle of motr based on accelerometer output


    }
    interrupt_counter++;//Increment the counter to track number of times entered into the ISR
}

/*************************************************************************************
Description   : This is the external hardware interrupt ISR which is triggered every
                time a push button switch is pressed. It allows the user to shift
                control between the flex sensor and the accelerometer
Input         : N/A
Output        : N/A
*************************************************************************************/

void external_interrupt()interrupt 0//ISR for external hardware interrupt 0
{
    sensor=!(sensor);//Toggle the value of sensor
    printf_tiny("\n\r Sensor source changed");
}

//Main Program
void main()
{

    ADCInit();//Initialize the ADC
    sensor=FLEX;//Set default sensor value to flex sensor
    duty_cycle=0x99;//Set default duty cycle to 40%
    PWMON();//Turn on the motor
    interrupt_counter=0;//Initialize the counter to zero
    TCON|=0x01;//Enabled falling edge triggered interrupts
    IEN0=0x83;//Enable timer 0 interrupt and external hardware 0 interrupt

    timer0();//Start timer 0
    while(1);



}






