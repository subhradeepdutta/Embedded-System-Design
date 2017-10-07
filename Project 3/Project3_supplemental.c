/*********************************
*Name:Subhradeep Dutta
*Embedded System Design ECEN 5613
*Lab 3 Supplemental
*This program provides a UI to 
*test the PWM functionality and 
*high speed output mode on the 
*AT89C51RC2 
*********************************/

//Including the required header files for 8051 microcontroller
#include <at89c51ed2.h>
#include <mcs51reg.h>
#include <stdio.h>
#include <malloc.h>
#include <stdlib.h>

int duty_cycle;

/*****************************************************STARTUP CODE********************************************************************/

_sdcc_external_startup()//Startup function that is executed first when the microcontroller is turned on
{

    TI=0;        //Set TI flag to 0
    AUXR|=0x0C;  //Set the XRS1:XRS0 bit to 1 to enable 1 KB of internal extended RAM
    TMOD=0X20;   //Enable Timer 1 Mode 2
    TH1=0xFD;    //Load TH1 with FD for 9600 baud rate
    SCON=0X50;   //Select Mode 1 for full duplex with variable baud rate
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

/********************************************************PWM INIT DEFINITION************************************************************/

void PWMInit()
{
    CMOD&=0x79;//Enable PCA Counter during idle mode and set Fclk/6
    CCAPM0|=0x42;// Enable PWM mode and enable comparator
    CCAP0L=duty_cycle;//Set to 0x99 which is 153 in decimal, 153/255=60% which is the Toff so Ton is 40%
    CCAP0H=duty_cycle;//Set to 40% duty cycle initially
    CL=0x00;
    CH=0x00;

}

/********************************************************INTERRUPT HANDLER************************************************************/

void INT0_ISR(void) interrupt 0
{
    IEN0=0x00;//Dsiable all interrupts
}

/********************************************************HIGH SPEED DEFINITION*********************************************************/

void HighSpeed()
{
    CMOD|=0x02; //Select Fclk/2 which is 5.52MHz
    CL=0x00;
    CH=0x00;
    CCAPM1=0x4C;//This enables the comparator, match and toggle
    CCON|=0x40;//Turns on the timer
}

/********************************************************PWMON DEFINITION************************************************************/

void PWMON()
{
    PWMInit();
    CCON|=0x40;//Enable PCA Counter Run Control Bit


}

/********************************************************PWMOFF DEFINITION************************************************************/

void PWMOFF()
{
    CCON&=0xBF;//Disable timer
}

/***********************************************************MAIN PROGRAM***************************************************************/

void main()
{

    //Declare variables
    char user_input[1];
    int choice;
for(;;)
{
    while(1)
    {
        printf("Please select one of the following options\n\r  \
                \n\r1. Turn on PWM\n\r  \
                \n\r2. Turn off PWM\n\r \
                \n\r3. Increase PWM duty cycle by 5%% \n\r \
                \n\r4. Decrease PWM cycle by 5%% \n\r \
                \n\r5. Enter IDLE Mode \n\r   \
                \n\r6. Enter Power Down Mode\n\r \
                \n\r7. Enter high speed mode\n\r");
    user_input[0]=getchar();//Store the user input
    putchar(user_input[0]);//Echo back the user input
    printf_tiny("\n\r");
    if(user_input[0]>=48 && user_input[0]<=57)//Condition to check if its a number
    {
        choice=atoi(user_input);//Convert user input to number
        break;
    }
    else
    {
        printf_tiny("Invalid Input\n\r");
        continue;

    }
    }

    switch(choice)
    {
    case 1://Case to start the PWM on module 0
            duty_cycle=0x99;//Set initial duty cycle
            PWMON();//Function call to turn on thw PWM
            printf_tiny("PWM Turned ON\n\r");
            break;

    case 2:
            PWMOFF();//Function call to turn off PWM
            printf_tiny("PWM turned OFF\n\r");
            break;

    case 3:
            PWMOFF();//Disable PWM before increasing duty cycle
           if(duty_cycle>0x0D)
           {
               duty_cycle-=0x0D;//0x0D is used as step value to increase duty cycle by 5% since (5/100)*255=12.75 ~ 13.0 = 0x0D
           }
           else
           {
               printf_tiny("Maximum Limit reached\n\r");
               duty_cycle=0x0D;//Retain the previous valid duty cycle
               PWMON();
               break;
           }
           PWMON();
           printf_tiny("Duty Cycle increased by 5%%\n\r");
            break;
    case 4:
            PWMOFF();
           if(duty_cycle<0xF2)
           {
               duty_cycle+=0x0D;
           }
           else
           {
               printf_tiny("Maximum Limit reached\n\r");
               duty_cycle=0xF2;//Retain the previous valid duty cycle
               PWMON();
               break;
           }
           PWMON();
           printf_tiny("Duty Cycle decreased by 5%%\n\r");
            break;
    case 5:
            printf_tiny("Entering IDLE mode\n\r");
            IEN0=0x81;//Enable all interrupts and external interrupt 0
            PCON|=0x01;//Enable IDLE Mode
            printf_tiny("Resumed from IDLE mode\n\r");
            break;
    case 6:
            printf_tiny("Entering power down mode\n\r");
            IEN0=0x81;//Enable all interrupts and external interrupt 0
            PCON|=0x02;//Enable power down mode
            break;
    case 7:


            CCAP1L = 0x7F;//Writing to CCAP0L before writing to ECOM bit
            CCAP1H = 0x7F;
            HighSpeed();//Function call to start high speed output mode
            printf_tiny("High Speed Output Mode Enabled\n\r");
            break;

    default: break;
    }
}
}






