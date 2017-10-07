/*
 ***********************************************************************************
 * Description	: This file contains the functions for the operations to be
 *		          performed on a HD44780u LCD module
 * Date		    : 27 October 2016
 * File name	: lcd.c
 *
 ***********************************************************************************
 */

#include "lcd.h"

/* Description  : This function waits for the LCD to be ready again to accept commands
 * Input        : NA
 * Return       : NA
 */
void lcdbusywait()
{
    /** Small delay to wait for the LCD operations to be complete */
    my_delay_ms(4);
}

/* Description  : This function is used to send data to the data register
 * Input        : The data to be sent
 * Return       : NA
 */
void send_data(char a)
{
    /* RS = 1. selects the data register */
    /* RW = 0, write */
    RS = 1;
    RW = 0;
    P0 = a;

    /** Enable LCD for data transfer */
    EN  = 1;
    lcdbusywait();
    EN  = 0;
}

/* Description  : This function is used to send commands to Instruction register
 * Input        : The command to be sent
 * Return       : NA
 */
void send_command(char a)
{
    /* RS = 0. selects the instruction register */
    /* RW = 0, write */
    RS = 0;
    RW = 0;
    P0 = a;

    /** Enable LCD for command transfer */
    EN  = 1;
    lcdbusywait();
    EN  = 0;
}

/* Description  : This function clears the LCD display
 * Input        : NA
 * Return       : NA
 */
void clear_display()
{
    //Command to clear display is 1
    send_command(1);
}

/* Description  : This function initializes the LCD module
 * Input        : NA
 * Return       : NA
 */
void lcdinit()
{
	send_data(0x00);
	RS = 0;
	/* RW = 1 : Read
	 * RW = 0 : Write
	 */
	RW = 0;
	my_delay_ms(200);

    send_command(0x30);
    my_delay_ms(50);
    send_command(0x30);
    my_delay_ms(110);
    send_command(0x30);

    /* Interface is 8 bit long, select 1 line for display */
    send_command(0x38);
    /*
     * RS  R/W DB7 DB6 DB5 DB4 DB3 DB2 DB1 DB0
     * === === === === === === === === === ===
     * 0   0   0   0   0   0   1   D   C   B
     *
     * Details
     * D: The display is ON when D = 1 and OFF when D = 0. The DD RAM contents remain unchanged.
     *
     * C: The cursor is displayed when C = 1 and is not displayed when C = 0.
     * The cursor is displayed as 5 dots in the 8th line when the 5 x 7 dot character font is selected and
     * as 5 dots in the 11th line when the 5 x 10 dot character font is selected.
     *
     * B: The character at the the cursor position blinks when B = 1.
     */
    send_command(0x0C);

    /* clear display */
    send_command(0x01);

    /* Entry Mode Set
     *
     * RS  R/W DB7 DB6 DB5 DB4 DB3 DB2 DB1 DB0
     * === === === === === === === === === ===
     * 0   0   0   0   0   0   0   1  I/D  S
     *
     * Details
     * Specifies whether to increment (I/D = 1) or decrement (I/D = 0) the address counter after subsequent
     * DD RAM read or write operations.
     *
     * If S = 1 the display will be shifted to the left (if I/D = 1) or right (if I/D = 0) on subsequent DD RAM write operations.
     * This makes it looks as if the cursor stands still and the display moves when each character is written to the DD RAM.
     * if S = 0 the display will not shift on subsequent DD RAM write operations.
     */
    send_command(0x06);
}

/* Description  : This function displays a single character on the LCD
 * Input        : The character to be displayed
 * Return       : NA
 */
void lcdputch(char a)
{
    /* Choose data register */
    RS = 1;
    RW = 0;
    send_data(a);
}

/* Description  : This function prints a string on the LCD
 * Input        : A pointer to the string to be printed
 * Return       : NA
 */
void lcdputstr(char *str)
{
    /* Loop variable */
	int i;

	/* Iterate through the string and print one character at a time,
	 * stop when the \0 character is encountered
     */
	for(i = 0; str[i] != '\0'; i++)
    {
        lcdputch(str[i]);
    }
}

/* Description  : This function
 * Input        :
 * Return       : NA
 */
