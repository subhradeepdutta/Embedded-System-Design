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



/*****************************************************I2CCLK DEFINITION**************************************************************
*This function is used to generate a clock for the I2C protocol. It toggles the SCL line high and then pulls it back low
**************************************************************************************************************************************/

void I2CClk()
{
    I2CDelay(0);
    SCL=1;//Pull the clock high
    I2CDelay(0);
    SCL=0;//Pull the clock low
}

/*****************************************************I2CDELAY DEFINITION**************************************************************
*This function is used to generate a short delay for the I2C clock
**************************************************************************************************************************************/

void I2CDelay(uint8_t var)
{
    while(var!=0)
        var--;
}

/*****************************************************I2CSTART DEFINITION**************************************************************
*This function is used to generate the start condition to initiate communication via I2C protocol
*
*This function has been derived from code at www.exploreembedded.com
*Link: http://exploreembedded.com/wiki/A6b:8051_Interfacing:_EEPROM_AT24C16
**************************************************************************************************************************************/

void I2CStart()
{

 	SCL = 0;		// Pull SCL low

	SDA = 1;        // Pull SDA High
	LCDDelay(1);

	SCL = 1;		//Pull SCL high
	LCDDelay(1);

	SDA = 0;        //Now Pull SDA low
	LCDDelay(1);

	SCL = 0;        //Pull SCL low
}


/*****************************************************I2CSTOP  DEFINITION**************************************************************
*This function is used to generate the stop condition to end communication via I2C protocol
*
*This function has been derived from code at www.exploreembedded.com
*Link: http://exploreembedded.com/wiki/A6b:8051_Interfacing:_EEPROM_AT24C16
**************************************************************************************************************************************/

void I2CStop()
{

 	SCL = 0;		// Pull SCL low

	SDA = 0;        // Pull SDA high
	LCDDelay(1);

	SCL = 1;		//Pull SCL high
	LCDDelay(1);

	SDA = 1;        //Pull SDA low
	LCDDelay(1);
}

/*****************************************************I2CREAD  DEFINITION**************************************************************
*This function is used to read data using I2C protocol
*
*This function has been derived from code at www.exploreembedded.com
*Link: http://exploreembedded.com/wiki/A6b:8051_Interfacing:_EEPROM_AT24C16
**************************************************************************************************************************************/

unsigned char I2CRead()
{
    unsigned char i=0;
    unsigned char datavalue=HEX_ZERO;
    SDA=1;  //Pull SDA high
    for(i=0;i<8;i++)
    {
        LCDDelay(1);
        SCL=1;
        LCDDelay(1);
        datavalue=datavalue<<1;
        datavalue=datavalue|SDA;
        SCL=0;

    }
    return datavalue;
}

/*****************************************************I2CWRITE  DEFINITION**************************************************************
*This function is used to write data using I2C protocol
*
*This function has been derived from code at www.exploreembedded.com
*Link: http://exploreembedded.com/wiki/A6b:8051_Interfacing:_EEPROM_AT24C16
**************************************************************************************************************************************/

 void I2CWrite(unsigned char datavalue)
{
    unsigned char i=0;

    while(i<8)
    {
        SDA=datavalue & MSB_BIT;
        I2CClk();
        datavalue=datavalue<<1;
        i++;
    }
    SDA=1;
}


/*****************************************************I2CACK  DEFINITION**************************************************************
*This function is used to generate acknowledgement for I2C protocol
**************************************************************************************************************************************/

 int I2CAck()
{
    volatile int acksig;
    acksig=SDA;
    I2CClk();       // Pull SCL high
    while(acksig==1);
    return 0;
}


/*****************************************************I2CNACK  DEFINITION**************************************************************
*This function is used to generate negative acknowledgement for I2C protocol
**************************************************************************************************************************************/

 void I2CNack()
{
    SDA = 1;		//Pull SDA high to indicate Negative/NO ACK
   I2CClk();	    // Generate the Clock
	SCL = 1;		// Set SCL
}





