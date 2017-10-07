/*********************************
*Name:Subhradeep Dutta
*Embedded System Design ECEN 5613
*Lab 4
*This program uses the LCD and
*the serial EEPROM to read and
*write data from both these
*peripheral components
*********************************/


//Include header files

#include <at89c51ed2.h>
#include <mcs51reg.h>
#include <stdio.h>
#include <stdlib.h>
#include "lcd.h"
#include <stdint.h>
#include "EEPROM.h"

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



/********************************************************LCDDELAY DEFINITION***********************************************************
*This function is used as a delay loop to meet the timing requirements for the LCD. The function makes use of one for loop nested
*inside another for loop
*************************************************************************************************************************************/
 void LCDDelay(int a)
 {

     while(a!=0)
     {
     int j;
     int i;
     for(i=0;i<=a;i++)
     {
         for(j=0;j<100;j++)
         {

         }
     }
     a--;

     }

 }

/********************************************************LCDBUSYWAIT DEFINITION*******************************************************
*This function polls the LCD busy flag(DB7) and returns once the flag is ser to ZERO
*************************************************************************************************************************************/
void LCDbusywait()
{
xdata uint8_t* LCD;
LCD=(xdata uint8_t*)LCDPTR;

do
{
    RS=0;
    RW=1;
}
    while(((*LCD)&SETMSBBIT));


}

 /********************************************************LCDGETADDR DEFINITION*******************************************************
 *This function wait for the busy flag and then returns the current address which is the last 7 bits
*************************************************************************************************************************************/
int LCDgetaddr()
{
    char lcd_en;
    uint8_t addr;
    xdata uint8_t* LCD;
    LCD=(xdata uint8_t*)LCDPTR;



    RS=0;
    RW=1;

    lcd_en=(*LCD)&SETMSBBIT;//Read the busy flag and store it
    while(lcd_en!=0)//Keep polling the busy flag
    {
        lcd_en=(*LCD)&SETMSBBIT;
    }
    addr=(*LCD)&0x7F; //Store the last 7 bits which contains the current address
    return(addr);



}

/********************************************************LCDINIT DEFINITION**********************************************************
*This function sends commands to the LCD to initialize it
*************************************************************************************************************************************/

 void LCDInit()
 {

    xdata uint8_t* LCD;
    LCD=(xdata uint8_t*)LCDPTR;

    LCDDelay(20);
    RS=0;
    RW=0;
    LCDDelay(2);
    *LCD=0x30;//Unlock command
    LCDDelay(2);


    LCDDelay(10);
    RS=0;
    RW=0;
    LCDDelay(2);
    *LCD=0x30;//Unlock command
    LCDDelay(2);


    LCDDelay(10);
    RS=0;
    RW=0;
    LCDDelay(2);
    *LCD=0x30;//Unlock command
    LCDDelay(2);


    LCDbusywait();//Polling for Busy Flag == 0
    RS=0;
    RW=0;
    LCDDelay(2);
    *LCD=0x38;//Function Set command
    LCDDelay(2);


    LCDbusywait();//Polling for Busy Flag == 0
    RS=0;
    RW=0;
    LCDDelay(2);
    *LCD=0x08;//Turning the display off
    LCDDelay(2);


    LCDbusywait();//Polling for Busy Flag == 0
    RS=0;
    RW=0;
    LCDDelay(2);
    *LCD=0x0C;//Turning the display on
    LCDDelay(2);


    LCDbusywait();//Polling for Busy Flag == 0
    RS=0;
    RW=0;
    LCDDelay(2);
    *LCD=0x06;//Entry mode set command
    LCDDelay(2);


    LCDbusywait();//Polling for Busy Flag == 0
    RS=0;
    RW=0;
    LCDDelay(2);
    *LCD=0x01;//Clear screen and send the cursor home
    LCDDelay(2);


    }

/********************************************************LCDGOTOADDR DEFINITION*********************************************************
*This function takes a LCD DDRAM address as input and sets the cursor to that position
****************************************************************************************************************************************/

void LCDgotoaddr(unsigned char addr)
{
    xdata uint8_t* LCD;
    LCD=(xdata uint8_t*)LCDPTR;

    LCDbusywait();
    RS=0;
    RW=0;


    *LCD=SETMSBBIT|addr;//Change the cursor position by writing new value to the LCD


}

