/*
************************************************
* File: pedometer.c
* Description: Pedometer step detection algorithm
* Uses peak detection with adaptive threshold
* and step confirmation state machine
* Sampling rate: 50Hz (20ms per sample)
************************************************
*/
#include "pedometer.h"
#include "mpu6050.h"
#include <stdlib.h>

// State machine states
typedef enum {
    STATE_IDLE,         // Waiting for first step pattern
    STATE_CONFIRMING,   // Detected potential steps, confirming walking
    STATE_WALKING       // Confirmed walking, counting steps
} PedometerState_t;

// Internal state variables
static uint32_t step_count = 0;
static uint32_t sample_count = 0;
static uint32_t last_step_sample = 0;

// Filter state
static int32_t filtered_prev = 0;
static int32_t raw_prev = 0;

// Peak detection state
static int32_t threshold = INITIAL_THRESHOLD;
static int32_t peak_value = 0;
static int32_t valley_value = 0;
static uint8_t looking_for_peak = 1;

// Step confirmation state machine
static PedometerState_t state = STATE_IDLE;
static uint8_t confirm_count = 0;           // Steps detected during confirmation
static uint32_t pending_steps = 0;          // Steps to add once confirmed
static uint32_t last_confirm_interval = 0;  // Interval of last confirmed step (for regularity check)

// Fast integer square root (Newton's method)
static uint32_t isqrt(uint32_t n)
{
    uint32_t x = n;
    uint32_t y = (x + 1) >> 1;

    if (n == 0) return 0;

    while (y < x)
    {
        x = y;
        y = (x + n / x) >> 1;
    }
    return x;
}

// Calculate vector magnitude from accelerometer data
static int32_t CalcMagnitude(int16_t ax, int16_t ay, int16_t az)
{
    // Calculate actual magnitude using integer sqrt
    int32_t x = ax;
    int32_t y = ay;
    int32_t z = az;
    uint32_t sum = (uint32_t)(x*x) + (uint32_t)(y*y) + (uint32_t)(z*z);
    return (int32_t)isqrt(sum);  // Returns ~16384 for 1g
}

// High-pass filter to remove DC component (gravity)
// y[n] = alpha * (y[n-1] + x[n] - x[n-1])
// Using alpha = 0.85 approximated as 218/256 (was 230/256 = 0.9)
static int32_t HighPassFilter(int32_t raw)
{
    int32_t filtered;
    filtered = (218 * (filtered_prev + raw - raw_prev)) >> 8;
    raw_prev = raw;
    filtered_prev = filtered;
    return filtered;
}

// Update adaptive threshold
static void UpdateThreshold(int32_t peak, int32_t valley)
{
    int32_t amplitude = peak - valley;
    int32_t new_threshold;

    // New threshold is 40% of the amplitude
    new_threshold = (amplitude * 40) / 100;

    // Exponential smoothing
    threshold = threshold + (new_threshold - threshold) / THRESHOLD_UPDATE_RATE;

    // Clamp threshold to reasonable range
    if (threshold < MIN_THRESHOLD) threshold = MIN_THRESHOLD;
    if (threshold > MAX_THRESHOLD) threshold = MAX_THRESHOLD;
}

// Check if two intervals are similar (within 40% of each other)
// Used to verify walking regularity
static uint8_t IntervalsAreSimilar(uint32_t interval1, uint32_t interval2)
{
    uint32_t diff;
    uint32_t threshold_diff;

    if (interval1 == 0 || interval2 == 0) return 1;  // First step, accept

    if (interval1 > interval2)
        diff = interval1 - interval2;
    else
        diff = interval2 - interval1;

    // Allow 40% variation
    threshold_diff = (interval1 > interval2 ? interval1 : interval2) * 40 / 100;

    return (diff <= threshold_diff);
}

// Initialize pedometer
void PedometerInit(void)
{
    step_count = 0;
    sample_count = 0;
    last_step_sample = 0;

    filtered_prev = 0;
    raw_prev = 0;

    threshold = INITIAL_THRESHOLD;
    peak_value = 0;
    valley_value = 0;
    looking_for_peak = 1;

    // State machine init
    state = STATE_IDLE;
    confirm_count = 0;
    pending_steps = 0;
    last_confirm_interval = 0;
}

