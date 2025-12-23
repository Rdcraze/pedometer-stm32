/*
 ************************************************
 * File: oled.c
 * Description: OLED display driver
 * Specs: 128x64 resolution, SSD1306 chip, SPI interface
 * Author: Lijh
 ************************************************
 */
#include "oled.h"

#include "../Fonts/font8x16.h"

#include "../Fonts/font6x8.h"

#include "../Fonts/font16x16.h"



// -

#define OLED_RESET_L() 	GPIO_ResetBits(OLED_Port, OLED_RES_Pin)   // - L

#define OLED_RESET_H()	GPIO_SetBits(OLED_Port, OLED_RES_Pin)     // - H



#define OLED_DC_L()		GPIO_ResetBits(OLED_Port, OLED_DC_Pin)      // 

#define OLED_DC_H()		GPIO_SetBits(OLED_Port, OLED_DC_Pin)	      // 



// OLED

#define OLED_X		0x128 // -128

#define OLED_Y		0x64  // -64



/******/

void OLED_SPI1_Init(void);    // SPI1Init

void OLED_WR_Cmd(uint8_t cw); // Write

void OLED_WR_Dat(uint8_t dw); // Write

void OLED_SetXY(uint8_t x, uint8_t y); // x,y

void delayus(uint8_t nus);    // Delay/Clock



// Delay/Clock

void delayus(uint8_t nus)

{        

   uint8_t i;

   while(nus--) 

   { 

     for(i = 0 ; i < 25 ; i++);

   } 

}



// OLEDInit

void OledInit(void)

{

	// SPIInit

	OLED_SPI1_Init();

	// OLED

	OLED_RESET_L();

	delayus(10); // Datasheet > 3s

	OLED_RESET_H();  	

	// 

  OLED_WR_Cmd(0xae);// --turn off oled panel

  OLED_WR_Cmd(0x00);// ---set low column address

  OLED_WR_Cmd(0x10);// ---set high column address

  OLED_WR_Cmd(0x40);// --set start line address  Set Mapping RAM Display Start Line (0x00~0x3F)

  OLED_WR_Cmd(0x81);// --set contrast control register

  OLED_WR_Cmd(0xcf);// Set SEG Output Current Brightness

  OLED_WR_Cmd(0xa1);// --Set SEG/Column Mapping     0xa0 0xa1

  OLED_WR_Cmd(0xc8);// Set COM/Row Scan Direction   0xc0 0xc8

  OLED_WR_Cmd(0xa6);// --set normal display

  OLED_WR_Cmd(0xa8);// --set multiplex ratio(1 to 64)

  OLED_WR_Cmd(0x3f);// --1/64 duty

  OLED_WR_Cmd(0xd3);// -set display offset	Shift Mapping RAM Counter (0x00~0x3F)

  OLED_WR_Cmd(0x00);// -not offset

  OLED_WR_Cmd(0xd5);// --set display clock divide ratio/oscillator frequency

  OLED_WR_Cmd(0x80);// --set divide ratio, Set Clock as 100 Frames/Sec

  OLED_WR_Cmd(0xd9);// --set pre-charge period

  OLED_WR_Cmd(0xf1);// Set Pre-Charge as 15 Clocks & Discharge as 1 Clock

  OLED_WR_Cmd(0xda);// --set com pins hardware configuration

  OLED_WR_Cmd(0x12);

  OLED_WR_Cmd(0xdb);// --set vcomh

  OLED_WR_Cmd(0x40);// Set VCOM Deselect Level

  OLED_WR_Cmd(0x20);// -Set Page Addressing Mode (0x00/0x01/0x02)

  OLED_WR_Cmd(0x02);// 

  OLED_WR_Cmd(0x8d);// --set Charge Pump enable/disable

  OLED_WR_Cmd(0x14);// --set(0x10) disable

  OLED_WR_Cmd(0xa4);// Disable Entire Display On (0xa4/0xa5)

  OLED_WR_Cmd(0xa6);// Disable Inverse Display On (0xa6/a7)

  OLED_WR_Cmd(0xaf);// --turn on oled panel

}



