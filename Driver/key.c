/*

************************************************

* �File�: key.c

* ����: KeyԴ�File�

* ���ߣ�Lijh

************************************************

*/

// File

#include "key.h"



// Init

void KeyExtiCfg(void)

{	

	EXTI_InitTypeDef  EXTI_InitStructure;

	/* EXTISYSCFGDelay/Clock */

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG,ENABLE);

	

	/* Key1(PI8)EXTI*/

	SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOI, EXTI_PinSource8); 

	

	/*Key1(PI8)EXTI*/

	EXTI_InitStructure.EXTI_Line = EXTI_Line8;               // 

	EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;      // 

	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling;  // 

	EXTI_InitStructure.EXTI_LineCmd=  ENABLE;                // 

	EXTI_Init(&EXTI_InitStructure);

}



// Init

void KeyInit(void)

{

	GPIO_InitTypeDef GPIO_InitStructure;

	/* GPIODelay/Clock */

	RCC_AHB1PeriphClockCmd(KEY1_RCC | KEY2_RCC | KEY3_RCC, ENABLE);



	/* */

	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;		  /*  */

	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;	    /* Internal pull-up */

	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;	/* IO */

  /* Init*/

	GPIO_InitStructure.GPIO_Pin = KEY1_Pin;

	GPIO_Init(KEY1_PORT, &GPIO_InitStructure);



	GPIO_InitStructure.GPIO_Pin = KEY2_Pin;

	GPIO_Init(KEY2_PORT, &GPIO_InitStructure);



	GPIO_InitStructure.GPIO_Pin = KEY3_Pin;

	GPIO_Init(KEY3_PORT, &GPIO_InitStructure);

}



// File()

Key_t GetKey(void)

{

	uint8_t temp1,temp2,temp3;

	Key_t key = KEY_NO; // 

	/**/

	temp1 = GPIO_ReadInputDataBit(KEY1_PORT, KEY1_Pin);

	temp2 = GPIO_ReadInputDataBit(KEY2_PORT, KEY2_Pin);

	temp3 = GPIO_ReadInputDataBit(KEY3_PORT, KEY3_Pin);

	/**/

  if(temp1 == 0x00)

		key = KEY1;

	if(temp2 == 0x00)

		key = KEY2;

	if(temp3 == 0x00)

		key = KEY3;

	return key; // 

}
