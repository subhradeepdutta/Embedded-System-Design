/*********************************
*Name:Subhradeep Dutta
*Embedded System Design ECEN 5613
*Lab 4
*This program uses the LCD and
*the serial EEPROM to read and
*write data from both these
*peripheral components
*It also implements a clock and 3
*different alarm clocks and the
*capability to generate custom
*characters as per the user's
*choice
*********************************/


//Include header files

#include <at89c51ed2.h>
#include <mcs51reg.h>
#include <stdio.h>
#include <stdlib.h>
#include "lcd.h"
#include <stdint.h>
#include "EEPROM.h"
#include "CustomChar.h"
#include "IOEXP.h"


//Define constants


#define DEVICE_ID 0xA0
#define HEX_ZERO 0x00
#define MSB_BIT 0x80
#define DEC_ZERO 0
#define ENABLEXRAM 0x0C
#define LCDPTR 0x8000
#define SETMSBBIT 0x80
#define LCDROW2 0x40
#define LCDROW3 0x10
#define LCDROW4 0x50
#define LCDROW1 0x00


//Declare global variables
int WDTflag;
int counter=0;
static char user_option;
static char alarmenable[3] = {0, 0, 0};
static int t = 0;
char clk_inp;
static unsigned int minute, sec, one_tenth_sec = 0;
static unsigned int one_tenth;
static unsigned int alarm_one_tenth_sec[3] = {0, 0, 0};
static int alarm_one_tenth[3];
static int alarm_min[3], alarm_sec[3];


//Declare function prototypes
void timer0();
void WDT();
void WDTReset();
void WDTDemo();
void Infinite();
void IOEXPDemo();
void reset();
void stop();
void start();
void IOEXPOutput();
void timer();
void Alarm1();
void Alarm2();
void Alarm3();




/*****************************************************STARTUP CODE********************************************************************
*This function is executed at the beginning even before main is executed
*************************************************************************************************************************************/
_sdcc_external_startup()//Startup function that is executed first when the microcontroller is turned on
{

    TI=DEC_ZERO;        //Set TI flag to 0
    AUXR|=ENABLEXRAM;   //Set the XRS1:XRS0 bit to 1 to enable 1 KB of internal extended RAM
    TMOD=0X21;          //Enable Timer 1 Mode 2 and Timer 0 mode 1
    TH1=0xFF;           //Load TH1 with FF for 57600 baud rate
    SCON=0x50;          //Enable Timer 1 Mode 2
    PCON|=0x80;         //Set SMOD=1 for higher baud rate of 57600
    TR1=1;              //Start Timer
    WDTflag=0;          //Flag to conditionally reset watchdog timer when the timer is enabled
    return 0;
}


/*****************************************************TIMER0 FUNCTION*****************************************************************
*This function is used to load the timer for the first time from the main function
*************************************************************************************************************************************/

void timer0() //Start Timer 0
{

    TL0=0x66;
    TH0=0xFC;
    TR0=1;
}
/*****************************************************WDT FUNCTION********************************************************************
*This function initializes and enables the watchdog timer
*************************************************************************************************************************************/

void WDT()
{

    WDTPRG|=0x07;//Procedure to initiate the watchdog timer and configure the timeout counter to 2.27 seconds for 11.0592 MHz
    WDTRST=0x1E;
    WDTRST=0xE1;

}

/*****************************************************WDTRESET FUNCTION***************************************************************
*This function feeds the watchdog timer and is called repeatedly in the program to prevent the watchdog from resetting the controller
*************************************************************************************************************************************/

void WDTReset()
{

    WDTRST=0x1E;//Procedure to reload the watchdog timer as per the datasheet
    WDTRST=0xE1;


}

/*****************************************************WDTDEMO FUNCTION***************************************************************
*This function calls the watchdog enable function and sets the watchdog flag to 1 so that the watcdog reset function keeps reloading
*the watchdog counter
*************************************************************************************************************************************/

