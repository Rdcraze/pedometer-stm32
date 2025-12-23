/*

************************************************

* File: lcd.c

* Function: LCD驱动function库

* 型号：NOKIA5110，分辨率84*48，SPI接口

* Author: Lijh

************************************************

*/

// Include header files

#include "lcd.h"



// Macros -

#define LCD_RESET_L() 	GPIO_ResetBits(LCD_Port, LCD_RST_Pin)   // - L

#define LCD_RESET_H() 	GPIO_SetBits(LCD_Port, LCD_RST_Pin)     // - H



#define LCD_DC_L()		  GPIO_ResetBits(LCD_Port, LCD_DC_Pin)    // mode

#define LCD_DC_H()		  GPIO_SetBits(LCD_Port, LCD_DC_Pin)	    // datamode



// LCD

#define LCD_X		0x84  // -84

#define LCD_Y		0x48  // -48



/***function***/

void LCD_SPI1_Init(void);    // SPI1

void LCD_WR_Cmd(uint8_t cw); // Write

void LCD_WR_Dat(uint8_t dw); // Writedata

void LCD_SetXY(uint8_t x, uint8_t y); // x,y

void delayus(uint8_t nus);    // function



// function

void delayus(uint8_t nus)

{        

   uint8_t i;

   while(nus--) 

   { 

     for(i = 0 ; i < 25 ; i++);

   } 

}



// LCDinitialization

void LcdInit(void)

{

	// SPIinitialization

	LCD_SPI1_Init();

	// LCD

	LCD_RESET_L();

	delayus(10);

	LCD_RESET_H();  	

	// 

	LCD_WR_Cmd(0x21);

	LCD_WR_Cmd(0xBF);

	LCD_WR_Cmd(0x13);

	LCD_WR_Cmd(0x20); // V=0

	LCD_WR_Cmd(0x0C);

}



// SPIinitialization

void LCD_SPI1_Init(void)

{

	GPIO_InitTypeDef 	GPIO_InitStructure;

	SPI_InitTypeDef   SPI_InitStructure;



  // clock

	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);	

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_SPI1,  ENABLE);

	

	// Enable PB3-SCK, PB5-DINSPIAF

	GPIO_PinAFConfig(GPIOB, GPIO_PinSource3, GPIO_AF_SPI1);

	GPIO_PinAFConfig(GPIOB, GPIO_PinSource5, GPIO_AF_SPI1);

	

	// initializationGPIO PB3-SCK, PB5-DIN, AF mode

	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;  /*AF mode*/

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3 | GPIO_Pin_5;

	GPIO_Init(GPIOB, &GPIO_InitStructure);

	

	// initializationGPIO PB6-RESET, PPB9-DC,

	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;		  /*  */

	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;		/* mode */

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6 | GPIO_Pin_9;

	GPIO_Init(GPIOB, &GPIO_InitStructure);

	

	// Configure SPI1

	SPI_InitStructure.SPI_Direction = SPI_Direction_2Lines_FullDuplex;  /*  */

	SPI_InitStructure.SPI_Mode = SPI_Mode_Master;       /*mastermode*/

	SPI_InitStructure.SPI_DataSize = SPI_DataSize_8b;   /* data8bits */

	SPI_InitStructure.SPI_CPOL = SPI_CPOL_High;  /*clockCPOL()1 */

	SPI_InitStructure.SPI_CPHA = SPI_CPHA_2Edge; /*clockCPHA */

	  /*SPI1 locates in APB2, BaudRate = fAPB2/16 = 84M/16 = 5.25M*/

	SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_16; /* 16*/

  SPI_InitStructure.SPI_FirstBit=SPI_FirstBit_MSB; /*MSB*/

	SPI_InitStructure.SPI_NSS = SPI_NSS_Soft;  /*NSS*/

	SPI_Init(SPI1, &SPI_InitStructure);  /*Complete Configure */

	SPI_Cmd (SPI1, ENABLE);	             /*SPI1*/

}



// LCDWrite

void LCD_WR_Cmd(uint8_t cw)

{

	LCD_DC_L(); // mode

	SPI_I2S_SendData(SPI1, cw);

	while(SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_TXE) == 0); /* Wait  */	

	delayus(2);

}



// LCDWritedata

void LCD_WR_Dat(uint8_t dat)

{

	LCD_DC_H(); // datamode

	SPI_I2S_SendData(SPI1, dat);

	while(SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_TXE) == 0); /* Wait  */	

	delayus(2);

}



// LCDx,y, x0~83y0~5 -

void LCD_SetXY(uint8_t x, uint8_t y)

{

	// this LCD have only 84*48 pixel, 48/8=6,thats 0~5.

	if(x > 83)

		x= 0;

	if(y > 5)

		y = 0;

	LCD_WR_Cmd(0x40 | y);  // reference to the 5110 user manual, table 1.

	LCD_WR_Cmd(0x80 | x);

}



/*****function.c****/

// LCD-Write0

void LcdClear(void)

{

	uint8_t i,j;

	LCD_SetXY(0,0);

	// 84*48 pixels.

	for(i = 0; i < 84; i++)

		{

			for(j = 0; j < 6; j++)

				LCD_WR_Dat(0);

		}

}



// Startx,y816 - 816

void LcdShowFont8X16(uint8_t x, uint8_t y, const uint8_t *pixel)

{

  uint8_t i, j ;

	for(i = 0; i < 2; i++)              // 

	 {

		 LCD_SetXY(x , y + i);  

		 for(j = 0 ; j < 8; j++)          // Write

			 LCD_WR_Dat(pixel[i * 8 + j]);

	 }

}



// Startx,y1616 - 1616

void LcdShowFont16X16(uint8_t x, uint8_t y, const uint8_t *pixel)

{

  uint8_t i, j ;

	for(i = 0; i < 2; i++)              // 

	 {

		 LCD_SetXY(x , y + i); 

		 for(j = 0 ; j < 16; j++)

			 LCD_WR_Dat(pixel[i * 16 + j]); // Write

	 }	

}



// 8448 - 8448

void LcdShowPic84X48(const uint8_t *pic)

{

  uint8_t i, j ;

	for(i = 0; i < 6; i++)               // 

	 {

		 LCD_SetXY(0 , i);      

		 for(j = 0 ; j < 84; j++) 

			LCD_WR_Dat(pic[i * 84 + j]);     // Write

	 }	

}