/********************************************************LCDGOTOXY DEFINITION*********************************************************
*This function takes a row and column number as input and translates that to the LCD address and passes it to the LCDGOTOADDR function
***************************************************************************************************************************************/

void LCDgotoxy(unsigned char row, unsigned char column)
{
    if(row==0)
        LCDgotoaddr(column);
    else if(row==1)
        LCDgotoaddr(LCDROW2+column);
    else if(row==2)
        LCDgotoaddr(LCDROW3+column);
    else if(row==3)
        LCDgotoaddr(LCDROW4+column);

}

/********************************************************LCDPUTCH DEFINITION**********************************************************
*This function takes a character as input and displays it on the LCD screen
**************************************************************************************************************************************/

void LCDputch(char c)
{
    xdata uint8_t* LCD;
    LCD=(xdata uint8_t*)LCDPTR;

    LCDbusywait();
    RS=1;
    RW=0;

    *LCD=c;//Write the character to the LCD



}

/********************************************************LCDCOMMAND DEFINITION********************************************************
*This function is used to send commands to the LCD
**************************************************************************************************************************************/

void LCDCommand(char a)
{
    xdata uint8_t* LCD;
    LCD=(xdata uint8_t*)LCDPTR;

    LCDbusywait();
    RS=0;
    RW=0;

    *LCD=a;//Write command to the LCD


}

/********************************************************LCDPUTSTR DEFINITION********************************************************
*This function repeatedly calls LCDPUTCH function to write a series of characters to the LCD screen and it also wraps text once it
*reaches the edge of a particular line
**************************************************************************************************************************************/

void LCDputstr(char *a)
{
    int i=0;
    int flag=0;
    volatile int P;


    while(*(a+i)!='\0')
    {
    P=LCDgetaddr();
    if(P==0x0F)
        flag=1;
    else if(P==0x4F)
        flag=2;
    else if(P==0x1F)
        flag=3;
    else if(P==0x5F)
        flag=4;

        LCDputch(*(a+i));
        i++;
    if(flag==1)
        {
            LCDgotoaddr(LCDROW2);
            flag=0;
        }
    else if(flag==2)
        {
            LCDgotoaddr(LCDROW3);
            flag=0;
        }
    else if(flag==3)
        {
            LCDgotoaddr(LCDROW4);
            flag=0;
        }
    else if(flag==4)
        {
            LCDgotoaddr(LCDROW1);
            flag=0;
        }

    }
}

/********************************************************LCDDEMO DEFINITION************************************************************
*This function is used to demo all the functions implemented for the LCD
**************************************************************************************************************************************/

void LCDDemo()
{
    char buffer[64];//Create a buffer to store the data to be displayed on the LCD
    uint8_t row, column;
    int i=0;
    unsigned char temp=DEC_ZERO, temp1=DEC_ZERO;
    printf_tiny("\n\r Enter string to be displayed on the LCD screen\n\r Hit enter to terminate\n\r");

    while(i<64)
    {
        *(buffer+i)=getchar();//Get input from the user
        putchar(*(buffer+i));//Print back the entered character
        if(*(buffer+i)=='\r')//Substitute return character with null
        {
            *(buffer+i)='\0';
            break;
        }
            i++;
    }

    printf_tiny("\n\r Enter the row location from 0 to 3 \n\r ");
    row=getchar();
    putchar(row);
    row=DecDigit(row);
    printf_tiny("\n\r Enter 2 digits for the column location from 00 to 15\n\r ");
    temp=getchar();
    putchar(temp);
    temp1=getchar();
    putchar(temp1);
    temp=DecDigit(temp);
    temp1=DecDigit(temp1);
    column=temp*10+temp1;
    LCDCommand(0x01);//Clear the screen
    LCDgotoxy(row,column);//Set cursor

    LCDputstr(buffer);//Print the series of characters


}

