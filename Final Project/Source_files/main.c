/*
 ***********************************************************************************
 * Description	: This file contains the main function for the final project
 * Date		    : 22 November 2016
 * File name	: main.c
 *
 ***********************************************************************************
 */

#include "main.h"

/** Global variable for PIR Sensor */
unsigned char PIRFlag = 0;

_sdcc_external_startup()
{
    //Setting the RS0:RS1 bits in the AUXR register
    //bits 2 and 3, enable 1 KB of internal XRAM
    AUXR |= 0x0C;

    //Setting the baud rate to 9600
    /* Timer 1, 8 bit auto reload mode */
    TMOD = 0x20;
    TH1 = -3;
    /* Serial mode 1, 8 bit data, 1 start bit, 1 stop bit, Receive data from RxD pin */
    SCON = 0x52;
    /* Start timer */
    TR1 = 1;

    TCON |= 0x01;

    //Note: need to return from startup
    return 0;
}

void main(void)
{
    /* A variable to store the message number */
    unsigned char message_no, message_no_1;
    unsigned char i;
    unsigned char read[17];
    PIRFlag = 0;

    /* Initialize LCD */
    lcdinit();

    /* Initialize PWM */
    init_pwm();

    /** Create custom characters for the graphical representation of speed */
    graphical_percentage_display();

    lcdgotoxy(1,1);
	lcdputstr("GSM interfacing");
	while (1)
	{
	    //printf_tiny("\r\nHello");
        /* Return message will be of the format <CR><LF>+CMTI: "SM",3<CR><LF> */
        /* Carriage return */
		while(getchar() != CARRIAGE_RETURN);
            /* New Line */
			while(getchar() != NEW_LINE);

                /* Checking for +CMTI:"SM",3 */
				if(getchar() == '+')
				{
				    //printf_tiny("\n\rHello1");
					if(getchar() == 'C')
					{
					    //printf_tiny("\n\rHello2");
						if(getchar() == 'M')
						{
						    //printf_tiny("\n\rHello3");
							if(getchar() == 'T')
							{
							    //printf_tiny("\n\rHello4");
								if(getchar() == 'I')
								{
								    //printf_tiny("\n\rHello5");
									while(getchar() != ',');

									/* Get the SMS number */
									message_no = getchar();
                                    message_no_1 = getchar();

									delay_ms(10);
									put_string("AT");
									putchar(CARRIAGE_RETURN);
									putchar(NEW_LINE);

									/* Set the operating mode to sms text mode */
									//put_string("at+cmgf=1");
									put_string("AT+CMGF=1");
									putchar(CARRIAGE_RETURN);
									putchar(NEW_LINE);

									/* Read the sms */
									put_string("AT+CMGR=");
									putchar(message_no);
									if (message_no_1 != CARRIAGE_RETURN)
                                    {
                                        putchar(message_no_1);
                                    }
									/* Carriage return */
									putchar(CARRIAGE_RETURN);
									/* New Line */
									putchar(NEW_LINE);

                                    /* Wait for carriage return */
									while (getchar()!= CARRIAGE_RETURN);
									/* Wait for new line */
									while (getchar()!= NEW_LINE);
									while (getchar()!= CARRIAGE_RETURN);
									while (getchar()!= NEW_LINE);
									//while (getchar()!= NEW_LINE);
									while (getchar()!= CARRIAGE_RETURN);
									while (getchar()!= NEW_LINE);

									while (getchar()!= CARRIAGE_RETURN);
									while (getchar()!= NEW_LINE);
									while (getchar()!= CARRIAGE_RETURN);
									while (getchar()!= NEW_LINE);
									while (getchar()!= CARRIAGE_RETURN);
									while (getchar()!= NEW_LINE);

                                    /** Store the received text message into the array */
									for (i = 0; i <= 15; i++)
									{
										read[i] = getchar();

										if (read[i] == CARRIAGE_RETURN)
                                        {
                                            read[i] = '\0';
                                            break;
                                        }
                                        read[16] = '\0';
									}
									lcdgotoxy(2, 1);

                                    lcdputstr("                ");
                                    delay_ms(2);
                                    lcdgotoxy(2, 1);
                                    delay_ms(2);

                                    /** Display the text message receved from GSM module onto the LCD */
									lcdputstr(read);

									/** Decide whether to change the direction or speed based on the sms command received  */
									motor_driver(read);
								}
							}
						}
					}
				}
	}
}

/** ISR for external interrupt 0 */
void external0 (void) __interrupt(0)
{
    /** Check the previous state of the motor and decide
     *  whether to turn the motor on or off */
    if (PIRFlag)
    {
        motor_on();
        PIRFlag = 0;
    }

    else if (PIRFlag == 0)
    {
        motor_off();
        PIRFlag = 1;
    }
}

void putchar(char ch)
{
    // load serial port with transmit value
    while(TI == 0);

    SBUF = ch;
    // clear TI flag
    TI=0;
}

char getchar ()
{
    // wait for character to be received, spin on RI
    while ((SCON & 0x01) == 0);

    // clear RI flag
	RI = 0;
	// return character from SBUF
	return SBUF;
}

void put_string(unsigned char *str)
{
	while(*str)
	{
		putchar(*str);
		str++;
	}
}
