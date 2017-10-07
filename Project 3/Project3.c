/*********************************
*Name:Subhradeep Dutta
*Embedded System Design ECEN 5613
*Lab 3
*********************************/


#define DEBUG
#ifdef DEBUG
#define DEBUGPORT(x) dataout(x); //Define the address for the output of Debugport
#else
#define DEBUGPORT(x)
#endif

#define OFFSET 2

#include <at89c51ed2.h>
#include <mcs51reg.h>
#include <stdio.h>
#include <malloc.h>
#include <stdlib.h>


#define HEAP_SIZE 1600 //Define the heap size as 1600 bytes

unsigned char xdata heap[HEAP_SIZE];//Declaring the heap of size 16000 bytes
xdata int* debug_port;
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
/********************************************************DATAOUT DEFINITION************************************************************/
void dataout(int i)      //Definition of dataout function
{
    debug_port=(xdata int *)0x7FCC;
    *debug_port=i;
}

/********************************************************GETVAL DEFINITION************************************************************/
int getval(void)      //Definition of getval function to fetch input for upto 4 characters from the user
{

    char buffer_size[4];
    printf_tiny("\n\rProvide inputs in 4 digit format and prefix zeroes if necessary\n\r  Example 4 should be entered as 0004\n\r");
            buffer_size[0]=getchar();
            putchar(buffer_size[0]);
            buffer_size[1]=getchar();
            putchar(buffer_size[1]);
            buffer_size[2]=getchar();
            putchar(buffer_size[2]);
            buffer_size[3]=getchar();
            putchar(buffer_size[3]);

            if(buffer_size[0]>=48&&buffer_size[0]<=57&&buffer_size[1]>=48&&buffer_size[1]<=57&&buffer_size[2]>=48&&buffer_size[2]<=57&&buffer_size[3]>=48&&buffer_size[3]<=57)
            {
                return(atoi(buffer_size));
            }
            else
            {
                return (-1);
            }
}
/***********************************************************MAIN PROGRAM***************************************************************/