/********************************************************RTLCD DEFINITION******************************************************
*This function is used to print on the LCD as soon as the user hits the keys on the keyboard
******************************************************************************************************************************/
void RTLCD()
{
    int flag=0;
    volatile int P;
    unsigned char c;
    LCDgotoaddr(0x00);
    printf_tiny("\n\rEnter characters \n\r");
    printf_tiny("\n\rHit Enter key to terminate \n\r");
    while(1)
    {
    c=getchar();
    P=LCDgetaddr();
    if(P==0x0F)
        flag=1;
    else if(P==0x4F)
        flag=2;
    else if(P==0x1F)
        flag=3;
    else if(P==0x5F)
        flag=4;

        if(c=='\r')
            return;
        else
        {
            LCDputch(c);
            putchar(c);
        }

    if(flag==1)
        {   //Check for end of the line to wrap around to the next line
            LCDgotoaddr(LCDROW2);
            flag=0;
        }
    else if(flag==2)
        {
            LCDgotoaddr(LCDROW3);
            flag=0;
        }
    else if(flag==3)
        {
            LCDgotoaddr(LCDROW4);
            flag=0;
        }
    else if(flag==4)
        {
            LCDgotoaddr(LCDROW1);
            flag=0;
        }

    }
}

/******************************************************DDRAMDUMP DEFINITION***********************************************************
*This function is used to dump the contents of DDRAM in hex format
**************************************************************************************************************************************/

void DDRAMDump()
{
    unsigned char addr=HEX_ZERO;
    unsigned char dump=HEX_ZERO;
    int i=0;
    xdata uint8_t* LCD;
    LCD=(xdata uint8_t*)LCDPTR;
    LCDbusywait();
    //Set DDRAM address
    RS=0;
    RW=0;

    *LCD=0x80;

    printf_tiny("\n\r");
    printf_tiny("\n\r============DDRAM DUMP=============\n\r");

    LCDgotoaddr(0x00);
    while(i<40)
    {
        LCDbusywait();
        RS=1;
        RW=1;

        dump=*LCD;
        if(i%16==0)
            printf_tiny("\n\r");
        printf("0x%02x %x ",addr, dump);
        addr++;
        i++;
    }
    printf_tiny("\n\r");
    i=0;
    LCDgotoaddr(0x40);
    addr=0x40;
    while(i<40)
    {
        LCDbusywait();
        RS=1;
        RW=1;

        dump=*LCD;
        if(i%16==0)
            printf_tiny("\n\r");
        printf("0x%02x %x ",addr, dump);
        addr++;
        i++;
    }

}


/******************************************************CGRAMDUMP DEFINITION*****************************************************
*This function is used to write a value to the CGRAM and then dump the contents of CGRAM in hex format
********************************************************************************************************************************/

void CGRAMDump()
{
    unsigned char addr=HEX_ZERO;
    unsigned char dump=HEX_ZERO;
    int i=0;
    unsigned char dataval=0x00;
    char high, low;
    xdata uint8_t* LCD;
    LCD=(xdata uint8_t*)LCDPTR;

    printf_tiny("\n\rEnter value to be written to the CGRAM\n\r");
    high=getchar();
    putchar(high);
    low=getchar();
    putchar(low);
    high=HexDigit(high);
    low=HexDigit(low);
    if(high<0||low<0)//Validate user input
    {
        printf_tiny("\n\rInvalid Input\n\r");
        return;
    }
    dataval|=high;
    dataval=dataval<<4;
    dataval|=low;//Combine user input into a single value

    LCDbusywait();
    RS=0;
    RW=0;

    *LCD=0x40;

    while(addr<=0x3F)//Loop until the last address of CGRAM
    {
        RS=1;
        RW=0;

        *LCD=dataval;//Write data to the CGRAM
        addr++;

    }

    printf_tiny("\n\r");
    printf_tiny("\n\r===========CGRAM DUMP=============\n\r");
    addr=HEX_ZERO;
    LCDbusywait();
    RS=0;
    RW=0;

    *LCD=0x40;


    while(addr<=0x3F)
    {
        LCDbusywait();
        //Command to read from CGRAM
        RS=1;
        RW=1;

        dump=*LCD;
        if(i==5)
        {
            i=0;
            printf_tiny("\n\r");
        }
        printf("%04x %x ",addr, dump);
        addr++;

        i++;
    }

}
