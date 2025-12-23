/*
************************************************
* File: speed_calc.h
* Description: Speed calculator based on cadence and stride
* Speed (km/h) = (Cadence / 60) * Stride * 3.6
************************************************
*/
#ifndef __SPEED_CALC_H
#define __SPEED_CALC_H

#include "stm32f4xx.h"

// Default user parameters
#define DEFAULT_HEIGHT_CM       170     // Default height in cm

// Stride length factors
#define WALKING_STRIDE_FACTOR   43      // height * 0.43 (stored as percentage)
#define RUNNING_STRIDE_FACTOR   45      // height * 0.45 (stored as percentage)
#define RUNNING_CADENCE_THRESHOLD 140   // Cadence above this = running

// Smoothing factor for exponential moving average (1/N)
#define SPEED_SMOOTHING_FACTOR  4

// Function declarations
void SpeedCalcInit(uint16_t height_cm);
void SpeedCalcUpdate(uint32_t step_count, uint32_t elapsed_ms);
uint16_t SpeedCalcGetSpeedKmh10(void);  // Returns speed * 10 (e.g., 52 = 5.2 km/h)
uint16_t SpeedCalcGetCadence(void);     // Returns steps per minute
uint32_t SpeedCalcGetDistanceM(uint32_t step_count);  // Returns distance in meters
void SpeedCalcReset(void);
void SpeedCalcSync(uint32_t step_count, uint32_t elapsed_ms);  // Sync state without spike

#endif