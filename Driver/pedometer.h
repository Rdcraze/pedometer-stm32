/*
************************************************
* File: pedometer.h
* Description: Pedometer step detection algorithm
* Uses peak detection with adaptive threshold
* and step confirmation state machine
************************************************
*/
#ifndef __PEDOMETER_H
#define __PEDOMETER_H

#include "stm32f4xx.h"

// Sampling rate configuration
#define PEDOMETER_SAMPLE_RATE   50      // 50 Hz
#define PEDOMETER_SAMPLE_MS     20      // 20 ms per sample

// Step timing constraints (in samples)
#define MIN_STEP_INTERVAL       (250 / PEDOMETER_SAMPLE_MS)   // 250ms minimum (12 samples)
#define MAX_STEP_INTERVAL       (2000 / PEDOMETER_SAMPLE_MS)  // 2000ms maximum (100 samples)

// Threshold configuration (tuned for better noise rejection)
#define INITIAL_THRESHOLD       2500    // Initial threshold value
#define MIN_THRESHOLD           1500    // Minimum threshold (was 1000)
#define MAX_THRESHOLD           8000    // Maximum threshold
#define THRESHOLD_UPDATE_RATE   8       // 1/8 exponential smoothing

// Noise rejection
#define MIN_PEAK_VALLEY_DIFF    2500    // Minimum peak-valley difference (was 1500)

// Step confirmation (state machine)
#define STEPS_TO_CONFIRM        4       // Need 4 consecutive valid steps to confirm walking
#define WALKING_TIMEOUT_SAMPLES (3000 / PEDOMETER_SAMPLE_MS)  // 3 seconds without step = back to IDLE

// Function declarations
void PedometerInit(void);
uint8_t PedometerProcessSample(int16_t ax, int16_t ay, int16_t az);
uint32_t PedometerGetSteps(void);
void PedometerClearSteps(void);
void PedometerAddSteps(uint32_t count);
uint32_t PedometerGetLastStepTime(void);

#endif