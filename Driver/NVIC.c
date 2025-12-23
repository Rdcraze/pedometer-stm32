/*
************************************************
* File: NVIC.c
* Description: NVIC interrupt configuration
************************************************
*/
#include "NVIC.h"

// NVIC initialization
void NvicCfg(void)
{
	NVIC_InitTypeDef  NVIC_InitStructure;

	// Configure the NVIC Priority Group
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_1);

	// TIM3 NVIC config - 50Hz sampling for pedometer
	NVIC_InitStructure.NVIC_IRQChannel = TIM3_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
}

