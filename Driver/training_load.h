/*
************************************************
* File: training_load.h
* Description: sRPE-based training load calculator
* Training Load = RPE * Duration(minutes)
************************************************
*/
#ifndef __TRAINING_LOAD_H
#define __TRAINING_LOAD_H

#include "stm32f4xx.h"

// RPE scale based on cadence
// Cadence < 80: RPE 2 (Very light)
// Cadence 80-100: RPE 3 (Light)
// Cadence 100-120: RPE 5 (Moderate)
// Cadence 120-140: RPE 7 (Hard)
// Cadence > 140: RPE 9 (Very hard)
#define RPE_VERY_LIGHT      2
#define RPE_LIGHT           3
#define RPE_MODERATE        5
#define RPE_HARD            7
#define RPE_VERY_HARD       9

// Cadence thresholds
#define CADENCE_LIGHT       80
#define CADENCE_MODERATE    100
#define CADENCE_HARD        120
#define CADENCE_VERY_HARD   140

// Function declarations
void TrainingLoadInit(void);
void TrainingLoadUpdate(uint16_t cadence, uint32_t delta_ms);
uint32_t TrainingLoadGetTotal(void);    // Returns training load units
void TrainingLoadReset(void);

#endif