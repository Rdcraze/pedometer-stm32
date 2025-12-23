/*
************************************************
* File: flash_data.c
* Description: Flash data storage implementation
* Uses internal Flash Sector 11 for persistent storage
************************************************
*/
#include "flash_data.h"
#include "stm32f4xx_flash.h"

// Calculate simple checksum
static uint32_t CalcChecksum(const FlashData_t *data)
{
    uint32_t sum = 0;
    const uint8_t *ptr = (const uint8_t *)data;
    uint32_t i;

    // Sum all bytes except the checksum field itself
    for(i = 0; i < sizeof(FlashData_t) - sizeof(uint32_t); i++)
    {
        sum += ptr[i];
    }
    return sum ^ 0xA5A5A5A5;  // XOR with pattern
}

// Initialize flash data module
void FlashDataInit(void)
{
    // Nothing special needed for basic init
}

// Check if valid data exists in Flash
uint8_t FlashDataIsValid(void)
{
    FlashData_t *flash_data = (FlashData_t *)FLASH_DATA_ADDR;

    // Check magic number
    if(flash_data->magic != FLASH_DATA_MAGIC)
        return 0;

    // Verify checksum
    if(flash_data->checksum != CalcChecksum(flash_data))
        return 0;

    return 1;
}

// Load data from Flash
// Returns 1 if successful, 0 if no valid data
uint8_t FlashDataLoad(FlashData_t *data)
{
    FlashData_t *flash_data = (FlashData_t *)FLASH_DATA_ADDR;
    uint32_t i;
    const uint32_t *src;
    uint32_t *dst;

    // Check if valid data exists
    if(!FlashDataIsValid())
        return 0;

    // Copy data from Flash to RAM
    src = (const uint32_t *)flash_data;
    dst = (uint32_t *)data;
    for(i = 0; i < sizeof(FlashData_t) / 4; i++)
    {
        dst[i] = src[i];
    }

    return 1;
}

// Save data to Flash
// Returns 1 if successful, 0 if failed
uint8_t FlashDataSave(const FlashData_t *data)
{
    FlashData_t temp_data;
    uint32_t i;
    const uint32_t *src;
    FLASH_Status status;

    // Copy data and set magic/checksum
    temp_data = *data;
    temp_data.magic = FLASH_DATA_MAGIC;
    temp_data.checksum = CalcChecksum(&temp_data);

    // Unlock Flash
    FLASH_Unlock();

    // Clear pending flags
    FLASH_ClearFlag(FLASH_FLAG_EOP | FLASH_FLAG_OPERR | FLASH_FLAG_WRPERR |
                    FLASH_FLAG_PGAERR | FLASH_FLAG_PGPERR | FLASH_FLAG_PGSERR);

    // Erase Sector 11
    status = FLASH_EraseSector(FLASH_DATA_SECTOR, VoltageRange_3);
    if(status != FLASH_COMPLETE)
    {
        FLASH_Lock();
        return 0;
    }

    // Program data word by word
    src = (const uint32_t *)&temp_data;
    for(i = 0; i < sizeof(FlashData_t) / 4; i++)
    {
        status = FLASH_ProgramWord(FLASH_DATA_ADDR + (i * 4), src[i]);
        if(status != FLASH_COMPLETE)
        {
            FLASH_Lock();
            return 0;
        }
    }

    // Lock Flash
    FLASH_Lock();

    return 1;
}