void WDTDemo()
{
    printf_tiny("\nWATCH DOG TIMER ENABLED\n\r");
    WDT();
    WDTflag=1;
}

/*****************************************************INFINITE FUNCTION***************************************************************
*This function simulates a hang like by executing an infinite while loop and this prevents the watchdog reset function from reloading
*the watchdog timer resulting in a system reset to come out of the infinite loop
*************************************************************************************************************************************/

void Infinite()
{
    printf("\n\rEntering an infinite loop");
    WDT();
    while(1)
    {

    }
}

/*****************************************************IOEXPDEMO FUNCTION***************************************************************
*This function configures the IO Expander to increment a counter on the button press
*************************************************************************************************************************************/

void IOEXPDemo()
{

    unsigned char mask=0x00;
    unsigned char datavalue=IOEXPRead();
    unsigned char temp;
    unsigned char temp1;
    unsigned char c;
    IEN0|=0x01;                 //Enable external hardware interupt
    TCON|=0x01;                 //Enable edge triggered interrupts for external hardware interrupt so that a longer button press does not trigger multiple calls to the ISR
    LCDgotoaddr(LCDROW2);
    LCDputch(HexToASCII(counter));  //Display the counter on the LCD
    printf_tiny("\n\r\Press Y to reset the counter value\n\r"); //Provide option to the user to reset the counter
    c=getchar();
    putchar(c);
    if(c=='Y')
    {
        counter=0;
        LCDgotoaddr(LCDROW2);
        LCDputch(HexToASCII(counter));
        printf("\n\r Counter value reset");
    }

    printf_tiny("\n\rEnter the mask value\n\r");
    temp=getchar();
    putchar(temp);
    temp1=getchar();
    putchar(temp1);
    temp=HexDigit(temp);
    temp1=HexDigit(temp1);
    mask=temp;
    mask=mask<<4;
    mask|=temp1;                //Combining the user inputs into one variable


    IOEXPWrite((mask));         //Configure the IO pins as per the input from the user
}

/*****************************************************EXTERNAL INTERRUPT ISR***********************************************************
*This function configures the IO Expander to increment a counter on the button press which triggers the external hardware interrupt 0
*************************************************************************************************************************************/


void external_interrupt()interrupt 0//ISR for external hardware interrupt 0
{
    if(counter<0x0F)
    {
        counter++;
        LCDgotoaddr(LCDROW2);
        LCDputch(HexToASCII(counter));
    }
    else
    {
        counter=0;
        LCDgotoaddr(LCDROW2);
        LCDputch(HexToASCII(counter));
    }

    printf_tiny("\n\rCounter incremented\n\r");
}

/*****************************************************RESET FUNCTION*****************************************************************
*This function resets the clock timer
*************************************************************************************************************************************/

void reset() //Function to reset the timer
{
    printf_tiny("\n\Reset Timer\n\r");
    minute=0;
    sec=0;
    one_tenth_sec = 0;
    one_tenth=0;
}

/*********************************************************STOP FUNCTION***************************************************************
*This function stops the clock timer
*************************************************************************************************************************************/
void stop() //Function to pause or stop the timer
{
    printf_tiny("\n\Timer Off \n\r");
    TR0=0;
}

/*******************************************************START FUNCTION***************************************************************
*This function starts the clock timer
*************************************************************************************************************************************/

void start() //Function to stop/pause the timer
{
    printf_tiny("\n\Timer on \n\r");
    TL0=0x66;
    TH0=0xFC;
    TR0=1;
}

/***********************************************************TIMER0 ISR***************************************************************
*This function executes when the timer overflows which is every 50 ms and it checks for the status of the alarms and accordingly
*updates the alarm value on the LCD
*************************************************************************************************************************************/

