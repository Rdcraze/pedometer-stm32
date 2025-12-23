#ifndef __FONT6X8_H
#define __FONT6X8_H

#include "stm32f4xx.h"

// 6x8 ASCII font (95 characters from space to ~)
// Each character is 6 bytes (6 pixels wide, 8 pixels tall)
extern const uint8_t Font6x8[95][6];

#endif
