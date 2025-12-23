/*
************************************************
* File: flash_data.h
* Description: Flash data storage for pedometer
* Uses Sector 11 (0x080E0000) of STM32F407 internal Flash
************************************************
*/
#ifndef __FLASH_DATA_H
#define __FLASH_DATA_H

#include "stm32f4xx.h"

// Flash sector 11 address (last 128KB sector, safe from program code)
#define FLASH_DATA_SECTOR       FLASH_Sector_11
#define FLASH_DATA_ADDR         0x080E0000
#define FLASH_DATA_MAGIC        0xCAFEBABE  // "CAFEBABE" marker

// Data structure to store in Flash
typedef struct {
    uint32_t magic;           // Validation marker (FLASH_DATA_MAGIC)
    uint32_t target_steps;    // Step target
    uint32_t step_count;      // Current step count (for resume)
    uint16_t height_cm;       // User height in cm
    uint16_t weight_kg;       // User weight in kg
    uint8_t  language_cn;     // Language: 1=Chinese, 0=English
    uint8_t  pace_mode;       // 0=km/h, 1=min/km
    uint8_t  reserved[2];     // Padding for alignment
    uint32_t checksum;        // Simple checksum for validation
} FlashData_t;

// Function declarations
void FlashDataInit(void);
uint8_t FlashDataLoad(FlashData_t *data);
uint8_t FlashDataSave(const FlashData_t *data);
uint8_t FlashDataIsValid(void);

#endif