void timer0interrupt() __interrupt(1) // ISR for the Timer0 mode1
{
    TR0=0;
    TL0=0xFC; //Load 0x4BFC to obtain timing of 50 ms.
    TH0=0x4B;
    TF0=0;
    TR0=1;
    if(t==2) //For every two times the ISR is executed this block is executed
    {
        t=0;//Reset the flag back to zero
        LCDgotoaddr(0x58);
        one_tenth = one_tenth_sec%10;
        sec=(one_tenth_sec/10)%60;
        minute = (one_tenth_sec/(60*10))%60;
        LCDputch(((minute/10)+0x30)); //Update the tens place minutes value on the LCD for the clock by dividing by 10
        LCDputch(((minute%10)+0x30)); //Update the ones place minutes value on the LCD for the clock by taking the remainder after division by 10
        LCDputch(':');
        LCDputch(((sec/10)+0x30));//Update the seconds value on the LCD for the clock
        LCDputch(((sec%10)+0x30));//Ones place value for seconds obtained by remainder after division by 10
        LCDputch(':');
        LCDputch((one_tenth+0x30));//Update the deciseconds value on the LCD for the clock
        one_tenth_sec++;


     if (alarmenable[0]==1)//Check if the alarm is enabled
        {
           alarm_one_tenth_sec[0]=0;

            if (alarm_one_tenth_sec[0] == 0)
            {
                alarmenable[0]=0;//Disable the alarm flag
                LCDgotoaddr(0x00);
                LCDputch('1');//Denotes alarm 1 is set off by putting the alarm number on the LCD
            }
            LCDgotoaddr(0x09);
            alarm_one_tenth[0]=alarm_one_tenth_sec[0]%10;
            alarm_sec[0]=(alarm_one_tenth_sec[0]/10)%60;
            alarm_min[0]=(alarm_one_tenth_sec[0]/(60*10))%60;
            LCDputch(((alarm_min[0]/10)+0x30));//Update the minutes value on the LCD for alarm
            LCDputch(((alarm_min[0]%10)+0x30));//0x30  is added to convert back to ASCII
            LCDputch(':');
            LCDputch(((alarm_sec[0]/10)+0x30));//Update the  seconds value on the LCD for alarm
            LCDputch(((alarm_sec[0]%10)+0x30));//0x30  is added to convert back to ASCII
            LCDputch(':');
            LCDputch((alarm_one_tenth[0]+0x30));//Update the deciseconds value on the LCD

        }

    if (alarmenable[1]==1)//Check if alarm 2 is enabled
        {
            alarm_one_tenth_sec[1]=0;

            if (alarm_one_tenth_sec[1]==0)
            {
                alarmenable[1]=0;
                LCDgotoaddr(0x40);
                LCDputch('2');//Denotes alarm 2 is set off by putting the alarm number on the LCD
            }
            LCDgotoaddr(0x49);
            alarm_one_tenth[1]=alarm_one_tenth_sec[1]%10;
            alarm_sec[1]=(alarm_one_tenth_sec[1]/10)%60;
            alarm_min[1]=(alarm_one_tenth_sec[1]/(60*10))%60;
            LCDputch(((alarm_min[1]/10)+0x30));//Update the minutes value on the LCD for alarm
            LCDputch(((alarm_min[1]%10)+0x30));//0x30  is added to convert back to ASCII
            LCDputch(':');
            LCDputch(((alarm_sec[1]/10)+0x30));//Update the  seconds value on the LCD for alarm
            LCDputch(((alarm_sec[1]%10)+0x30));//0x30  is added to convert back to ASCII
            LCDputch(':');
            LCDputch((alarm_one_tenth[1]+0x30));//Update the deciseconds value on the LCD for the clock

        }

    if (alarmenable[2]==1)
        {
            alarm_one_tenth_sec[2]=0;

            if (alarm_one_tenth_sec[2]==0)
            {
                alarmenable[2]=0;
                LCDgotoaddr(0x10);
                LCDputch('3');
            }
            LCDgotoaddr(0x19);
            alarm_one_tenth[2]=alarm_one_tenth_sec[2]%10;
            alarm_sec[2]=(alarm_one_tenth_sec[2]/10)%60;
            alarm_min[2]=(alarm_one_tenth_sec[2]/(60*10))%60;
            LCDputch(((alarm_min[2]/10)+0x30));
            LCDputch(((alarm_min[2]%10)+0x30));
            LCDputch(':');
            LCDputch(((alarm_sec[2]/10)+0x30));
            LCDputch(((alarm_sec[2]%10)+0x30));
            LCDputch(':');
            LCDputch((alarm_one_tenth[2]+0x30));

        }

    }
    t++;//Increment the flag
}

