/*

************************************************

* File: lcd.h

* Function: LCD驱动header file

* 型号：NOKIA5110，分辨率84*48，SPI接口

* Author: Lijh

************************************************

*/

// Prevent multiple inclusion

#ifndef __LCD_H

#define __LCD_H

#endif



// Include headers

#include "stm32f4xx.h"



// 

#define LCD_RCC    			   RCC_AHB1Periph_GPIOB  // clock

#define LCD_Port					 GPIOB



#define LCD_CLK_Pin			 	 GPIO_Pin_3	      // PB3 clock

#define LCD_DIN_Pin				 GPIO_Pin_5	      // PB5 data-DIN-MOSI

#define LCD_RST_Pin				 GPIO_Pin_6	      // PB6 -RESET

#define LCD_DC_Pin				 GPIO_Pin_9       // PB9 data/

#define LCD_CE_Pin				 GND       	      // 

#define LCD_BL_Pin         VCC/3.3V/NULL    // Vcc3.3V



/*function*/

 // LCDinitializationfunction

void LcdInit(void);

 // LCDfunction

void LcdClear(void); 

 // Startx,y816 - 816

void LcdShowFont8X16(uint8_t x, uint8_t y, const uint8_t *pixel);

 // Startx,y1616 - 1616

void LcdShowFont16X16(uint8_t x, uint8_t y, const uint8_t *pixel);

 // 84*48 - 8448

void LcdShowPic84X48(const uint8_t *pic);

 // Addfunction

 // ......

 
