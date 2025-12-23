/*

************************************************

* �File�: timer.c

* ����: ��Delay/Clock���Ĳ�������

* ���ߣ�Lijh

************************************************

*/

/*---------------------------------------------------------------------------------

APB1 ����Ƶ��84MHz, ��Delay/Clock���� TIM2, TIM3 ,TIM4, TIM5, TIM6, TIM7, TIM12, TIM13,TIM14

APB2 ����Ƶ��168MHz,��Delay/Clock���� TIM1, TIM8 ,TIM9, TIM10, TIM11

--------------------------------------------------------------------------------- */

// File

#include "timer.h"



 /*Delay/Clock2Init */

	void Tim2Init(void)

{

  TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;

	TIM_Cmd(TIM2, DISABLE);

  RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2 , ENABLE);

 	TIM_TimeBaseStructure.TIM_Period =    10000;

	TIM_TimeBaseStructure.TIM_Prescaler = 8400-1;

 	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up; // 

	TIM_TimeBaseInit(TIM2,&TIM_TimeBaseStructure);		  

  TIM_ITConfig(TIM2, TIM_IT_Update, ENABLE);

	TIM_ClearFlag(TIM2, TIM_FLAG_Update);

  TIM_Cmd(TIM2, ENABLE);

}	



 /*Delay/Clock3Init - 50Hz for pedometer sampling */

	void Tim3Init(uint16_t time)

{

  TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;

	TIM_Cmd(TIM3, DISABLE);

  RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3 , ENABLE);

	// For 50Hz (20ms): 84MHz / 8400 = 10kHz, 10000 / 200 = 50Hz

 	TIM_TimeBaseStructure.TIM_Period = 200 - 1;      // 50Hz = 20ms

	TIM_TimeBaseStructure.TIM_Prescaler = 8400-1;     // 84MHz / 8400 = 10kHz

 	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;

	TIM_TimeBaseInit(TIM3, &TIM_TimeBaseStructure);

  TIM_ITConfig(TIM3, TIM_IT_Update, ENABLE);

	TIM_ClearFlag(TIM3, TIM_FLAG_Update);

  TIM_Cmd(TIM3, ENABLE);

}	