/*******************************************************TIMER FUNCTION***************************************************************
*This function provides additional configuration options for the clock timer
*************************************************************************************************************************************/


void timer()
{
    for(;;)
    {
    printf("\n\r Enter s to start timer\n\r Enter p to stop timer\n\r Enter r to reset timer\n\r Enter c to return to main menu\n\r");
    clk_inp=getchar();
	putchar(clk_inp);

    switch(clk_inp)
    {   case 's':start();//Call the start function to start or resume the clock timer
                break;
        case 'p': stop();//Call the stop function to stop or pause the timer
                break;
        case 'r': reset();//Call the reset function to reset the timer
                break;
        case 'c':return;//Return to the main menu
                break;
        default:printf_tiny("Invalid Input");
                break;
    }
    }

}

/*******************************************************IOEXPOUTPUT FUNCTION*********************************************************
*This function outputs the counter value in hexadecimal on the lower nibble of the IO Expander
*************************************************************************************************************************************/


void IOEXPOutput()
{
    unsigned char mask=0xF0;//Mask to make the lower nibble ports as outputs
    unsigned char current_value=IOEXPRead();//Read current status of the pins in the IOExpander
    IOEXPWrite(mask&current_value);//Setting the lower nibbble pins as output
    current_value=IOEXPRead();
    IOEXPWrite(current_value|counter);//Output the current value of the counter on the lower nibble ports

}

/*******************************************************ALARM1 FUNCTION**************************************************************
*This function sets up the first alarm
*************************************************************************************************************************************/

void Alarm1()
{
    char min1=0, min2=0;
    while (1)
    {


        printf_tiny("\n\r Press 1 to edit Alarm 1: mm:ss:s");
        printf_tiny("\n\r Press 2 enable Alarm");
        printf_tiny("\n\r Press 3 to disable Alarm");
        printf_tiny("\n\r Press 4 to return to main menu");

        clk_inp=getchar();

        user_option = DecDigit(clk_inp);//Convert from ASCII to decimal

        if (user_option==1)//Edit alarm option
        {
            if (alarmenable[0]==0)//Check if alarm enabled
            {
                LCDgotoaddr(0x09);
                LCDputch(((alarm_min[0]/10)+0x30));//Update the minutes value
                LCDputch(((alarm_min[0]%10)+0x30));//0x30 added to convert to ASCII
                LCDputch(':');
                LCDputch(((alarm_sec[0]/10)+0x30));//Update the seconds value
                LCDputch(((alarm_sec[0]%10)+0x30));
                LCDputch(':');
                LCDputch((alarm_one_tenth[0]+0x30));//Update the deciseconds valie


                while (1)
                {
                    printf_tiny("\n\rEnter minutes in the range of 0-59\t");
                    min1=getchar();
                    putchar(min1);
                    min2=getchar();
                    putchar(min2);
                    min1=DecDigit(min1);
                    min2=DecDigit(min2);
                    alarm_min[0]=10*min1+min2;//Combine user input


                    if ((alarm_min[0]<0)||(alarm_min[0]>59))//Validate user input
                    {
                        printf_tiny("\n\rInvalid Input Try Again\n\r");
                        continue;
                    }

                    break;
                }

                while (1)
                {

                    printf_tiny("\n\rEnter seconds in the range of 0-59:\t");
                    min1=getchar();
                    putchar(min1);
                    min2=getchar();
                    putchar(min2);
                    min1=DecDigit(min1);
                    min2=DecDigit(min2);
                    alarm_sec[0]=10*min1+min2;//Combine user input
                    if ((alarm_sec[0]<0)||(alarm_sec[0]>59))//Validate user input
                    {
                        printf_tiny("\n\rInvalid Input Try Again\n\r");
                        continue;
                    }

                    break;
                }
                alarm_one_tenth[0] = 0;
                LCDgotoaddr(0x09);

                LCDputch(((alarm_min[0]/10)+0x30));//Update the minutes value
                LCDputch(((alarm_min[0]%10)+0x30));
                LCDputch(':');
                LCDputch(((alarm_sec[0]/10)+0x30));//Update the seconds value
                LCDputch(((alarm_sec[0]%10)+0x30));
                LCDputch(':');
                LCDputch((alarm_one_tenth[0]+0x30));//Update the deciseconds value


                alarm_one_tenth_sec[0]=(alarm_min[0]*60*10)+(alarm_sec[0]*10);
            }

            else
            {
                printf_tiny("\n\rDisable alarm first");//Message displayed if user tries to modify the alarm while it is enabled
            }
        }

        else if (user_option == 2)
        {
            alarmenable[0] = 1;//Set the alarm 1 flag
        }

        else if (user_option == 3)
        {
            alarmenable[0] = 0; //Clear the alarm enable 1 flag
            LCDgotoaddr(LCDROW1);
            LCDputch(' '); //Clear the alarm from LCD
        }

        else if (user_option == 4)
        {
            break;
        }

        else
        {
            printf_tiny("\n\rInvalid Input Try Again\n\r");
        }
    }

}

