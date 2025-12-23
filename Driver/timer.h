/*

************************************************

* File: timer.h

* Function: Timer设备header file

* Author: Lijh

************************************************

*/

// Prevent multiple inclusion

#ifndef __TIMER_H

#define __TIMER_H

#endif



// Include headers

#include "stm32f4xx.h"



// function

void Tim2Init(void); /*initializationfunction*/

void Tim3Init(uint16_t time);  /*initializationfunction*/