void shift_right()
{
    /* Cursor and Display Shift
     *
     *   RS  R/W DB7 DB6 DB5 DB4 DB3 DB2 DB1 DB0
     *   === === === === === === === === === ===
     *    0   0   0   0   0   1  S/C R/L  *   *
     *
     *  S/C R/L
     *   === ===
     *    0   0   Shifts the cursor position to the left
     *            (Address Counter is decremented by 1)
     *    0   1   Shifts the cursor position to the right
     *            (Address Counter is incremented by 1)
     *    1   0   Shifts the entire display to the left
     *            The cursor follows the display shift
     *    1   1   Shifts the entire display to the right
     *            The cursor follows the display shift
     */
	send_command(0x1C);
}

/* Description  : This function
 * Input        :
 * Return       : NA
 */
void shift_left()
{
    /* Cursor and Display Shift
     *
     *   RS  R/W DB7 DB6 DB5 DB4 DB3 DB2 DB1 DB0
     *   === === === === === === === === === ===
     *    0   0   0   0   0   1  S/C R/L  *   *
     *
     *  S/C R/L
     *   === ===
     *    0   0   Shifts the cursor position to the left
     *            (Address Counter is decremented by 1)
     *    0   1   Shifts the cursor position to the right
     *            (Address Counter is incremented by 1)
     *    1   0   Shifts the entire display to the left
     *            The cursor follows the display shift
     *    1   1   Shifts the entire display to the right
     *            The cursor follows the display shift
     */
	send_command(0x18);
}

/* Description  : This function
 * Input        :
 * Return       : NA
 */
void lcdgotoaddr(char x)
{
    send_command(0x80 + x);
}

/* Description  : This function goes to a particular cursor location
 * Input        : Line number, and Character number within the line
 * Return       : NA
 */
void lcdgotoxy(char x, char y)
{
    if (x == 1)
    {
        lcdgotoaddr(0x00 + y - 1);
    }
    else if (x == 2)
    {
        lcdgotoaddr(0x40 + y - 1);
    }
    else if (x == 3)
    {
        lcdgotoaddr(0x10 + y - 1);
    }
    else if (x == 4)
    {
        lcdgotoaddr(0x50 + y - 1);
    }
}

/* Description  : This function reads and returns the DDRAM data
 * Input        : NA
 * Return       : DDRAM data
 */
unsigned char read_ddram_data()
{
    char ddram_data;

    /* RW = 1 : Read
	 * RW = 0 : Write
	 */
    /* RS = 0 : Instruction Register
     * RS = 1 : Data Register
     */
     RS = 1;
     RW = 1;
     ddram_data = *(LCD_MAP);

     return ddram_data;
}

/* Description  : This function reads and returns the DDRAM address
 * Input        : NA
 * Return       : DDRAM address
 */
char read_ddram_addr()
{
    char ddram_addr;

    /* To read DDRAM address, RS = 0, and RW = 1 */
    /* Get DDRAM address */
    RS = 0;
    RW = 1;
    ddram_addr = *(LCD_MAP);

    return ddram_addr;
}

/* Description  : This function forces the LCD to print in the next line
 * Input        : NA
 * Return       : NA
 */
void goto_next_line()
{
    char addr = read_ddram_addr();

    if (addr == 0x10)
    {
        lcdgotoxy(2,1);
    }

    else if (addr == 0x50)
    {
        lcdgotoxy(3,1);
    }

    else if (addr == 0x20)
    {
        lcdgotoxy(4,1);
    }

    else if (addr == 0x61)
    {
        clear_display();
        lcdgotoxy(1,1);
    }
}

/* Description  : This function creates a custom character
 * Input        : Character code and the Eight row values
 * Return       : NA
 */
void lcd_create_custom_char(unsigned char char_code, unsigned char pixel_val[8])
{
    /** Loop variable */
    unsigned char i;

    /*******************************
    Create CGRAM address byte
    D7  D6  D5  D4  D3  D2  D1  D0
    0   1   C3  C2  C1  R3  R2  R1

    C3, C2, C1 is the character code
    R3, R2, R1 is the row number
    ********************************/

    unsigned char cgram_addr = 0;

    /** Make bit_6 1 */
    cgram_addr |= 0x40;

    /** Store the character code at D5, D4, D3 */
    cgram_addr |= (char_code << 3);

    for (i = 0; i < 8; i++)
    {
        /** Set CGRAM address */
        RS = 0;
        RW = 0;
        send_command(0x40 | cgram_addr);

        lcdbusywait();
        /* Select data register */
        RS = 1;
        RW = 0;

        send_data(pixel_val[i]);
        EN = 1;
        my_delay_ms(5);
        EN = 0;

        /* Go to the next row */
        cgram_addr++;
    }
}