/*******************************************************ALARM2 FUNCTION**************************************************************
*This function sets up the second alarm
*************************************************************************************************************************************/


void Alarm2()
{
    char min1=0, min2=0;
    while (1)
    {


        printf_tiny("\n\r Press 1 to edit Alarm 2: mm:ss:s");
        printf_tiny("\n\r Press 2 enable Alarm");
        printf_tiny("\n\r Press 3 to disable Alarm");
        printf_tiny("\n\r Press 4 to return to main menu");




         clk_inp=getchar();

        user_option=DecDigit(clk_inp);

        if (user_option==1)
        {
            if (alarmenable[1]==0)//Check if alarm is enabled
            {
                LCDgotoaddr(0x49);
                LCDputch(((alarm_min[1]/10)+0x30));//Update the minutes
                LCDputch(((alarm_min[1]%10)+0x30));
                LCDputch(':');
                LCDputch(((alarm_sec[1]/10)+0x30));//Update the seconds
                LCDputch(((alarm_sec[1]%10)+0x30));
                LCDputch(':');
                LCDputch((alarm_one_tenth[1]+0x30));//Update the deciseconds


                while (1)
                {
                    printf_tiny("\n\rEnter minutes in the range of 0-59\t");
                    min1=getchar();
                    putchar(min1);
                    min2=getchar();
                    putchar(min2);
                    min1=DecDigit(min1);
                    min2=DecDigit(min2);
                    alarm_min[1]=10*min1+min2;//Combine user inputs

                    if ((alarm_min[1]<0)||(alarm_min[1]>59))//Validate user input
                    {
                        printf_tiny("\n\rInvalid Input Try Again\n\r");
                        continue;
                    }

                    break;
                }

                while(1)
                {

                    printf_tiny("\n\rEnter seconds in the range of 0-59:\t");
                    min1=getchar();
                    putchar(min1);
                    min2=getchar();
                    putchar(min2);
                    min1=DecDigit(min1);
                    min2=DecDigit(min2);
                    alarm_sec[1]=10*min1+min2;//Combine user inputs
                    if ((alarm_sec[1]<0)||(alarm_sec[1]>59))//Validate user input
                    {
                        printf_tiny("\n\rInvalid Input Try Again\n\r");
                        continue;
                    }

                    break;
                }
                alarm_one_tenth[1]=0;
                LCDgotoaddr(0x49);

                LCDputch(((alarm_min[1]/10)+0x30));//Update the minutes
                LCDputch(((alarm_min[1]%10)+0x30));
                LCDputch(':');
                LCDputch(((alarm_sec[1]/10)+0x30));//Update the seconds
                LCDputch(((alarm_sec[1]%10)+0x30));
                LCDputch(':');
                LCDputch((alarm_one_tenth[1]+0x30));//Update the deciseconds


                alarm_one_tenth_sec[1]=(alarm_min[1]*60*10)+(alarm_sec[1]*10);
            }

            else
            {
                printf_tiny("\n\rDisable alarm first");//Message displayed if user tries to modify the alarm while it is enabled
            }
        }

        else if(user_option==2)
        {
            alarmenable[1]=1;//Set the alarm enabled flag
        }

        else if(user_option==3)
        {
            alarmenable[1]=0;//Disable the alarm flag
            LCDgotoaddr(LCDROW2);
            LCDputch(' ');
        }

        else if(user_option==4)
        {
            break;
        }

        else
        {
            printf_tiny("\n\rInvalid Input Try Again\n\r");
        }
    }

}

