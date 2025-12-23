/*

************************************************

* File: led.h

* Function: LEDheader file

* Author: Lijh

************************************************

*/

// Prevent multiple inclusion

#ifndef __LED_H

#define __LED_H

#endif



// Include headers

#include "stm32f4xx.h"



// LED

typedef enum

{

	LED1 = 1,

	LED2 = 2,

	LED3 = 3,

	LED4 = 4,

}Led_t;





// LED Macros

#define LED2_RCC			RCC_AHB1Periph_GPIOF  // clock

#define LED2_Port		  GPIOF        // 

#define LED2_Pin			GPIO_Pin_7   // 



#define LED4_RCC			RCC_AHB1Periph_GPIOC  // clock

#define LED4_Port		  GPIOC        // 

#define LED4_Pin			GPIO_Pin_2   // 





// LED function

void LedInit(void);     // initializationConfigure

void LedOn(Led_t);      // 

void LedOff(Led_t);     // 

void LedToggle(Led_t);  // 
