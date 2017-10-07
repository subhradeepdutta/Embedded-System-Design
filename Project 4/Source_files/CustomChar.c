//Include header files

#include <at89c51ed2.h>
#include <mcs51reg.h>
#include <stdio.h>
#include <stdlib.h>
#include "lcd.h"
#include <stdint.h>
#include "EEPROM.h"
#include "CustomChar.h"

//Define constants


#define HEX_ZERO 0x00
#define DEC_ZERO 0
#define LCDROW2 0x40
#define LCDROW3 0x10
#define LCDROW4 0x50
#define LCDROW1 0x00
#define LCDPTR 0x8000



/*****************************************************CUSTOMCHAR FUNCTION****************************************************************
*This function accepts the following inputs:
*            ccode which refers to the custom character location on the CGRAM,
*            a pointer to the aray which contains the values passed by the user,
*            the DDRAM memory location to print out the custom character on the LCD
*and outputs the custom character on a user defined LCD location
*************************************************************************************************************************************/

void CustomChar(unsigned char ccode, unsigned char * row_val, unsigned char loc)
{

LCDCommand(0x40+(8*ccode));//Set CGRAM Address to write to
LCDputch(*(row_val++));//Enter the values for row 0 and increment the pointer

LCDputch(*(row_val++));

LCDputch(*(row_val++));

LCDputch(*(row_val++));

LCDputch(*(row_val++));

LCDputch(*(row_val++));

LCDputch(*(row_val++));

LCDputch(*(row_val));

LCDgotoaddr(loc);//Set the location on DDRAM to print out the character
LCDputch(ccode);//Write the corresponding ASCII code to print out the custom character
}
/*****************************************************BATTERYLOGO FUNCTION***********************************************************
*This function creates a custom logo that represents a gradually charging battery |_||_||_||_||_|
*************************************************************************************************************************************/

void BatteryLogo()
{
   int i=0;
   int j=0;
   unsigned char addr=0x00;
   unsigned char cgramaddr=0x40;
   unsigned char loc=0;
   unsigned char battery[][]={
   {0x04,0x1F,0x11,0x11,0x11,0x11,0x11,0x1F},
   {0x04,0x1F,0x11,0x11,0x11,0x11,0x1F,0x1F},
   {0x04,0x1F,0x11,0x11,0x11,0x1F,0x1F,0x1F},
   {0x04,0x1F,0x11,0x11,0x1F,0x1F,0x1F,0x1F},
   {0x04,0x1F,0x11,0x1F,0x1F,0x1F,0x1F,0x1F},
   {0x04,0x1F,0x1F,0x1F,0x1F,0x1F,0x1F,0x1F},
   };//Array containing the values for the battery logo


    for(i=0;i<6;i++)
    {
        j=0;

    LCDCommand(cgramaddr);//Set CGRAM Address
    cgramaddr=cgramaddr+8;
    LCDputch(battery[i][j++]);

    LCDputch(battery[i][j++]);

    LCDputch(battery[i][j++]);

    LCDputch(battery[i][j++]);

    LCDputch(battery[i][j++]);

    LCDputch(battery[i][j++]);

    LCDputch(battery[i][j++]);

    LCDputch(battery[i][j]);

    LCDgotoaddr(addr);
    LCDputch(loc);
    loc++;
    addr++;

    }
}

/*****************************************************CUSTOMCHARDEMO FUNCTION*********************************************************
*This function accepts the inputs for the custom character from the user and passes it to the custom character function
*************************************************************************************************************************************/

void CustomCharDemo()
{
    unsigned char loc=0x00;
    unsigned char row_vals[8];//Array to store user inputs from the user
    char temp;
    char ccode;
    int i=0, j;
    printf_tiny("\n\r Enter the custom character location from 0-7\n\r");
    ccode=getchar();
    putchar(ccode);
    ccode=DecDigit(ccode);
    if(ccode>7||ccode<0)//Validate the user input
    {
        printf_tiny("\n\r Invalid Input\n\r");
        return;
    }
    for(j=0;j<8;j++)//Initialize the array with 0x00
    {
        row_vals[j]=0x00;
    }
    printf_tiny("\n\r Enter row values\n\r");
    for(i=0;i<8;i++)
    {
        printf_tiny("\n\r Enter values for row%d \n\r",i);
        row_vals[i]=0x00;
        temp=getchar();
        putchar(temp);
        temp=HexDigit(temp);
        if(temp<0)
        {
            printf("Invalid input");
            return;
        }
        row_vals[i]|=temp;
        row_vals[i]=row_vals[i]<<4;
        temp=getchar();
        putchar(temp);
        temp=HexDigit(temp);
        if(temp<0)//Validate user input
        {
            printf("Invalid input");
            return;
        }
        row_vals[i]|=temp;
        printf_tiny("\n\r==================================\n\r");
        printf_tiny("Current row values are : \n\r");//Print out the row values every time the user inputs a new row value
        for(j=0;j<8;j++)
        {
            printf("\n\r%02x",row_vals[j]);

        }

    }
    printf_tiny("\n\rEnter 2 digit DDRAM memory location for printing custom character\n\r");
    temp=getchar();
    putchar(temp);
    temp=HexDigit(temp);
    if(temp<0)
    {
        printf_tiny("\n\rInvalid Input\n\r");
        return;
    }
    loc|=temp;
    loc=loc<<4;
    temp=getchar();
    putchar(temp);
    temp=HexDigit(temp);
    if(temp<0)
    {
        printf_tiny("\n\rInvalid Input\n\r");
        return;
    }
    loc|=temp;
    CustomChar(ccode,row_vals,loc);//Call the custom character function and pass the character code, pointer to the array of row values and the DDRAM location
}