/*******************************************************ALARM3 FUNCTION**************************************************************
*This function sets up the third alarm
*************************************************************************************************************************************/


void Alarm3()
{
    char min1=0, min2=0;
    while (1)
    {


        printf_tiny("\n\r Press 1 to edit Alarm 3: mm:ss:s");
        printf_tiny("\n\r Press 2 enable Alarm");
        printf_tiny("\n\r Press 3 to disable Alarm");
        printf_tiny("\n\r Press 4 to return to main menu");




         clk_inp=getchar();

        user_option=DecDigit(clk_inp);

        if(user_option==1)
        {
            if(alarmenable[2]==0)
            {
                LCDgotoaddr(0x19);
                LCDputch(((alarm_min[2]/10)+0x30));//Update the minutes value
                LCDputch(((alarm_min[2]%10)+0x30));
                LCDputch(':');
                LCDputch(((alarm_sec[2]/10)+0x30));//Update the seconds value
                LCDputch(((alarm_sec[2] % 10)+0x30));
                LCDputch(':');
                LCDputch((alarm_one_tenth[2]+0x30));//Update the deciseconds value


                while(1)
                {
                    printf_tiny("\n\rEnter minutes in the range of 0-59\t");
                    min1=getchar();
                    putchar(min1);
                    min2=getchar();
                    putchar(min2);
                    min1=DecDigit(min1);
                    min2=DecDigit(min2);
                    alarm_min[2]=10*min1+min2;//Combine user input


                    if ((alarm_min[2]<0)||(alarm_min[2]>59))//Validate user input
                    {
                        printf_tiny("\n\rInvalid Input Try Again\n\r");
                        continue;
                    }

                    break;
                }

                while(1)
                {
                    printf_tiny("\n\rEnter seconds in the range of 0-59:\t");
                    min1=getchar();
                    putchar(min1);
                    min2=getchar();
                    putchar(min2);
                    min1=DecDigit(min1);
                    min2=DecDigit(min2);
                    alarm_sec[2]=10*min1+min2;//Combine user input
                    if ((alarm_sec[2]<0)||(alarm_sec[2]>59))//Validate user input
                    {
                        printf_tiny("\n\rInvalid Input Try Again\n\r");
                        continue;
                    }

                    break;
                }
                alarm_one_tenth[2]=0;
                LCDgotoaddr(0x19);
                LCDputch(((alarm_min[2]/10)+0x30));
                LCDputch(((alarm_min[2]%10)+0x30));
                LCDputch(':');
                LCDputch(((alarm_sec[2]/10)+0x30));
                LCDputch(((alarm_sec[2]%10)+0x30));
                LCDputch(':');
                LCDputch((alarm_one_tenth[2]+0x30));
                alarm_one_tenth_sec[2]=(alarm_min[2]*60*10) + (alarm_sec[2] * 10);
            }

            else
            {
                printf_tiny("\n\rDisable alarm first");//Message displayed when user tries to edit the alarm while it is enabled
            }
        }

        else if (user_option==2)
        {
            alarmenable[2]=1;//Set the alarm enabled flag
        }

        else if(user_option==3)
        {
            alarmenable[2]=0;//Clear the alarm enabled flag
            LCDgotoaddr(LCDROW3);
            LCDputch(' ');
        }

        else if(user_option==4)
        {
            break;
        }

        else
        {
            printf_tiny("\n\rInvalid Input");
        }
    }

}


