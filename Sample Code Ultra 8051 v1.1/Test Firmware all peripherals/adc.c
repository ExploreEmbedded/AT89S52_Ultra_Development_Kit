/***************************************************************************************************
                                   ExploreEmbedded	
****************************************************************************************************
 * File:   adc.c
 * Version: 15.0
 * Author: ExploreEmbedded
 * Website: http://www.exploreembedded.com/wiki
 * Description: Contains the library function for ADC conversion
 
The libraries have been tested on ExploreEmbedded development boards. We strongly believe that the 
library works on any of development boards for respective controllers. However, ExploreEmbedded 
disclaims any kind of hardware failure resulting out of usage of libraries, directly or indirectly.
Files may be subject to change without prior notice. The revision history contains the information 
related to updates. 
 
 
GNU GENERAL PUBLIC LICENSE: 
    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
 
Errors and omissions should be reported to codelibraries@exploreembedded.com
 **************************************************************************************************/
 
 
 
/***************************************************************************************************
                             Revision History
****************************************************************************************************				   
15.0: Initial version 
***************************************************************************************************/

#include"adc.h"
#include"delay.h"
#include"stdutils.h"





/***************************************************************************************************
                         void ADC_Init()
****************************************************************************************************
 * I/P Arguments: none.
 * Return value	: none

 * description :This function initializes the ADC module.
***************************************************************************************************/
void ADC_Init()
{
          util_BitClear(adc_controlbus,adc_Start); 
		  util_BitClear(adc_controlbus,adc_ALE); 
		  util_BitClear(adc_controlbus,adc_OE); 
		  util_BitSet(adc_controlbus,adc_OE); 
		  adc_databus=0xff; 
		/*  
	     adc_Start=0;                //Initialize all the control lines to zero.
		 adc_ALE=0;
		 adc_OE=0;
		 adc_EOC=1;                  //Configure the EOC pin as I/P
	   adc_databus=0xff;             //configure adc_databus as input
	   */
	   
}




/***************************************************************************************************
                    uint16_t ADC_GetAdcValue(uint8_t var_adcChannel_u8);
****************************************************************************************************
 * I/P Arguments: uint8_t(channel number).
 * Return value	: uint16_t(16 bit ADC result)

 * description  :This function does the ADC conversion for the Selected Channel
                 and returns the converted 16bit result.
				 The adc value per bit depends on the resolution of the ADC. 
				 For ADC0809(8-bit ADC)) the adc value per lsb will be 5/255=0.0196V.
				 For AVR/PIC(10-bit adc) the adc value per lsb will be 5/1023=0048v

              ___     ___     ___     ___     ___     ___     ___     ___     ___
         |   |   |   |   |   |   |   |   |   |   |   |   |   |   |   |   |   |   |
Clock:   |___|   |___|   |___|   |___|   |___|   |___|   |___|   |___|   |___|   |__

Address  ----------(Address A,B,C)--------------------------------------------------
(Channel number)                    ______
                                   |      |
ALE:     __________________________|      |_________________________________________
        		        	           _______
                                      |       |
Start;   _____________________________|       |_____________________________________
    	 __________________________________________			 ______________________
 												   |		|
EOC:  											   |________|
         						                                 ______________
		                                                        |		       |
OE;     ________________________________________________________|			   |____

Data:   -----------------------------------------------------------(adc result)-----

***************************************************************************************************/
uint16_t ADC_GetAdcValue(uint8_t var_adcChannel_u8)
{
   	 uint16_t adc_result;
       /*   
   	   adc_A=((var_adcChannel_u8>>0) & 0x01);   //Selectthe channel
   	   adc_B=((var_adcChannel_u8>>1) & 0x01);   //for which the conversion needs to be done
   	   adc_C=((var_adcChannel_u8>>2) & 0x01);
       */
	    util_UpdateBit(adc_controlbus,adc_A,util_GetBitStatus(var_adcChannel_u8,0X00)); 
	    util_UpdateBit(adc_controlbus,adc_B,util_GetBitStatus(var_adcChannel_u8,0X01));	
		util_UpdateBit(adc_controlbus,adc_C,util_GetBitStatus(var_adcChannel_u8,0X02));
				
	   
		util_BitSet(adc_controlbus,adc_ALE);        // Latch the address by making the ALE high.
		DELAY_us(50);
		util_BitSet(adc_controlbus,adc_Start);       //Start the conversion after latching the channel address
		DELAY_us(25);

		util_BitClear(adc_controlbus,adc_ALE);          //Pull ALE line to zero after starting the conversion.
		DELAY_us(50);
		util_BitClear(adc_controlbus,adc_Start);;       //Pull Start line to zero after starting the conversion.

		
     while(util_GetBitStatus(adc_controlbus,adc_EOC)==0);    // Wait till the ADC conversion is completed,
                           // EOC will be pulled to HIGH by the hardware(ADC0809)
                           // once conversion is completed.

		 util_BitSet(adc_controlbus,adc_OE);          //Make the Output Enable high
		                   //to bring the ADC data to port pins
		 DELAY_us(25);
     adc_result=adc_databus;  //Read the ADC data from ADC bus
	 util_BitClear(adc_controlbus,adc_OE); 			  //After reading the data, disable th ADC output line.

	  return(adc_result) ;

  }
