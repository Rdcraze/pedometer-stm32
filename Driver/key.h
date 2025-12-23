/*

************************************************

* 锟侥硷拷: key.h

* 锟斤拷锟斤拷: KEY头锟侥硷拷

* 锟斤拷锟竭ｏ拷Lijh

************************************************

*/

// 

#ifndef __KEY_H

#define __KEY_H

#endif



// 

#include "stm32f4xx.h"



// KEY

typedef enum

{

	KEY_NO = 0,

	KEY1   = 1,

	KEY2   = 2,

	KEY3   = 3,

}Key_t;



// KEY

#define KEY1_RCC			RCC_AHB1Periph_GPIOI  // 

#define KEY1_PORT		  GPIOI                 // 

#define KEY1_Pin			GPIO_Pin_8            // 



#define KEY2_RCC			RCC_AHB1Periph_GPIOC  // 

#define KEY2_PORT		  GPIOC                 // 

#define KEY2_Pin			GPIO_Pin_13           // 



#define KEY3_RCC			RCC_AHB1Periph_GPIOI  // Key3 clock

#define KEY3_PORT		  GPIOI                 // Key3 port

#define KEY3_Pin			GPIO_Pin_11           // Key3 pin PI11



// KEY

void  KeyInit(void);     // 

Key_t GetKey(void);      // 

void KeyExtiCfg(void);   // 


