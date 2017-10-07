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
#define IODEVICEID_WRITE 0x70
#define IODEVICEID_READ 0x71


/*****************************************************IOEXPWRITE FUNCTION*************************************************************
*This function writes to the IOExpander using the I2C protocol
*************************************************************************************************************************************/

void IOEXPWrite(unsigned char address)
{
    I2CStart();//Send a start condition on the bus
	I2CWrite(IODEVICEID_WRITE);//Provide the device address for the IOExpander
	while(I2CAck());
   	I2CWrite(address); // Write the data on the I2C bus
	while(I2CAck());
    I2CStop();          	   // Stop I2C communication on the bus
	LCDDelay(2);
}

/*****************************************************IOEXPREAD FUNCTION*************************************************************
*This function reads from the IOExpander using the I2C protocol
*************************************************************************************************************************************/

unsigned char IOEXPRead()
{
    unsigned char datavalue;
    I2CStart();               // Send a start condition on the bus
   	I2CWrite(IODEVICEID_READ);//Provide the device address for the IOExpander
	while(I2CAck());
	datavalue = I2CRead();  // Read the data and store it in a variable
	I2CNack();
    I2CStop(); // Send a stop condition to end I2C communication
	LCDDelay(1);
    return datavalue;
}

/*****************************************************HEXTOASCII FUNCTION*************************************************************
*This function takes hex values as input and returns its corresponding ASCII value
*************************************************************************************************************************************/

unsigned char HexToASCII(unsigned char n)
{
    if(n<10)
        return n+'0';
    else
        return (n-10) + 'A';
}


/*****************************************************READPINVALUES FUNCTION**********************************************************
*This function uses the I2C protocol to read the current status of the IOExpander pins
*************************************************************************************************************************************/

void ReadPinValues()
{
    unsigned char pin_value;
    pin_value=IOEXPRead();
    printf_tiny("\n\rCurrent status of the pins: %x\n\r",pin_value);
}