/*****************************************************PUTCHAR DEFINITION**************************************************************
*This function takes a character as input and displays it on the serial terminal
*************************************************************************************************************************************/
void putchar (char c)//Definition for putchar function to display characters on the serial terminal
{
    SBUF = c;    //SBUF stores the character to be dsiplayed
	while (!TI)//Wait for TI flag
    {
        if(WDTflag==1) //Reload the watchdog timer
        WDTReset();
    }
	TI=DEC_ZERO;      //Set TI flag to 0

}

/********************************************************GETCHAR DEFINITION***********************************************************
*This function captures user input on the serial terminal
*************************************************************************************************************************************/
char getchar()      //Definition of getchar function to fetch input from the user
{
    while(RI==0)//Wait for RI flag
    {
        if(WDTflag==1)//Reload the watchdog timer
        WDTReset();
    }
    RI=DEC_ZERO;    //Set RI to 0

    return SBUF;//Return the value in the buffer
}





/********************************************************MAIN DEFINITION************************************************************/

void main()
{
    uint8_t choice;
    counter=0;
    alarmenable[0] =0;
    alarmenable[1] =0;
    alarmenable[2] =0;
    alarm_min[0]=0;
    alarm_min[1]=0;
    alarm_min[2]=0;
    alarm_sec[0]=0;
    alarm_sec[1]=0;
    alarm_sec[2]=0;
    alarm_one_tenth_sec[0] = 0;
    alarm_one_tenth_sec[1] = 0;
    alarm_one_tenth_sec[2] = 0;
    alarm_one_tenth[0]=0;
    alarm_one_tenth[1]=0;
    alarm_one_tenth[2]=0;
    minute=0;
    sec=0;
    one_tenth_sec = 0;
    one_tenth=0;
    t = 0;
    WDTflag=0;//Set watchdog timer flag to zero to prevent reloading of watchdog timer
    LCDInit();//Initialize the LCD
    IEN0=0x82;//Enable external interrupt 0 and timer 0 interrupt
    timer0();//Call the timer0 function to start the clock
    printf_tiny("\n\r*****************MENU******************\n\r");
    printf_tiny("\n\rEnter 1 for writing a byte");
    printf_tiny("\n\rEnter 2 for reading a byte");
    printf_tiny("\n\rEnter 3 to clear the LCD screen");
    printf_tiny("\n\rEnter 4 to read from EEPROM and display on LCD");
    printf_tiny("\n\rEnter 5 to dump DDRAM");
    printf_tiny("\n\rEnter 6 to dump CGRAM");
    printf_tiny("\n\rEnter 7 to dump data from EEPROM");
    printf_tiny("\n\rEnter 8 to enter values for custom characters");
    printf_tiny("\n\rEnter 9 for additional timer options");
    printf_tiny("\n\rEnter L to display logo on LCD");
    printf_tiny("\n\rEnter R to reset the EEPROM");
    printf_tiny("\n\rEnter H to simulate system hang");
    printf_tiny("\n\rEnter W to enable watchdog");
    printf_tiny("\n\rEnter I to assign values to I/O Expander ");
    printf_tiny("\n\rEnter S to read values from I/O Expander ");
    printf_tiny("\n\rEnter J to output the counter value on I/O Expander ");
    printf_tiny("\n\rEnter A to configure alarm 1 ");
    printf_tiny("\n\rEnter B to configure alarm 2 ");
    printf_tiny("\n\rEnter C to configure alarm 3 ");
    printf_tiny("\n\rEnter M to print the menu again\n\r");

while(1)
{
    if(WDTflag==1)
        WDTReset();
    printf_tiny("\n\r============================================\n\r");
    printf_tiny("\n\rEnter your choice\n\r");
    choice=getchar();
    putchar(choice);
    switch(choice)
    {
        case '1':if(WDTflag==1)
                    WDTReset();
                WriteByteDemo();
                break;

        case '2':if(WDTflag==1)
                    WDTReset();
                ReadByteDemo();
                break;

        case '3':if(WDTflag==1)
                    WDTReset();
                LCDCommand(0x01);
                break;

        case'4':if(WDTflag==1)
                    WDTReset();
                LCDDisplayDemo();
                break;

        case '5':if(WDTflag==1)
                    WDTReset();
                DDRAMDump();
                break;

        case '6':if(WDTflag==1)
                    WDTReset();
                CGRAMDump();
                break;

        case '7':if(WDTflag==1)
                    WDTReset();
                HexDump();
                break;

        case '8':if(WDTflag==1)
                    WDTReset();
                CustomCharDemo();
                break;

        case '9':if(WDTflag==1)
                    WDTReset();
                timer();
                break;

        case 'M':
        case 'm':
                if(WDTflag==1)
                    WDTReset();
                printf_tiny("\n\r*****************MENU******************\n\r");
                printf_tiny("\n\rEnter 1 for writing a byte");
                printf_tiny("\n\rEnter 2 for reading a byte");
                printf_tiny("\n\rEnter 3 to clear the LCD screen");
                printf_tiny("\n\rEnter 4 to read from EEPROM and display on LCD");
                printf_tiny("\n\rEnter 5 to dump DDRAM");
                printf_tiny("\n\rEnter 6 to dump CGRAM");
                printf_tiny("\n\rEnter 7 to dump data from EEPROM");
                printf_tiny("\n\rEnter 8 to enter values for custom characters");
                printf_tiny("\n\rEnter 9 for additional timer options");
                printf_tiny("\n\rEnter L to display logo on LCD");
                printf_tiny("\n\rEnter R to reset the EEPROM");
                printf_tiny("\n\rEnter H to simulate system hang");
                printf_tiny("\n\rEnter W to enable watchdog");
                printf_tiny("\n\rEnter I to assign values to I/O Expander ");
                printf_tiny("\n\rEnter S to read values from I/O Expander ");
                printf_tiny("\n\rEnter J to output the counter value on I/O Expander ");
                printf_tiny("\n\rEnter A to configure alarm 1 ");
                printf_tiny("\n\rEnter B to configure alarm 2 ");
                printf_tiny("\n\rEnter C to configure alarm 3 ");
                printf_tiny("\n\rEnter M to print the menu again\n\r");
                break;


        case 'L':
        case 'l':
                if(WDTflag==1)
                    WDTReset();
                BatteryLogo();
                break;


        case 'R':
        case 'r':
                if(WDTflag==1)
                    WDTReset();
                eereset();
                break;


        case 'H':
        case 'h':
                if(WDTflag==1)
                    WDTReset();
                Infinite();
                break;



        case 'I':
        case 'i':
                if(WDTflag==1)
                    WDTReset();
                IOEXPDemo();
                break;

        case 'S':
        case 's':
                if(WDTflag==1)
                    WDTReset();
                ReadPinValues();
                break;

        case 'J':
        case 'j':
                if(WDTflag==1)
                    WDTReset();
                IOEXPOutput();
                break;

        case 'A':
        case 'a':
                if(WDTflag==1)
                    WDTReset();
                Alarm1();
                break;

        case 'B':
        case 'b':
                if(WDTflag==1)
                    WDTReset();
                Alarm2();
                break;

        case 'C':
        case 'c':
                if(WDTflag==1)
                    WDTReset();
                Alarm3();
                break;

        case'w':
        case'W':
                if(WDTflag==1)
                    WDTReset();
                WDTDemo();
                break;

    default:printf_tiny("\n\rInvalid Input\n\r");
            if(WDTflag==1)
                WDTReset();
    break;
}

}
}