// SPIInit

void OLED_SPI1_Init(void)

{

	GPIO_InitTypeDef 	GPIO_InitStructure;

	SPI_InitTypeDef   SPI_InitStructure;



  // Delay/Clock

	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);	

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_SPI1,  ENABLE);

	

	// PB3-SCK, PB5-DINSPI

	GPIO_PinAFConfig(GPIOB, GPIO_PinSource3, GPIO_AF_SPI1);

	GPIO_PinAFConfig(GPIOB, GPIO_PinSource5, GPIO_AF_SPI1);

	

	// InitGPIO PB3-SCK, PB5-DIN,

	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;  /**/

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3 | GPIO_Pin_5;

	GPIO_Init(GPIOB, &GPIO_InitStructure);

	

	// InitGPIO PB6-RESET, PPB9-DC,

	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;		  /*  */

	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;		/*  */

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6 | GPIO_Pin_9;

	GPIO_Init(GPIOB, &GPIO_InitStructure);

	

	// SPI1

	SPI_InitStructure.SPI_Direction = SPI_Direction_2Lines_FullDuplex;  /*  */

	SPI_InitStructure.SPI_Mode = SPI_Mode_Master;       /**/

	SPI_InitStructure.SPI_DataSize = SPI_DataSize_8b;   /* 8bits */

	SPI_InitStructure.SPI_CPOL = SPI_CPOL_High;  /*Delay/ClockCPOL(Init)1 */

	SPI_InitStructure.SPI_CPHA = SPI_CPHA_2Edge; /*Delay/ClockCPHA */

	  /*SPI1 locates in APB2, BaudRate = fAPB2/16 = 84M/16 = 5.25M*/

	SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_16; /* 16*/

  SPI_InitStructure.SPI_FirstBit=SPI_FirstBit_MSB; /*MSB*/

	SPI_InitStructure.SPI_NSS = SPI_NSS_Soft;  /*NSS*/

	SPI_Init(SPI1, &SPI_InitStructure);  /**/

	SPI_Cmd (SPI1, ENABLE);	             /*SPI1*/

}



// OLEDWrite

void OLED_WR_Cmd(uint8_t cw)

{

	OLED_DC_L(); // 

	SPI_I2S_SendData(SPI1, cw);

	while(SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_TXE) == 0); /*  */	

	delayus(2);

}



// OLEDWrite

void OLED_WR_Dat(uint8_t dat)

{

	OLED_DC_H(); // 

	SPI_I2S_SendData(SPI1, dat);

	while(SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_TXE) == 0); /*  */	

	delayus(2);

}



// OLEDx,y, x0~127y0~63 -

void OLED_SetXY(uint8_t x, uint8_t y)

{

	OLED_WR_Cmd(0xb0+(y>>3));

  OLED_WR_Cmd(((x&0xf0)>>4)|0x10);

  OLED_WR_Cmd((x&0x0f)|0x01); 

}



/*****.cFile****/

// OLED-

void OledClear(void)

{

	uint8_t i,j;

	for(i=0; i<8; i++)

	{

		OLED_WR_Cmd(0xb0+i);

		OLED_WR_Cmd(0x01);

		OLED_WR_Cmd(0x10);

		for(j=0; j<128; j++)

			OLED_WR_Dat(0x00);

	}

}



// Initx,y816 - 816

void OledShowFont8X16(uint8_t x, uint8_t y, const uint8_t *pixel)

{

  uint8_t i, j ;

	for(i = 0; i < 2; i++)

	 {

		 OLED_SetXY(x , (y + i)*8); // *8,8OLED Y

		 for(j = 0 ; j < 8; j++)

			 OLED_WR_Dat(pixel[i * 8 + j]);

	 }

}



// Initx,y1616 - 1616

void OledShowFont16X16(uint8_t x, uint8_t y, const uint8_t *pixel)

