/*
************************************************
* File: oled.h
* Description: OLED display driver header
* Resolution: 128*64, Controller: SSD1306, Interface: SPI
************************************************
*/
#ifndef __OLED_H
#define __OLED_H

#include "stm32f4xx.h"

// Port configuration
#define OLED_RCC               RCC_AHB1Periph_GPIOB
#define OLED_Port              GPIOB
#define OLED_SCK_Pin           GPIO_Pin_3   // PB3 Clock
#define OLED_SDA_Pin           GPIO_Pin_5   // PB5 Data (MOSI)
#define OLED_RES_Pin           GPIO_Pin_6   // PB6 Reset
#define OLED_DC_Pin            GPIO_Pin_9   // PB9 Data/Command
#define OLED_CS_Pin            GND          // Chip select tied to GND

// Function declarations
void OledInit(void);
void OledClear(void);
void OledShowFont8X16(uint8_t x, uint8_t y, const uint8_t *pixel);
void OledShowFont16X16(uint8_t x, uint8_t y, const uint8_t *pixel);
void OledShowPic128X64(const uint8_t *pic);

// ASCII display functions
void OledShowChar8x16(uint8_t x, uint8_t y, char ch);
void OledShowString8x16(uint8_t x, uint8_t y, const char *str);
void OledShowChar6x8(uint8_t x, uint8_t y, char ch);
void OledShowString6x8(uint8_t x, uint8_t y, const char *str);
void OledShowNum(uint8_t x, uint8_t y, uint32_t num, uint8_t len);

// Progress bar (0-100%)
void OledDrawProgressBar(uint8_t y, uint8_t percent);

// Chinese character display (16x16 font)
void OledShowChinese16x16(uint8_t x, uint8_t y, uint8_t index);
void OledShowChineseString(uint8_t x, uint8_t y, const uint8_t *indices, uint8_t len);

// Bitmap and animation functions
void OledDrawBitmap16(uint8_t x, uint8_t y, const uint8_t *bitmap, uint8_t width);
void OledDrawAnimatedProgress(uint8_t percent, uint8_t state, uint8_t frame);

#endif