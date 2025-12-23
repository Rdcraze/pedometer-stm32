/*
************************************************
* File: calories.h
* Description: METs-based calorie calculator
* Formula: Calories = MET * Weight(kg) * Duration(hours)
************************************************
*/
#ifndef __CALORIES_H
#define __CALORIES_H

#include "stm32f4xx.h"

// Default user parameters
#define DEFAULT_WEIGHT_KG       70      // Default weight in kg

// MET values for different speeds (x10 to avoid float)
// Speed < 3.2 km/h: 2.0 METs
// Speed 3.2-4.8 km/h: 3.0 METs
// Speed 4.8-6.4 km/h: 4.0 METs
// Speed 6.4-8.0 km/h: 5.0 METs
// Speed > 8.0 km/h: 8.0 METs
#define MET_SLOW_WALK       20      // 2.0 METs * 10
#define MET_MODERATE_WALK   30      // 3.0 METs * 10
#define MET_BRISK_WALK      40      // 4.0 METs * 10
#define MET_FAST_WALK       50      // 5.0 METs * 10
#define MET_RUNNING         80      // 8.0 METs * 10

// Speed thresholds in km/h * 10
#define SPEED_SLOW_WALK     32      // 3.2 km/h
#define SPEED_MODERATE      48      // 4.8 km/h
#define SPEED_BRISK         64      // 6.4 km/h
#define SPEED_FAST          80      // 8.0 km/h

// Function declarations
void CaloriesInit(uint16_t weight_kg);
void CaloriesUpdate(uint16_t speed_kmh_x10, uint32_t delta_ms);
uint32_t CaloriesGetTotal(void);    // Returns kcal
void CaloriesReset(void);

#endif