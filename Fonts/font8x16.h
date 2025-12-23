#ifndef __FONT8X16_H
#define __FONT8X16_H

#include "stm32f4xx.h"

// 8x16 ASCII font (95 characters from space to ~)
// Each character is 16 bytes (8 pixels wide, 16 pixels tall)
extern const uint8_t Font8x16[95][16];

#endif
