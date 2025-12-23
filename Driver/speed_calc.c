/*
************************************************
* File: speed_calc.c
* Description: Speed calculator based on cadence and stride
* Speed (km/h) = (Cadence / 60) * Stride * 3.6
************************************************
*/
#include "speed_calc.h"

// Internal state variables
static uint16_t user_height = DEFAULT_HEIGHT_CM;
static uint16_t walking_stride_cm;  // Stride length in cm
static uint16_t running_stride_cm;

static uint32_t prev_step_count = 0;
static uint32_t prev_elapsed_ms = 0;

static uint16_t current_cadence = 0;      // Steps per minute
static uint16_t current_speed_x10 = 0;    // Speed * 10 in km/h

// Initialize speed calculator
void SpeedCalcInit(uint16_t height_cm)
{
    if (height_cm < 100) height_cm = 100;
    if (height_cm > 250) height_cm = 250;

    user_height = height_cm;

    // Calculate stride lengths
    walking_stride_cm = (user_height * WALKING_STRIDE_FACTOR) / 100;
    running_stride_cm = (user_height * RUNNING_STRIDE_FACTOR) / 100;

    prev_step_count = 0;
    prev_elapsed_ms = 0;
    current_cadence = 0;
    current_speed_x10 = 0;
}

// Update speed calculation
// Call this periodically (e.g., every second)
void SpeedCalcUpdate(uint32_t step_count, uint32_t elapsed_ms)
{
    uint32_t delta_steps;
    uint32_t delta_ms;
    uint32_t cadence_raw;
    uint16_t stride_cm;
    uint32_t speed_raw;

    // Calculate deltas
    if (elapsed_ms <= prev_elapsed_ms)
    {
        return;  // No time passed or timer overflow
    }

    delta_ms = elapsed_ms - prev_elapsed_ms;
    delta_steps = step_count - prev_step_count;

    // Need at least 500ms for meaningful calculation
    if (delta_ms < 500)
    {
        return;
    }

    // Update previous values
    prev_step_count = step_count;
    prev_elapsed_ms = elapsed_ms;

    // Calculate cadence (steps per minute)
    // cadence = (delta_steps / delta_ms) * 60000
    if (delta_steps == 0)
    {
        // Exponential decay when not walking
        current_cadence = (current_cadence * 3) / 4;
        current_speed_x10 = (current_speed_x10 * 3) / 4;
        return;
    }

    cadence_raw = (delta_steps * 60000) / delta_ms;

    // Exponential moving average smoothing
    current_cadence = current_cadence + (cadence_raw - current_cadence) / SPEED_SMOOTHING_FACTOR;

    // Select stride based on cadence
    if (current_cadence > RUNNING_CADENCE_THRESHOLD)
    {
        stride_cm = running_stride_cm;
    }
    else
    {
        stride_cm = walking_stride_cm;
    }

    // Calculate speed in km/h * 10
    // speed (km/h) = (cadence / 60) * stride_m * 3.6
    // speed (km/h) = cadence * stride_cm / 100 * 3.6 / 60
    // speed (km/h) = cadence * stride_cm * 0.0006
    // speed * 10 = cadence * stride_cm * 0.006 = cadence * stride_cm / 166.67
    // To avoid float: speed * 10 = cadence * stride_cm * 6 / 1000
    speed_raw = ((uint32_t)current_cadence * stride_cm * 6) / 1000;

    // Exponential moving average smoothing
    current_speed_x10 = current_speed_x10 + (speed_raw - current_speed_x10) / SPEED_SMOOTHING_FACTOR;

    // Clamp to reasonable range
    if (current_speed_x10 > 200) current_speed_x10 = 200;  // Max 20 km/h
}

// Get current speed (multiplied by 10)
uint16_t SpeedCalcGetSpeedKmh10(void)
{
    return current_speed_x10;
}

// Get current cadence (steps per minute)
uint16_t SpeedCalcGetCadence(void)
{
    return current_cadence;
}

// Get distance in meters based on step count
// Uses walking stride as default (most common activity)
uint32_t SpeedCalcGetDistanceM(uint32_t step_count)
{
    // distance_m = step_count * stride_cm / 100
    return (step_count * walking_stride_cm) / 100;
}

// Reset speed calculator
void SpeedCalcReset(void)
{
    prev_step_count = 0;
    prev_elapsed_ms = 0;
    current_cadence = 0;
    current_speed_x10 = 0;
}

// Sync speed calculator with current state (prevents spike on start/resume)
// Call this after loading steps from flash or starting demo mode
void SpeedCalcSync(uint32_t step_count, uint32_t elapsed_ms)
{
    prev_step_count = step_count;
    prev_elapsed_ms = elapsed_ms;
    // Don't reset cadence/speed - let them decay naturally if no activity
}