void main()
{   //Initialize and declare variables

    //Integer variables
    int size1;//Used to store size of buffer 0 and 1 provided by the user
    int buffer_n_loop=0;//Loop variable for the array of buffers
    int loop_buffer0=0;// Loop variable for buffer 0
    int loop_buffer1=0;//Loop variable for buffer 1
    int loop=0; //Temporary loop variable
    int temp2=0;//Temporary variable used for swapping
    int print_content=0;//Temporary variable used for displaying values
    int flag=1;//Flag variable to conditionally assign values to a variable
    int temp1=0;//Temporary variable
    int loop_clear;//Temporary loop variable
    int char_counter=0;//This variable keeps track of the characters
    int char_counter_old=0;//This variable is used to keep track of the last few characters since the previous ?
    int storage_counter_buffer0=0;//Stores number of storage counters in buffer 0
    int storage_counter_buffer1=0;//Stores number of storage counters in buffer 1
    int storage_total=0;//Stores the total number of storage characters
    int plus_input_int;//The the user input for plus command
    int minus_input_int;//Stores the user input for minus command
    int buffern_size[50];//Stores the size for each of the newly created buffers in an array

//Character variables
    char buffer_input;
    char temp;


//Pointer variables
    xdata char * buffer0=NULL;
	xdata char * buffer1=NULL;
	xdata char * buffer[50];


	init_dynamic_memory((MEMHEADER xdata *)heap, HEAP_SIZE);//Allocate memory to heap for 1600 bytes

/***********************************************************USER INPUT*****************************************************************/

	retry1://Label to restart the entire process

    for(;;)//Infinite loop to prompt user for input
    {
        printf_tiny("\n\rEnter buffer size between 32 and 1600 bytes which is divisible by 16\n\r");
        size1=getval();//Function call to getval to obtain input from user
        if(size1==-1||size1==0)
        {
            printf_tiny("Invalid input\n\r");
            continue;//Force the user to enter input in the valid format
        }

        else
        {
            if((size1>31)&&(size1<1601)&&(size1%16==0))
            {
                //Attempt to allocate memmory using malloc function
                buffer0=malloc(size1);
                buffer1=malloc(size1);
                if(buffer0!=NULL && buffer1!=NULL)//Check if memory allocation was successful
                {
                    printf_tiny("Entered size is %d\n\r",size1);
                    printf("Memory of %d  bytes allocated at address %p and %p \n\r",size1, buffer0, buffer1);//Display the number of bytes and pointer location for the allocated buffers
                    break;
                }
                else
                {
                    printf_tiny("Memory allocation failed\n\r Try smaller size\n\r");
                    free(buffer0); //Free associated memory with the buffer
                    free(buffer1);
                    continue; //Force the user to try again
                }

            }
        }

    }

    while(1)//Infinite loop prompting user for inputs
    {
    printf_tiny("------------------------------------");
    printf_tiny("Please enter character\n\r");
    printf_tiny("Press + to add a buffer\n\r");
    printf_tiny("Press - to delete a buffer\n\r");
    printf_tiny("Press ? to display the heap report\n\r");
    printf_tiny("Press = to display contents of buffer0\n\r");;
    printf_tiny("Press %% to swap contents of buffer0 and buffer1\n\r");;
    printf_tiny("Press @ to clear all buffers and start afresh\n\r");
    printf_tiny("------------------------------------");
    buffer_input=getchar(); //Function call to getchar to get input from the user
    printf_tiny("Entered character is --> ");
    putchar(buffer_input);//Echo back character
    printf_tiny("\n\r");
    if((buffer_input>='a' && buffer_input<='z')||(buffer_input>='A' && buffer_input<='Z'))//Check if the entered character is an uppercase or lowercase alphabet
       {
           if(loop_buffer0==size1)//Condition to check if buffer is filled up
           {//If buffer is filled up then characters are not stored in the buffer but only echoed back
               printf_tiny("Buffer full\n\r");
               char_counter++;//Tracks the total number of characters
               storage_total++;//Tracks the total number of storage characters
           }
           else
           {//If there is space available in the buffer characters are stored in buffer0
            *(buffer0+loop_buffer0)=buffer_input;
           loop_buffer0++;//This variable tracks progress through buffer0
           storage_counter_buffer0++;//This variable stores the number of storage counters in buffer0
           char_counter++;//This variable stores the total characters entered
           storage_total++;//This variable keeps track of the total storage character
           }
           continue;
       }

    switch(buffer_input)//Switch case to process command characters
    {
/***********************************************************PLUS COMMAND*****************************************************************/
    case '+'://This code block allows the user to add new buffers
        char_counter++;//Tracks the total characters
        y:printf_tiny("Enter buffer size between 20 and 400 bytes\n\r");
        plus_input_int=getval();
        printf_tiny("\n\r");
        if(plus_input_int<20 || plus_input_int>400)//Condition to validate the input
            goto y;
        buffer[buffer_n_loop]=malloc(plus_input_int);//Allocate memory to the buffer
        if(buffer[buffer_n_loop]==NULL)//Check if memory allocation was successful
        {
            printf_tiny("Buffer allocation failed \n\rRan out of heap space\n\r");

        }
        else
        {
            printf_tiny("Buffer allocation successful\n\r");
            buffern_size[buffer_n_loop]=plus_input_int;//Store the buffer sizes entered by the user into an array
            buffer_n_loop++;
        }
        break;

/***********************************************************MINUS COMMAND*****************************************************************/

    case '-'://This code block deletes a particular buffer based on user input
        char_counter++;
        printf_tiny("Enter a valid buffer number starting from 1\n\r  ");
        minus_input_int=getval();
        if((minus_input_int<=buffer_n_loop+1)&&(minus_input_int>0)&&(minus_input_int<51)&&(buffern_size[minus_input_int-OFFSET]!=0))//Conditions to delete a valid buffer
        {
            if(minus_input_int==1)//Special condition to delete buffer1 which is separate from the array of buffers
            {
                if(buffer1!=NULL)
                {

                free(buffer1);//Free sapce allocated to buffer 1
                buffer1=NULL; //Point Buffer 1 to NULL
                printf_tiny("Buffer 1 deleted\n\r");
                }
                else
                printf_tiny("Requested buffer 1 already deleted\n\r");

            }
            else
            {
            //Offset is 2 to handle the difference in array indexes due to buffer 0 and buffer 1 being separate from the array
            free(buffer[minus_input_int-OFFSET]);//free memory allocated to the buffer
            buffer[minus_input_int-OFFSET]=NULL; //Point the buffer pointer to NULL
            buffern_size[minus_input_int-OFFSET]=0;//Reset the size in the array that tracks the size of all buffers
            printf_tiny("Buffer %d deletion successful\n\r",minus_input_int);
            }
        }

        else
        {
            printf_tiny("Buffer number invalid\n\r");
        }
        break;

/***********************************************************MODULUS COMMAND*****************************************************************/

    case '%':// This code block swaps the contents of buffer 0 with buffer 1
        char_counter++;
        if(buffer1==NULL)
        {
            printf_tiny("Buffer 1 missing\n\r");//A swap is only valid if both buffers are available
            break;
        }

        else
        {
            for(loop=0;loop<size1;loop++)
        {
          temp=*(buffer0+loop);//These operations swap the contents of buffer 0 and buffer 1
          *(buffer0+loop)=*(buffer1+loop);
          *(buffer1+loop)=temp;
         }
         temp2=storage_counter_buffer0; //These operation swap the characteristics of the two buffers
         storage_counter_buffer0=storage_counter_buffer1;
         storage_counter_buffer1=temp2;

         printf_tiny("Contents of buffer0 and buffer1 swapped\n\r");
         break;

        }

/***********************************************************QUESTION COMMAND*****************************************************************/

    case '?':// This block prints out a detailed report of all the available buffers and other relevant data
        char_counter++;
        if(flag==1)//Flag variable is used to assign value to the old character counter so that we can track the characters received since the last question mark
        {
            flag=0;
            char_counter_old=char_counter;
        }
        printf_tiny("----------------------HEAP REPORT--------------------\n\r");

        printf_tiny("Buffer Number - Buffer0\n\r");//Print out the buffer number
        printf("Buffer Start Address - %p\n\r",buffer0);//Print out the buffer starting address
        printf("Buffer End Address - %p\n\r",(buffer0+size1-1));//Print out the buffer ending address
        printf_tiny("Total allocated size of buffer %d\n\r",size1);//Total size allocated to the buffer
        printf_tiny("Storage characters in buffer %d\n\r",storage_counter_buffer0);//Display the total storage characters in this buffer
        printf_tiny("Free space in buffer %d\n\r",(size1-storage_counter_buffer0));//Calculate the free spaces in the buffer and print it

        if(buffer1==NULL)//Check before printing buffer 1 if buffer 1 exists
        {
            printf_tiny("Buffer 1 deleted\n\r");
        }
        else
        {


        printf_tiny("-----------------------------------------------------\n\r");
        printf_tiny("Buffer Number - Buffer1\n\r");//Print out the buffer number
        printf("Buffer Start Address - %p\n\r",buffer1);//Print out the buffer starting address
        printf("Buffer End Address - %p\n\r",(buffer1+size1-1));//Print out the buffer ending address
        printf_tiny("Total allocated size of buffer %d\n\r",size1);//Total size allocated to the buffer
        printf_tiny("Storage characters in buffer %d\n\r",storage_counter_buffer1);//Display the total storage characters in this buffer
        printf_tiny("Free space in buffer %d\n\r",(size1-storage_counter_buffer1));//Calculate the free spaces in the buffer and print it
        }
        temp1=0;
        for(temp1=0;temp1<51-OFFSET;temp1++)
        {
            if((buffern_size[temp1]>19)&&((buffern_size[temp1])<401)&&(buffer[temp1]!=NULL))//Condition to check valid conditions before printing
            {                                                                               //The number must be between 20 and 400 since that is the valid size we accepted from the user
            printf_tiny("Buffer number - %d\n\r",temp1+OFFSET);//Printing out the buffer number in sequence
            printf("Buffer Start Address - %p\n\r",(buffer[temp1]));// Display the buffer start address
            printf("Buffer End Address - %p\n\r",(buffer[temp1]+buffern_size[temp1]));//Display the buffer end address
            printf_tiny("Total allocated size of buffer  %d \n\r",buffern_size[temp1]);//Total allocated size of buffer
            printf_tiny("-----------------------------------------------------\n\r");
            }
        else
            continue;
        }
        printf_tiny("-----------------------------------------------------\n\r");
        printf_tiny("Total characters received so far  %d\n\r",char_counter);
        printf_tiny("Total characters received since last ?  %d\n\r",char_counter-char_counter_old);
        printf_tiny("Total storage characters %d \n\r",storage_total);
        char_counter_old=char_counter;//Store the current value in the other variable to use it for the next iteration




        for(print_content=0;print_content<storage_counter_buffer0;print_content++)//Printing out buffer 0 before emptying it
        {
            if(print_content%48==0)//After every 48 characters print on a new line
            {
                printf_tiny("\n\r");
            }
            printf("%c " ,*(buffer0+print_content));
            *(buffer0+print_content)='\0';//Empty the buffer element

        }
        printf("\n\r");
        storage_counter_buffer0=0;
        loop_buffer0=0;
        break;

/***********************************************************EQUALS COMMAND*****************************************************************/

    case '='://This block contains code to print out buffer 0
        DEBUGPORT(0x8A);//Calling the dubug function
        char_counter++;

        printf_tiny("Dumping contents of buffer0\n\r");
        for(print_content=0;print_content<storage_counter_buffer0;print_content++)
        {
            if(print_content%16==0)//Print pointer address and new line at every 16th chaa
            {
                printf_tiny("\n\r");
                printf("%p :",(buffer0+print_content));
            }
            printf_tiny("%x " ,*(buffer0+print_content));
        }
        printf("\n\r");


        break;

/***********************************************************ATTHERATE COMMAND*****************************************************************/

    case '@'://This piece of code resets all the buffer pointers and frees the memory allocated to all the buffers
        char_counter++;
        loop_clear=buffer_n_loop;
        free(buffer0);
        buffer0=NULL;
        free(buffer1);
        buffer1=NULL;
        for(loop_clear=0;loop_clear<51;loop_clear++)
        {
            free(buffer[loop_clear]);
            buffer[loop_clear]=NULL;
        }
        printf_tiny("All buffers cleared from heap\n\r");
        //All the associated variables are also reset to keep track
        char_counter=0;
        storage_counter_buffer0=0;
        storage_counter_buffer1=0;
        buffer_n_loop=0;
        loop_buffer0=0;
        loop_buffer1=0;
        storage_total=0;
        flag=1;
        goto retry1;
    default:
        char_counter++;//Count for those cases which are not alphabet and not FD
        break;//Break out of thw switch case

    }
    }

    }





