/*
*********************************************************************************************************
*  File: stm32f4xxx_it.c
*  Description: Interrupt handlers for pedometer
*********************************************************************************************************
*/
#include "timer.h"

// External flags defined in main.c
extern volatile uint8_t sample_flag;

/*************************************
* TIM3 interrupt handler
* 50Hz (20ms) sampling for pedometer
**************************************
*/
void TIM3_IRQHandler(void)
{
	if (TIM_GetITStatus(TIM3, TIM_IT_Update) == 1)
	{
		sample_flag = 1;  // Signal main loop to sample accelerometer
		TIM_ClearFlag(TIM3, TIM_IT_Update);
	}
}
