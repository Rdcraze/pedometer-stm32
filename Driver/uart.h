/*

************************************************

* File: uart.h

* Function: UART操作header file

* Author: Lijh

************************************************

*/

// Prevent multiple inclusion

#ifndef __UART_H

#define __UART_H

#endif



// Include headers

#include "stm32f4xx.h"



// function



void UartInit(void); 	                                  /*UARTinitialization */

void UartSendByte(USART_TypeDef *USARTx, uint8_t ch);	  /*byteSend function */

void UartSendString(USART_TypeDef *USARTx, char *str);  /*Send function */

uint8_t UartReceiveByte(USART_TypeDef *USARTx);         /*byteReceive function */