/*****************************************************WRITEBYTE  DEFINITION**************************************************************
*This function is used to write a byte of data to the serial EEPROM. It accepts the data , EEPROM address and page block number as input
*
*This function has been derived from code at www.exploreembedded.com
*Link: http://exploreembedded.com/wiki/A6b:8051_Interfacing:_EEPROM_AT24C16
**************************************************************************************************************************************/

 void WriteByte(unsigned char address, unsigned char datavalue, unsigned char PageNo)
{
    I2CStart();               // Send out the start condition
   	I2CWrite(DEVICE_ID|(PageNo<<1));	   //Connect to the EEPROM by providing the correct device ID
	while(I2CAck());
   	I2CWrite(address); // Provide the EEPROM address to be written to
	while(I2CAck());         //Wait for acknowledgement
   	I2CWrite(datavalue);  // Write the data at the specified address
	while(I2CAck());                //Wait for acknowledgement
    I2CStop();           	   // Send out the stop condition to end I2C communication
	LCDDelay(5);
}

/*****************************************************READBYTE  DEFINITION**************************************************************
*This fucntion is used to read a byte from the serial EEPROM. It takes the EEPROM address and page block number as input and return the data
*
*This function has been derived from code at www.exploreembedded.com
*Link: http://exploreembedded.com/wiki/A6b:8051_Interfacing:_EEPROM_AT24C16
**************************************************************************************************************************************/


unsigned char ReadByte(unsigned char address,unsigned char PageNo)
{
    unsigned char datavalue;
    I2CStart();               // Send out the start condition
   	I2CWrite(DEVICE_ID|(PageNo<<1));	   //Connect to the EEPROM by providing the correct address
	while(I2CAck());
   	I2CWrite(address); // Select the address to be read from
    while(I2CAck());        //Wait for acknowledgement
    I2CStart();		       // Send out the start condition
    I2CWrite(0xA1|(PageNo<<1));
    while(I2CAck());
	datavalue = I2CRead(); // Read the data from specified address
	I2CNack();
    I2CStop();		           // Stop i2c communication
	LCDDelay(1);
    return datavalue;       //Return the read byte

}


/********************************************************HEXDIGIT DEFINITION************************************************************
*This function is used to convert user input from ascii to hex format
**************************************************************************************************************************************/

unsigned char HexDigit(unsigned char n)
{
    unsigned char c=HEX_ZERO;
    if (n>='0'&&n<='9')
        c|=(n-'0');
    else if(n>='A'&&n<='F')
        c|=(0x0A+(n-'A'));
    else if(n>='a'&&n<='f')
        c|=(0x0A+(n-'a'));
    else
        c=c-5;


return c;


}

/********************************************************DECDIGIT DEFINITION************************************************************
*This function is used to convert user input from ascii to decimal format
**************************************************************************************************************************************/

unsigned char DecDigit(unsigned char n)
{
    unsigned char c=DEC_ZERO;
    if (n>='0'&&n<='9')
        c=n-'0';
    else
        c=-5;


return c;


}


/******************************************************WRITEBYTEDEMO DEFINITION*****************************************************
*This function is used to demo the write byte functionality. It accepts an EEPROM address and page block number and the data to be written
**************************************************************************************************************************************/

void WriteByteDemo()
{
    char temp=HEX_ZERO;
    char temp1=HEX_ZERO;
    unsigned int address=HEX_ZERO;
    unsigned char PageBlock=DEC_ZERO;
    unsigned char datavalue=HEX_ZERO;
    printf_tiny("\n\rEnter an EEPROM address\n\r");
    temp=getchar();
    putchar(temp);
    temp1=getchar();
    putchar(temp1);
    temp=HexDigit(temp);
    temp1=HexDigit(temp1);
    if(temp<0||temp1<0)//Validate user input
    {
        printf_tiny("\n\rInvalid Input\n\r");
        return;
    }
    address|=temp;
    address=address<<4;
    address|=temp1;//Combine the inputs from user
    printf("\n\r Entered address is %02x \n\r",address);
    printf_tiny("\n\rEnter an EEPROM page block from 0 to 7\n\r");
    temp=getchar();
    putchar(temp);
    temp=DecDigit(temp);//Convert the value to decimal
    if(temp<0||temp>7)
    {
        printf_tiny("\n\rInvalid Input\n\r");
        return;
    }
    PageBlock=temp;
    printf_tiny("\n\r Entered page block is %d \n\r",PageBlock);
    printf_tiny("\n\rEnter data between 0x00 to 0xFF\n\r");
    temp=getchar();
    putchar(temp);
    temp1=getchar();
    putchar(temp1);
    temp=HexDigit(temp);
    temp1=HexDigit(temp1);
    if(temp<0||temp1<0)//validate user input
    {
        printf_tiny("\n\rInvalid Input\n\r");
        return;
    }
    datavalue|=temp;
    datavalue=datavalue<<4;
    datavalue|=temp1;//Combine the user inputs from temp and temp1
    WriteByte(address,datavalue,PageBlock);//Write data to the EEPROM
    printf("\n\rData %03x written successfully to %03x addresss at %d Page block\n\r", datavalue, address, PageBlock);
}

