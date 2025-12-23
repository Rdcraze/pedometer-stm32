/*

************************************************

* File: led.c

* Function: LEDsource file

* Author: Lijh

************************************************

*/



// Include headers

#include "led.h"



// initializationLED

void LedInit(void)

{

	GPIO_InitTypeDef  GPIO_InitStructure;

	/* GPIOclock */

	RCC_AHB1PeriphClockCmd(LED2_RCC | LED4_RCC , ENABLE);  

	

	/* Configure GPIOmode */

	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;		    /*  */

	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;		  /* mode */

	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;	  /* Enable  */

	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;	  /* IOspeed */

	

  /* Complete LEDinitialization */

 	GPIO_InitStructure.GPIO_Pin = LED2_Pin;

	GPIO_Init(LED2_Port, &GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Pin = LED4_Pin;

	GPIO_Init(LED4_Port, &GPIO_InitStructure);

}



// LED

void LedOn(Led_t led)   /*led LED*/

{

	if(led == LED2)

		GPIO_ResetBits(LED2_Port , LED2_Pin);

	if(led == LED4)

		GPIO_ResetBits(LED4_Port , LED4_Pin);

}



// LED

void LedOff(Led_t led)   /*led LED*/

{

	if(led == LED2)

		GPIO_SetBits(LED2_Port , LED2_Pin);

	if(led == LED4)

		GPIO_SetBits(LED4_Port , LED4_Pin);

}



// LED

void LedToggle(Led_t led)   /*led LED*/

{

	if(led == LED2)

		GPIO_ToggleBits(LED2_Port , LED2_Pin);

	if(led == LED4)

		GPIO_ToggleBits(LED4_Port , LED4_Pin);

}