{

  uint8_t i, j ;

	for(i = 0; i < 2; i++)

	 {

		 OLED_SetXY(x , (y + i)*8); // *8,8OLED Y

		 for(j = 0 ; j < 16; j++)

			 OLED_WR_Dat(pixel[i * 16 + j]);

	 }	

}



// 12864 - 12864

void OledShowPic128X64(const uint8_t *pic)

{

  uint8_t i, j ;

	for(i = 0; i < 8; i++)

	 {

		 OLED_SetXY(0 , i*8);      // *8,8Y

		 for(j = 0 ; j < 128; j++) // 0~1261270

			OLED_WR_Dat(pic[i * 128 + j]);

	 }

}



/*************************************************************

 * New helper functions using Fonts/ folder

 *************************************************************/



// Display single ASCII character using 8x16 font

void OledShowChar8x16(uint8_t x, uint8_t y, char ch)

{

    uint8_t i, j;

    uint8_t c = ch - ' ';  // Font starts at space (0x20)



    if (c > 94) return;  // Invalid character



    for(i = 0; i < 2; i++)

    {

        OLED_SetXY(x, (y + i) * 8);

        for(j = 0; j < 8; j++)

        {

            OLED_WR_Dat(Font8x16[c][i * 8 + j]);

        }

    }

}



// Display ASCII string using 8x16 font

void OledShowString8x16(uint8_t x, uint8_t y, const char *str)

{

    while(*str != '\0')

    {

        OledShowChar8x16(x, y, *str);

        x += 8;

        if(x > 120)  // Wrap to next line

        {

            x = 0;

            y += 2;

        }

        str++;

    }

}



// Display single ASCII character using 6x8 font

void OledShowChar6x8(uint8_t x, uint8_t y, char ch)

{

    uint8_t j;

    uint8_t c = ch - ' ';  // Font starts at space (0x20)



    if (c > 94) return;  // Invalid character



    OLED_SetXY(x, y * 8);

    for(j = 0; j < 6; j++)

    {

        OLED_WR_Dat(Font6x8[c][j]);

    }

}



// Display ASCII string using 6x8 font

void OledShowString6x8(uint8_t x, uint8_t y, const char *str)

{

    while(*str != '\0')

    {

        OledShowChar6x8(x, y, *str);

        x += 6;

        if(x > 122)  // Wrap to next line

        {

            x = 0;

            y += 1;

        }

        str++;

    }

}



// Display unsigned integer with specified length

void OledShowNum(uint8_t x, uint8_t y, uint32_t num, uint8_t len)

{

    char buf[12];

    uint8_t i;



    // Convert number to string (right-aligned)

    for(i = 0; i < len; i++)

    {

        buf[len - 1 - i] = '0' + (num % 10);

        num /= 10;

    }

    buf[len] = '\0';



    OledShowString8x16(x, y, buf);

}



// Draw progress bar at specified y position

// Uses 6x8 font row, draws a bar like [========>        ]

void OledDrawProgressBar(uint8_t y, uint8_t percent)

{

    uint8_t i;

    uint8_t filled;

    char bar[18];  // "[" + 15 chars + "]" + null



    if(percent > 100) percent = 100;

    filled = (percent * 15) / 100;  // 15 character width



    bar[0] = '[';

    for(i = 0; i < 15; i++)

    {

        if(i < filled)

            bar[i + 1] = '=';

        else if(i == filled && percent > 0 && percent < 100)

            bar[i + 1] = '>';

        else

            bar[i + 1] = ' ';

    }

    bar[16] = ']';

    bar[17] = '\0';



    OledShowString6x8(0, y, bar);

}



// Display Chinese character by index (16x16 font)

void OledShowChinese16x16(uint8_t x, uint8_t y, uint8_t index)

{

    uint8_t i, j;



    if(index >= 61) return;  // Font has 61 characters



    for(i = 0; i < 2; i++)

    {

        OLED_SetXY(x, (y + i) * 8);

        for(j = 0; j < 16; j++)

        {

            OLED_WR_Dat(Font16x16[index][i * 16 + j]);

        }

    }

}