/* Description  :   This function creates a custom character for representing
                    speed of motor in pictorial way
 * Input        :   Character code and the Eight row values
 * Return       :   NA
 */
void graphical_percentage_display()
{
    unsigned char pixel_val[8];

    /* Custom character 0 */
    pixel_val[0] = 0x10;
    pixel_val[1] = 0x10;
    pixel_val[2] = 0x10;
    pixel_val[3] = 0x10;
    pixel_val[4] = 0x10;
    pixel_val[5] = 0x10;
    pixel_val[6] = 0x10;
    pixel_val[7] = 0x10;
    lcd_create_custom_char(0, pixel_val);

    /* Custom character 1 */
    pixel_val[0] = 0x18;
    pixel_val[1] = 0x18;
    pixel_val[2] = 0x18;
    pixel_val[3] = 0x18;
    pixel_val[4] = 0x18;
    pixel_val[5] = 0x18;
    pixel_val[6] = 0x18;
    pixel_val[7] = 0x18;
    lcd_create_custom_char(1, pixel_val);

    /* Custom character 2 */
    pixel_val[0] = 0x1C;
    pixel_val[1] = 0x1C;
    pixel_val[2] = 0x1C;
    pixel_val[3] = 0x1C;
    pixel_val[4] = 0x1C;
    pixel_val[5] = 0x1C;
    pixel_val[6] = 0x1C;
    pixel_val[7] = 0x1C;
    lcd_create_custom_char(2, pixel_val);

    /* Custom character 3 */
    pixel_val[0] = 0x1E;
    pixel_val[1] = 0x1E;
    pixel_val[2] = 0x1E;
    pixel_val[3] = 0x1E;
    pixel_val[4] = 0x1E;
    pixel_val[5] = 0x1E;
    pixel_val[6] = 0x1E;
    pixel_val[7] = 0x1E;
    lcd_create_custom_char(3, pixel_val);

    /* Custom character 4 */
    pixel_val[0] = 0x1F;
    pixel_val[1] = 0x1F;
    pixel_val[2] = 0x1F;
    pixel_val[3] = 0x1F;
    pixel_val[4] = 0x1F;
    pixel_val[5] = 0x1F;
    pixel_val[6] = 0x1F;
    pixel_val[7] = 0x1F;
    lcd_create_custom_char(4, pixel_val);

    /* Custom character 5 */
    pixel_val[0] = 0x01;
    pixel_val[1] = 0x01;
    pixel_val[2] = 0x01;
    pixel_val[3] = 0x01;
    pixel_val[4] = 0x01;
    pixel_val[5] = 0x01;
    pixel_val[6] = 0x01;
    pixel_val[7] = 0x01;
    lcd_create_custom_char(5, pixel_val);

    /* Custom character 6 */
    pixel_val[0] = 0x00;
    pixel_val[1] = 0x00;
    pixel_val[2] = 0x00;
    pixel_val[3] = 0x00;
    pixel_val[4] = 0x00;
    pixel_val[5] = 0x00;
    pixel_val[6] = 0x00;
    pixel_val[7] = 0x00;
    lcd_create_custom_char(6, pixel_val);

    /* Custom character 6 */
    pixel_val[0] = 0x1F;
    pixel_val[1] = 0x1F;
    pixel_val[2] = 0x1F;
    pixel_val[3] = 0x1F;
    pixel_val[4] = 0x00;
    pixel_val[5] = 0x00;
    pixel_val[6] = 0x00;
    pixel_val[7] = 0x00;
    lcd_create_custom_char(7, pixel_val);
}

/* Description  : This function generates a delay of 1 ms
 * Input        : Delay needed in ms
 * Return       : NA
 */
void my_delay_ms(int a)
{
    int i, j;
    for(i = 0; i < a; i++)
    {
        for(j = 0; j < 124; j++)
        {

        }
    }
}
