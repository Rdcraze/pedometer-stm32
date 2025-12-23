/*
************************************************
* File: calories.c
* Description: METs-based calorie calculator
* Formula: Calories = MET * Weight(kg) * Duration(hours)
************************************************
*/
#include "calories.h"

// Internal state variables
static uint16_t user_weight_kg = DEFAULT_WEIGHT_KG;
static uint32_t total_calories_x1000 = 0;  // Calories * 1000 for precision

// Get MET value based on speed
static uint16_t GetMET(uint16_t speed_x10)
{
    if (speed_x10 < SPEED_SLOW_WALK)
        return MET_SLOW_WALK;
    else if (speed_x10 < SPEED_MODERATE)
        return MET_MODERATE_WALK;
    else if (speed_x10 < SPEED_BRISK)
        return MET_BRISK_WALK;
    else if (speed_x10 < SPEED_FAST)
        return MET_FAST_WALK;
    else
        return MET_RUNNING;
}

// Initialize calorie calculator
void CaloriesInit(uint16_t weight_kg)
{
    if (weight_kg < 30) weight_kg = 30;
    if (weight_kg > 200) weight_kg = 200;

    user_weight_kg = weight_kg;
    total_calories_x1000 = 0;
}

// Update calorie calculation
// Call this periodically with current speed and time delta
void CaloriesUpdate(uint16_t speed_kmh_x10, uint32_t delta_ms)
{
    uint16_t met_x10;
    uint32_t calories_increment;

    // Only count calories if moving
    if (speed_kmh_x10 < 10)  // Less than 1 km/h = not really moving
    {
        return;
    }

    // Get MET value based on speed
    met_x10 = GetMET(speed_kmh_x10);

    // Calculate calorie increment
    // Calories = MET * Weight(kg) * Duration(hours)
    // calories_x1000 = (MET/10) * weight * (delta_ms/3600000) * 1000
    // calories_x1000 = MET * weight * delta_ms / 36000

    calories_increment = ((uint32_t)met_x10 * user_weight_kg * delta_ms) / 36000;
    total_calories_x1000 += calories_increment;
}

// Get total calories burned
uint32_t CaloriesGetTotal(void)
{
    return total_calories_x1000 / 1000;
}

// Reset calorie counter
void CaloriesReset(void)
{
    total_calories_x1000 = 0;
}