// Display Chinese string by indices array

void OledShowChineseString(uint8_t x, uint8_t y, const uint8_t *indices, uint8_t len)

{

    uint8_t i;

    for(i = 0; i < len; i++)

    {

        OledShowChinese16x16(x, y, indices[i]);

        x += 16;

        if(x > 112)  // Wrap to next line

        {

            x = 0;

            y += 2;

        }

    }

}



// Walking figure sprites (8x8 pixels each, 8 bytes)

// Simple stick figures for page 7

static const uint8_t sprite_idle[8] = {

    0x00,0x1C,0x22,0x1C,0x08,0x3E,0x08,0x14  // Standing figure

};



static const uint8_t sprite_walk1[8] = {

    0x00,0x1C,0x22,0x1C,0x08,0x1C,0x22,0x22  // Left leg forward

};



static const uint8_t sprite_walk2[8] = {

    0x00,0x1C,0x22,0x1C,0x08,0x1C,0x08,0x14  // Right leg forward

};



// Draw 16-pixel high bitmap at position

void OledDrawBitmap16(uint8_t x, uint8_t y, const uint8_t *bitmap, uint8_t width)

{

    uint8_t i;

    uint8_t page = y / 8;



    // Draw top half

    OLED_SetXY(x, page * 8);

    for(i = 0; i < width; i++)

    {

        OLED_WR_Dat(bitmap[i]);

    }



    // Draw bottom half

    OLED_SetXY(x, (page + 1) * 8);

    for(i = 0; i < width; i++)

    {

        OLED_WR_Dat(bitmap[width + i]);

    }

}



// Draw animated progress bar (page 6) with walking figure (page 7)

// percent: 0-100, state: 0=idle, 1=walk, 2=run, frame: animation frame (0-7)

void OledDrawAnimatedProgress(uint8_t percent, uint8_t state, uint8_t frame)

{

    uint8_t i;

    uint8_t filled;

    uint8_t figureX;

    const uint8_t *sprite;

    uint8_t page6[128];

    uint8_t page7[128];

    uint8_t stripe_offset;



    if(percent > 100) percent = 100;



    // Vertical stripe offset (0-3) - stripes move horizontally

    stripe_offset = frame & 0x03;



    // Calculate filled portion (columns 1-126, leaving borders)

    filled = 1 + (percent * 126) / 100;



    // Calculate figure position (0 to 119, so sprite ends at index 126, avoiding wrap)

    figureX = (percent * 119) / 100;



    // Select sprite based on state

    if(state == 0)

        sprite = sprite_idle;

    else

        sprite = (frame & 0x01) ? sprite_walk2 : sprite_walk1;



    // Build page 6 - progress bar with vertical flowing stripes

    for(i = 0; i < 128; i++)

    {

        if(i == 0 || i == 127)

            page6[i] = 0xFF;  // Left/right border

        else if(i < filled)

        {

            // Vertical stripe every 4 columns, moving right

            if(((i + 4 - stripe_offset) & 0x03) == 0)

                page6[i] = 0xFF;  // Stripe column (full)

            else

                page6[i] = 0x81;  // Non-stripe column (just border)

        }

        else

            page6[i] = 0x81;  // Empty - just top/bottom border

    }



    // Build page 7 - clear then draw walking figure

    for(i = 0; i < 128; i++)

        page7[i] = 0x00;



    // Draw sprite at figure position

    for(i = 0; i < 8; i++)

    {

        if(figureX + i < 128)

            page7[figureX + i] = sprite[i];

    }



    // Write page 6 to screen

    OLED_SetXY(0, 48);  // Page 6

    for(i = 0; i < 128; i++)

        OLED_WR_Dat(page6[i]);



    // Write page 7 to screen

    OLED_SetXY(0, 56);  // Page 7

    for(i = 0; i < 128; i++)

        OLED_WR_Dat(page7[i]);

}
