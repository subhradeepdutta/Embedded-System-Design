/*
 ***********************************************************************************
 * Description	: This file contains functions to generate delays that are needed for
                  ADC
 * Date		    : 22 November 2016
 * File name	: Delays.c
 ***********************************************************************************
 */


/*************************************************************************************
Description   : This function is used to generate a long delay

Input         : A number indicating the amount of delay required
Output        : N/A
*************************************************************************************/

 void Delay(int a)
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

/*************************************************************************************
Description   : This function is used to generate a very short delay

Input         :  A number indicating the amount of delay required
Output        : N/A
*************************************************************************************/

void FastDelay(uint8_t var)
{
    while(var!=0)
        var--;
}