/******************************************************READBYTEDEMO DEFINITION********************************************************
*This function is used to demo the read byte functionality. It accepts an EEPROM address and page block number and prints out the data
*from the EEPROM
**************************************************************************************************************************************/

void ReadByteDemo()
{
    char temp=HEX_ZERO;
    char temp1=HEX_ZERO;
    unsigned int address=HEX_ZERO;
    unsigned char PageBlock=DEC_ZERO;
    unsigned char datavalue;
    printf_tiny("\n\rEnter an EEPROM address\n\r");
    temp=getchar();
    putchar(temp);
    temp1=getchar();
    putchar(temp1);
    temp=HexDigit(temp);
    temp1=HexDigit(temp1);
    if(temp<0||temp1<0)//Validate user input
    {
        printf_tiny("\n\rInvalid Input\n\r");
        return;
    }
    address|=temp;
    address=address<<4;
    address|=temp1;//Combine user input into a single value
    printf("\n\r Entered address is %02x \n\r",address);
    printf_tiny("\n\rEnter an EEPROM page block from 0 to 7\n\r");
    temp=getchar();
    putchar(temp);
    temp=DecDigit(temp);//Convert to decimal
    if(temp<0||temp>7)//Validate user input
    {
        printf_tiny("\n\rInvalid Input\n\r");
        return;
    }
    PageBlock=temp;
    printf_tiny("\n\r Entered page block is %d \n\r",PageBlock);
    datavalue=ReadByte(address,PageBlock);
    printf_tiny("\n\r%d%x : %x\n\r",PageBlock,address, datavalue);
}


/******************************************************HEXDUMP DEFINITION*****************************************************
*This function is used to dump the contents of EEPROM for a certain address range input by the user
******************************************************************************************************************************/

void HexDump()
{
    int i=0;
    char temp=HEX_ZERO;
    char temp1=HEX_ZERO;
    unsigned int addressStart=HEX_ZERO;
    unsigned int addressEnd=HEX_ZERO;
    unsigned int addressCurr=HEX_ZERO;
    unsigned char PageBlockStart=DEC_ZERO;
    unsigned char PageBlockEnd=DEC_ZERO;
    unsigned char PageBlockCurr=DEC_ZERO;
    unsigned char datavalue;
    printf_tiny("\n\rEnter starting EEPROM address\n\r");
    temp=getchar();
    putchar(temp);
    temp1=getchar();
    putchar(temp1);
    temp=HexDigit(temp);//Convert user input to hex
    temp1=HexDigit(temp1);//Convert user input to hex
    if(temp<0||temp1<0)//Validating user input
    {
        printf_tiny("\n\rInvalid Input\n\r");
        return;
    }
    addressStart|=temp;
    addressStart=addressStart<<4;
    addressStart|=temp1;//Combine user inputs into a single variable
    printf("\n\r Entered address is %03x \n\r",addressStart);
    printf_tiny("\n\rEnter an EEPROM page block from 0 to 7\n\r");
    temp=getchar();
    putchar(temp);
    temp=DecDigit(temp);//Convert to decimal
    if(temp<0||temp>7)//Validate user input for values above 7
    {
        printf_tiny("\n\rInvalid Input\n\r");
        return;
    }
    PageBlockStart=temp;
    printf_tiny("\n\r Entered page block is %d \n\r",PageBlockStart);

    printf_tiny("\n\rEnter last EEPROM address\n\r");
    temp=getchar();
    putchar(temp);
    temp1=getchar();
    putchar(temp1);
    temp=HexDigit(temp);
    temp1=HexDigit(temp1);
    if(temp<0||temp1<0)//Validate user inputs
    {
        printf_tiny("\n\rInvalid Input\n\r");
        return;
    }
    addressEnd|=temp;
    addressEnd=addressEnd<<4;
    addressEnd|=temp1;//Combine user inputs into a single variable
    printf("\n\r Entered address is %03x \n\r",addressEnd);
    printf_tiny("\n\rEnter an EEPROM page block from 0 to 7\n\r");
    temp=getchar();
    putchar(temp);
    temp=DecDigit(temp);
    if(temp<0||temp>7)
    {
        printf_tiny("\n\rInvalid Input\n\r");
        return;
    }
    PageBlockEnd=temp;
    printf_tiny("\n\r Entered page block is %d \n\r",PageBlockEnd);

    if((PageBlockStart>PageBlockEnd)||((PageBlockStart==PageBlockEnd)&&(addressStart>addressEnd)))//Check if starting address is ahead of the ending address
    {
        printf_tiny(" End address is located before start address");
        return;
    }
    else if(PageBlockStart==PageBlockEnd&&addressStart==addressEnd)//Check if starting address is same as ending address
    {
        printf_tiny(" End address is same as start address");
        return;
    }

    PageBlockCurr=PageBlockStart;
    addressCurr=addressStart;


    for(PageBlockCurr=PageBlockStart;PageBlockCurr<=PageBlockEnd;PageBlockCurr++)//Loop through page blocks in the EEPROM
    {
        for(addressCurr;addressCurr<=0xFF;addressCurr++)//Loop through the address in the EEPROM
        {
            if(PageBlockCurr==PageBlockEnd&&addressCurr==addressEnd+1)//Breaking condition for the loop
                return;
            if(i%16==0)
            {
                printf_tiny("\n\r");
                printf("%x %02x:",PageBlockCurr,addressCurr);
            }
            datavalue=ReadByte(addressCurr,PageBlockCurr);
            printf_tiny("%x ",datavalue);
            i++;
        }
        addressCurr=0;
    }

}

