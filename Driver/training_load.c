/*
************************************************
* File: training_load.c
* Description: sRPE-based training load calculator
* Training Load = RPE * Duration(minutes)
************************************************
*/
#include "training_load.h"

// Internal state variables
static uint32_t total_load_x100 = 0;  // Training load * 100 for precision

// Get RPE value based on cadence
static uint8_t GetRPE(uint16_t cadence)
{
    if (cadence < CADENCE_LIGHT)
        return RPE_VERY_LIGHT;
    else if (cadence < CADENCE_MODERATE)
        return RPE_LIGHT;
    else if (cadence < CADENCE_HARD)
        return RPE_MODERATE;
    else if (cadence < CADENCE_VERY_HARD)
        return RPE_HARD;
    else
        return RPE_VERY_HARD;
}

// Initialize training load calculator
void TrainingLoadInit(void)
{
    total_load_x100 = 0;
}

// Update training load calculation
// Call this periodically with current cadence and time delta
void TrainingLoadUpdate(uint16_t cadence, uint32_t delta_ms)
{
    uint8_t rpe;
    uint32_t load_increment;

    // Only count load if moving (cadence > 0)
    if (cadence < 20)  // Less than 20 steps/min = not really active
    {
        return;
    }

    // Get RPE based on cadence
    rpe = GetRPE(cadence);

    // Calculate load increment
    // Training Load = RPE * Duration(minutes)
    // load_x100 = RPE * (delta_ms / 60000) * 100
    // load_x100 = RPE * delta_ms / 600

    load_increment = ((uint32_t)rpe * delta_ms) / 600;
    total_load_x100 += load_increment;
}

// Get total training load
uint32_t TrainingLoadGetTotal(void)
{
    return total_load_x100 / 100;
}

// Reset training load
void TrainingLoadReset(void)
{
    total_load_x100 = 0;
}