// Process one accelerometer sample
// Returns 1 if a step was detected, 0 otherwise
uint8_t PedometerProcessSample(int16_t ax, int16_t ay, int16_t az)
{
    int32_t magnitude;
    int32_t filtered;
    uint8_t step_detected = 0;
    uint8_t raw_step = 0;  // Raw step before state machine
    uint32_t interval = 0;
    int32_t peak_valley_diff;

    sample_count++;

    // Calculate magnitude of acceleration vector
    magnitude = CalcMagnitude(ax, ay, az);

    // Apply high-pass filter
    filtered = HighPassFilter(magnitude);

    // Peak detection with hysteresis
    if (looking_for_peak)
    {
        if (filtered > peak_value)
        {
            peak_value = filtered;
        }
        else if (filtered < peak_value - threshold)
        {
            // Peak detected, now look for valley
            looking_for_peak = 0;
            valley_value = filtered;

            // Check timing constraints
            interval = sample_count - last_step_sample;

            // Check minimum peak-valley difference (noise rejection)
            peak_valley_diff = peak_value - valley_value;

            if (interval >= MIN_STEP_INTERVAL && interval <= MAX_STEP_INTERVAL
                && peak_valley_diff >= MIN_PEAK_VALLEY_DIFF)
            {
                // Valid step pattern detected
                raw_step = 1;
                last_step_sample = sample_count;

                // Update adaptive threshold
                UpdateThreshold(peak_value, valley_value);
            }
            else if (interval > MAX_STEP_INTERVAL)
            {
                // Reset timing after long pause
                last_step_sample = sample_count;

                // If we were walking and paused too long, go back to IDLE
                if (state == STATE_WALKING || state == STATE_CONFIRMING)
                {
                    state = STATE_IDLE;
                    confirm_count = 0;
                    pending_steps = 0;
                    last_confirm_interval = 0;
                }
            }
        }
    }
    else  // Looking for valley
    {
        if (filtered < valley_value)
        {
            valley_value = filtered;
        }
        else if (filtered > valley_value + threshold)
        {
            // Valley detected, now look for peak
            looking_for_peak = 1;
            peak_value = filtered;
        }
    }

    // State machine for step confirmation
    // Note: 'interval' was calculated above before last_step_sample was updated
    if (raw_step)
    {
        switch (state)
        {
            case STATE_IDLE:
                // First potential step - start confirming
                state = STATE_CONFIRMING;
                confirm_count = 1;
                pending_steps = 1;
                last_confirm_interval = 0;  // No previous interval yet
                break;

            case STATE_CONFIRMING:
                // Check if this step has similar timing to previous
                if (IntervalsAreSimilar(interval, last_confirm_interval))
                {
                    confirm_count++;
                    pending_steps++;
                    last_confirm_interval = interval;

                    if (confirm_count >= STEPS_TO_CONFIRM)
                    {
                        // Confirmed walking! Add all pending steps
                        state = STATE_WALKING;
                        step_count += pending_steps;
                        step_detected = 1;
                        pending_steps = 0;
                    }
                }
                else
                {
                    // Irregular timing - might be noise, reset
                    // But keep this as the first step of a new sequence
                    confirm_count = 1;
                    pending_steps = 1;
                    last_confirm_interval = 0;
                }
                break;

            case STATE_WALKING:
                // Already confirmed walking, count the step
                step_count++;
                step_detected = 1;
                last_confirm_interval = interval;
                break;
        }
    }

    // Check for walking timeout (no steps for 3 seconds)
    if (state != STATE_IDLE)
    {
        if (sample_count - last_step_sample > WALKING_TIMEOUT_SAMPLES)
        {
            state = STATE_IDLE;
            confirm_count = 0;
            pending_steps = 0;
            last_confirm_interval = 0;
        }
    }

    return step_detected;
}

// Get current step count
uint32_t PedometerGetSteps(void)
{
    return step_count;
}

// Clear step count
void PedometerClearSteps(void)
{
    step_count = 0;
    last_step_sample = sample_count;

    // Reset state machine
    state = STATE_IDLE;
    confirm_count = 0;
    pending_steps = 0;
    last_confirm_interval = 0;
}

// Add steps (for testing/demo mode)
void PedometerAddSteps(uint32_t count)
{
    step_count += count;
}

// Get time of last step (in sample counts)
uint32_t PedometerGetLastStepTime(void)
{
    return last_step_sample;
}