/******************************************************LCDDISPLAYDEMO DEFINITION**********************************************
*This function is used to read data from the EEPROM and display it on the LCD
******************************************************************************************************************************/

void LCDDisplayDemo()
{

    char temp=HEX_ZERO;
    char temp1=HEX_ZERO;
    char addrhigh;
    char addrlow;
    char pageno;
    unsigned char address=HEX_ZERO;
    unsigned int row_no=DEC_ZERO;
    unsigned char PageBlock=DEC_ZERO;
    unsigned char datavalue;
    char strdata[2];

    printf_tiny("\n\rEnter an EEPROM address\n\r");
    temp=getchar();
    putchar(temp);
    addrhigh=temp;
    temp1=getchar();
    putchar(temp1);
    addrlow=temp1;
    temp=HexDigit(temp);
    temp1=HexDigit(temp1);
    if(temp<0||temp1<0)//Validate user input
    {
        printf_tiny("\n\rInvalid Input\n\r");
        return;
    }
    address|=temp;
    address=address<<4;
    address|=temp1;//Combine the user inputs
    printf("\n\r Entered address is %03x \n\r",address);
    printf_tiny("\n\rEnter an EEPROM page block from 0 to 7\n\r");
    temp=getchar();
    putchar(temp);
    pageno=temp;
    temp=DecDigit(temp);
    if(temp<0||temp>7)
    {
        printf_tiny("\n\rInvalid Input\n\r");
        return;
    }
    PageBlock=temp;
    printf("\n\r Enter a row number between 0 and 3\n\r");
    temp=getchar();
    putchar(temp);
    temp=DecDigit(temp);
    if(temp<0||temp>3)//Check if row number is between 0 and 3
    {
        printf("\n\rInvalid input\n\r");
        return;
    }
    row_no=temp;
    datavalue=ReadByte(address,PageBlock);
    LCDgotoxy(row_no,DEC_ZERO);//Set cursor position based on user input
    LCDputch(pageno);
    LCDputch(addrhigh);
    LCDputch(addrlow);
    sprintf(strdata,"%x",datavalue);
    LCDputch(':');
    LCDputch(strdata[0]);
    LCDputch(strdata[1]);
    printf_tiny("\n\rOutput is available on the LCD\n\r");


}


/******************************************************EERESET DEFINITION******************************************************
*This function is used to perform a software reset for the EEPROM. The following commands are sent to the EEPROM to force
*it to reset.
*     1.START condition
*     2. 9 bits of 1 on the SDA along with clock
*     3. START condition
*     4. STOP condition
******************************************************************************************************************************/

void eereset()
{
    unsigned char i=0;
    I2CStart();
    while(i<9)
    {
        SDA=1;
        I2CClk();
        i++;
    }
    I2CStart();
    I2CStop();

    printf_tiny("\n\rEEPROM software reset successsful\n\r");
}
