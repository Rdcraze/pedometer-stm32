/*


************************************************


* File: uart.c


* Function: UART操作function


* Author: Lijh


************************************************


*/


// Include header files


#include "uart.h"





/*UARTinitialization */


void UartInit(void)


{


	GPIO_InitTypeDef  GPIO_InitStructure;


	USART_InitTypeDef USART_InitStructure;


	/****************************USART1: TX = PA9, RX = PA10 *****************************/


	/* Enable device clock */


	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA,  ENABLE);  // GPIOA clock


	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE);  // USART1 clock





	/* Enable GPIOAF USART */


	GPIO_PinAFConfig(GPIOA, GPIO_PinSource9, GPIO_AF_USART1);  // PA9AF


	GPIO_PinAFConfig(GPIOA, GPIO_PinSource10, GPIO_AF_USART1); // PA10AF





	/* Configure GPIO */


	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9 | GPIO_Pin_10;


	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;            // AF mode


// GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;	        //


	GPIO_Init(GPIOA, &GPIO_InitStructure);





	/* Configure USART */


	USART_InitStructure.USART_BaudRate = 9600;	                    // Baud rate


	USART_InitStructure.USART_WordLength = USART_WordLength_8b  ;   // 8bit bits data


	USART_InitStructure.USART_StopBits = USART_StopBits_1;          // 1bit stop bit


	USART_InitStructure.USART_Parity = USART_Parity_No ;            // parity


	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx; // RX/TXmode


	USART_Init(USART1, &USART_InitStructure);                       // Complete initialization


	USART_Cmd(USART1, ENABLE);		                                  // Enable UART


	USART1->SR &= ~0x0040;                                          // SRregisterTC


}


		


/*byteSend function */


void UartSendByte(USART_TypeDef *USARTx, uint8_t ch)


{


	 USART_SendData(USARTx, ch);


	 /* Wait Send Stop */


	 while (USART_GetFlagStatus(USARTx, USART_FLAG_TC) == 0){ };  // Wait until TC flag set


}





/*Send function */


void UartSendString(USART_TypeDef *USARTx, char *str)


{


	 uint16_t k =0; 


	 while (*(str+k) != '\0') // Continue until null terminator


	 {


		  USART_SendData(USARTx, *(str+k));


		  k++;


		  while (USART_GetFlagStatus(USARTx, USART_FLAG_TC) == 0){ }; // Wait until TC flag set





	 }


}





/*byteReceive function */


uint8_t UartReceiveByte(USART_TypeDef *USARTx)


{


	uint8_t ch;


  while (USART_GetFlagStatus(USARTx, USART_FLAG_RXNE) == 0) {}; // Wait until RXNE flag set


	ch = USART_ReceiveData(USARTx);


	return ch;


}







