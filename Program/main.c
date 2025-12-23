/*
************************************************
* File: main.c
* Description: Pedometer main application
* Author: Li Zhe
************************************************
*/
#include "stm32f4xx.h"
#include "led.h"
#include "key.h"
#include "key_handler.h"
#include "NVIC.h"
#include "timer.h"
#include "uart.h"
#include "oled.h"
#include "mpu6050.h"
#include "buzzer.h"
#include "pedometer.h"
#include "speed_calc.h"
#include "calories.h"
#include "training_load.h"
#include "flash_data.h"
#include <stdio.h>

// Global flags
volatile uint8_t sample_flag = 0;     // Set by TIM3 interrupt
volatile uint32_t elapsed_ms = 0;     // System time in ms

// Pedometer state
static uint32_t target_steps = 10000;  // Default target
static uint8_t target_reached = 0;     // Flag for buzzer notification
static uint8_t language_cn = 1;        // Language: 1=Chinese (default), 0=English

// User settings (saved to flash)
static uint16_t user_height = 180;     // Height in cm
static uint16_t user_weight = 80;      // Weight in kg

// Stopwatch state
static uint8_t stopwatch_active = 0;   // 0=off, 1=running, 2=paused
static uint32_t stopwatch_ms = 0;      // Accumulated stopwatch time
static uint32_t stopwatch_start = 0;   // Start timestamp

// Pace display mode
static uint8_t pace_mode = 0;          // 0=km/h, 1=min/km

// Auto-save state
static uint32_t last_autosave_ms = 0;       // Last auto-save time
static uint32_t last_saved_steps = 0;       // Step count at last save
#define AUTOSAVE_INTERVAL_MS   300000       // Auto-save every 5 minutes
#define AUTOSAVE_STEP_DELTA    500          // Auto-save every 500 steps

// Sedentary reminder state
static uint32_t last_activity_ms = 0;       // Time of last detected step
static uint32_t last_activity_steps = 0;    // Step count at last activity check
static uint8_t sedentary_reminded = 0;      // Flag to prevent repeated reminders
#define SEDENTARY_TIMEOUT_MS   1800000      // 30 minutes = 1800000 ms

// Demo mode state
static uint8_t demo_mode = 0;               // 0=off, 1=walking, 2=running, 3=idle (cycle)
static uint32_t demo_last_step_ms = 0;      // Last simulated step time
static uint32_t demo_phase_start_ms = 0;    // When current phase started
#define DEMO_WALK_INTERVAL_MS   500         // 2 steps/sec = 120 cadence (walking)
#define DEMO_RUN_INTERVAL_MS    300         // 3.3 steps/sec = 200 cadence (running)
#define DEMO_PHASE_DURATION_MS  10000       // 10 seconds per phase
#define DEMO_SEDENTARY_MS       5000        // 5 seconds idle triggers sedentary in demo

// Animation state for progress bar
static uint8_t anim_frame = 0;              // Animation frame (0-7)
static uint32_t last_anim_ms = 0;           // Last animation frame change

// LED4 speed indicator state
static uint32_t last_led4_toggle_ms = 0;     // Last LED4 toggle time
static uint8_t led4_state = 0;               // Current LED4 state
static uint32_t last_step_count_for_led = 0; // Step count at last check
static uint32_t last_step_change_ms = 0;     // When step count last changed

/* 0-燕,1-山,2-大,3-学,4-李,5-哲,6-高,7-欣,8-宇,9-作,10-者,11-嵌,12-入,13-式,
 14-三,15-级,16-项,17-目,18-计,19-步,20-器,21-数,22-卡,23-路,24-里,25-训,
 26-练,27-负,28-荷,29-标,30-达,31-成,32-恭,33-喜,34-，,35-。,36-（,37-）,
 38-？,39-！,40-速,41-度,42-距,43-离,44-保,45-存,46-功,47-失,48-败,49-演,
 50-示,51-模,52-走,53-跑,54-停,55-动,56-该,57-活,58-无,59-分,60-钟 */
#define CN_YAN    0
#define CN_SHAN   1
#define CN_DA     2
#define CN_XUE    3
#define CN_LI     4
#define CN_ZHE    5
#define CN_GAO    6
#define CN_XIN    7
#define CN_YU     8
#define CN_QIAN   11
#define CN_RU     12
#define CN_SHI    13
#define CN_SAN    14
#define CN_JI     15
#define CN_XIANG  16
#define CN_MU     17
#define CN_BU     19
#define CN_SHU    21
#define CN_KA     22
#define CN_LU     23
#define CN_LI2    24
#define CN_FU     27
#define CN_HE     28
#define CN_DA2    30
#define CN_CHENG  31
#define CN_GONG   32
#define CN_XI     33
#define CN_COMMA  34
#define CN_EXCL   39
#define CN_SU     40
#define CN_DU     41
#define CN_JU     42  
#define CN_LI3    43  
#define CN_BAO    44  
#define CN_CUN    45  
#define CN_GONG2  46  // 功 
#define CN_SHI2   47  // 失
#define CN_BAI    48  
#define CN_YAN2   49  // 演
#define CN_SHI3   50  // 示
#define CN_MO     51  
#define CN_ZOU    52  
#define CN_PAO    53 
#define CN_TING   54 
#define CN_DONG   56  
#define CN_GAI    55  
#define CN_HUO    57  // 活
#define CN_WU     58  // 无
#define CN_FEN    59  // 分
#define CN_ZHONG  60  // 钟

void delayms(uint32_t msnum)
{
   uint16_t i;
   while(msnum--)
   {
     for(i = 0 ; i < 25*1000 ; i++);
   }
}

static void FormatSpeedOrPace(char *buf, uint16_t speed_x10)
{
    if(pace_mode)
    {
        // pace = 600 / speed
        if(speed_x10 > 0)
        {
            uint16_t pace_x10 = 6000 / speed_x10;  // min/km * 10
            uint16_t pace_min = pace_x10 / 10;
            uint16_t pace_sec = (pace_x10 % 10) * 6;  // 0.x min to seconds
            sprintf(buf, "%u:%02u/km", pace_min, pace_sec);
        }
        else
        {
            sprintf(buf, "--:--/km");
        }
    }
    else
    {
        sprintf(buf, "%u.%u km/h", speed_x10 / 10, speed_x10 % 10);
    }
}

static uint32_t GetStopwatchTime(void)
{
    if(stopwatch_active == 1)  // Running
        return stopwatch_ms + (elapsed_ms - stopwatch_start);
    else
        return stopwatch_ms;
}

static void SaveToFlash(void)
{
    FlashData_t data;

    OledClear();
    if(language_cn)
    {
        // Saving...
        const uint8_t baocun[] = {CN_BAO, CN_CUN};
        OledShowChineseString(32, 2, baocun, 2);
        OledShowString8x16(64, 2, "...");
    }
    else
    {
        OledShowString8x16(16, 2, "Saving...");
    }

    data.target_steps = target_steps;
    data.step_count = PedometerGetSteps();
    data.height_cm = user_height;
    data.weight_kg = user_weight;
    data.language_cn = language_cn;
    data.pace_mode = pace_mode;

    if(FlashDataSave(&data))
    {
        last_autosave_ms = elapsed_ms;
        last_saved_steps = data.step_count;

        OledClear();
        if(language_cn)
        {
            // Save Success!
            const uint8_t baocun_ok[] = {CN_BAO, CN_CUN, CN_CHENG, CN_GONG2, CN_EXCL};
            OledShowChineseString(24, 2, baocun_ok, 5);
        }
        else
        {
            OledShowString8x16(8, 2, "Save Success!");
        }

        // Success sound
        BuzzerPlayTone(NOTE_C5, 50);
        delayms(50);
        BuzzerPlayTone(NOTE_E5, 50);
        delayms(50);
        BuzzerPlayTone(NOTE_G5, 100);
    }
    else
    {
        // Error notification
        OledClear();
        if(language_cn)
        {
            // Save Failed!
            const uint8_t baocun_fail[] = {CN_BAO, CN_CUN, CN_SHI2, CN_BAI, CN_EXCL};
            OledShowChineseString(24, 2, baocun_fail, 5);
        }
        else
        {
            OledShowString8x16(8, 2, "Save Failed!");
        }

        // Error sound
        BuzzerPlayTone(NOTE_C4, 200);
    }

    delayms(1000); 
}

static void AutoSaveToFlash(void)
{
    FlashData_t data;

    data.target_steps = target_steps;
    data.step_count = PedometerGetSteps();
    data.height_cm = user_height;
    data.weight_kg = user_weight;
    data.language_cn = language_cn;
    data.pace_mode = pace_mode;

    if(FlashDataSave(&data))
    {
        last_autosave_ms = elapsed_ms;
        last_saved_steps = data.step_count;

        LedOn(LED4);
    }
}

static void LoadFromFlash(void)
{
    FlashData_t data;

    if(FlashDataLoad(&data))
    {
        target_steps = data.target_steps;
        user_height = data.height_cm;
        user_weight = data.weight_kg;
        language_cn = data.language_cn;
        pace_mode = data.pace_mode;

        // Restore step count
        PedometerClearSteps();
        PedometerAddSteps(data.step_count);

        SpeedCalcInit(user_height);
        CaloriesInit(user_weight);

        last_saved_steps = data.step_count;
    }
    // If no valid data, use defaults
}

static void UpdateDisplayEN(uint32_t steps, uint16_t speed_x10, uint32_t calories,
                            uint32_t training_load, uint8_t percent)
{
    char line[24];
    uint32_t distance_m;

    OledClear();

    sprintf(line, "%5lu/%5lu", steps, target_steps);
    OledShowString8x16(0, 0, line);

    if(demo_mode > 0)
    {
        const char *demo_phase[] = {"", "WALK", "RUN", "IDLE"};
        OledShowString6x8(98, 0, demo_phase[demo_mode]);
    }

    // Line 1: Speed or Pace (8x16 font, pages 2-3)
    FormatSpeedOrPace(line, speed_x10);
    OledShowString8x16(0, 2, line);

    // Line 2: Calories or Stopwatch (6x8 font, page 4)
    if(stopwatch_active)
    {
        uint32_t sw_time = GetStopwatchTime();
        uint16_t sw_min = (sw_time / 60000) % 100;
        uint16_t sw_sec = (sw_time / 1000) % 60;
        uint16_t sw_cs = (sw_time / 10) % 100;
        sprintf(line, "SW %02u:%02u.%02u%s", sw_min, sw_sec, sw_cs,
                stopwatch_active == 2 ? " P" : "");
    }
    else
    {
        sprintf(line, "Cal:%lu kcal", calories);
    }
    OledShowString6x8(0, 4, line);

    // Line 3: Distance and Load (6x8 font, page 5)
    distance_m = SpeedCalcGetDistanceM(steps);
    if(distance_m >= 1000)
    {
        sprintf(line, "%lu.%lukm L:%lu", distance_m / 1000, (distance_m % 1000) / 100, training_load);
    }
    else
    {
        sprintf(line, "%lum L:%lu", distance_m, training_load);
    }
    OledShowString6x8(0, 5, line);

    // Animated progress bar with walking figure (pages 6-7)
    {
        uint8_t activity_state;
        uint16_t cadence = SpeedCalcGetCadence();
        uint32_t anim_interval;

        // Determine activity state from cadence
        if(cadence < 30)
            activity_state = 0;  // Idle
        else if(cadence < 140)
            activity_state = 1;  // Walking
        else
            activity_state = 2;  // Running

        // Animation speed based on activity (faster = shorter interval)
        if(activity_state == 0)
            anim_interval = 500;      // Idle: slow flow
        else if(activity_state == 1)
            anim_interval = 200;      // Walking: medium flow
        else
            anim_interval = 100;      // Running: fast flow

        // Update animation frame (cycle 0-7)
        if(elapsed_ms - last_anim_ms >= anim_interval)
        {
            anim_frame = (anim_frame + 1) & 0x07;
            last_anim_ms = elapsed_ms;
        }

        OledDrawAnimatedProgress(percent, activity_state, anim_frame);
    }
}

// Update OLED display with pedometer data (Chinese)
// Layout: Steps xxxx/xxxx | Speed xx km/h | Calories/Load xx/xx |  | [progress]
static void UpdateDisplayCN(uint32_t steps, uint16_t speed_x10, uint32_t calories,
                            uint32_t training_load, uint8_t percent)
{
    char line[16];
    uint32_t distance_m;
    const uint8_t bushu[] = {CN_BU, CN_SHU};      // 步数
    const uint8_t sudu[] = {CN_SU, CN_DU};        // 速度
    const uint8_t kaluli[] = {CN_KA, CN_LU, CN_LI2};  // Calories
    const uint8_t fuhe[] = {CN_FU, CN_HE};        // 负荷

    OledClear();

    // Line 0: Steps xxxx/xxxx (16x16 + 8x16 for numbers)
    OledShowChineseString(0, 0, bushu, 2);
    sprintf(line, "%4lu/%4lu", steps, target_steps);
    OledShowString8x16(40, 0, line);

    // Demo mode indicator (top right, page 0) - Chinese
    if(demo_mode > 0)
    {
        const uint8_t zou[] = {CN_ZOU};   // 走
        const uint8_t pao[] = {CN_PAO};   // 跑
        const uint8_t ting[] = {CN_TING}; // 停
        switch(demo_mode)
        {
            case 1: OledShowChineseString(112, 0, zou, 1); break;
            case 2: OledShowChineseString(112, 0, pao, 1); break;
            case 3: OledShowChineseString(112, 0, ting, 1); break;
        }
    }

    // Line 1: Speed x.x km/h or pace
    OledShowChineseString(0, 2, sudu, 2);
    FormatSpeedOrPace(line, speed_x10);
    OledShowString8x16(40, 2, line);

    // Line 2: Calories/Load or Stopwatch (page 4)
    if(stopwatch_active)
    {
        uint32_t sw_time = GetStopwatchTime();
        uint16_t sw_min = (sw_time / 60000) % 100;
        uint16_t sw_sec = (sw_time / 1000) % 60;
        uint16_t sw_cs = (sw_time / 10) % 100;
        sprintf(line, "SW %02u:%02u.%02u%s", sw_min, sw_sec, sw_cs,
                stopwatch_active == 2 ? " P" : "");
        OledShowString6x8(0, 4, line);
    }
    else
    {
        OledShowChineseString(0, 4, kaluli, 3);
        OledShowChar6x8(48, 4, '/');
        OledShowChineseString(54, 4, fuhe, 2);
        sprintf(line, "%lu/%lu", calories, training_load);
        OledShowString6x8(86, 4, line);
    }

    // Line 3: Distance (page 5, 6x8 font only to avoid overlap)
    // Align with the numbers above (start at x=86)
    distance_m = SpeedCalcGetDistanceM(steps);
    if(distance_m >= 1000)
    {
        sprintf(line, "%lu.%lukm", distance_m / 1000, (distance_m % 1000) / 100);
    }
    else
    {
        sprintf(line, "%lum", distance_m);
    }
    OledShowString6x8(86, 5, line);

    {
        uint8_t activity_state;
        uint16_t cadence = SpeedCalcGetCadence();
        uint32_t anim_interval;
			
        if(cadence < 30)
            activity_state = 0; 
        else if(cadence < 140)
            activity_state = 1;
        else
            activity_state = 2;

        if(activity_state == 0)
            anim_interval = 500;    
        else if(activity_state == 1)
            anim_interval = 200;
        else
            anim_interval = 100;

        // Update animation frame (cycle 0-7)
        if(elapsed_ms - last_anim_ms >= anim_interval)
        {
            anim_frame = (anim_frame + 1) & 0x07;
            last_anim_ms = elapsed_ms;
        }

        OledDrawAnimatedProgress(percent, activity_state, anim_frame);
    }
}

// Update display based on current language
static void UpdateDisplay(uint32_t steps, uint16_t speed_x10, uint32_t calories,
                          uint32_t training_load, uint8_t percent)
{
    if(language_cn)
        UpdateDisplayCN(steps, speed_x10, calories, training_load, percent);
    else
        UpdateDisplayEN(steps, speed_x10, calories, training_load, percent);
}

static void Key1_OnDouble(void)
{
    language_cn = !language_cn;
    BuzzerPlayTone(NOTE_E4, 50);
    delayms(30);
    BuzzerPlayTone(NOTE_G4, 50);
}

static void Key1_OnTriple(void)
{
    uint32_t test_steps = (target_steps > 50) ? (target_steps - 50) : 0;
    PedometerClearSteps();
    PedometerAddSteps(test_steps);
    BuzzerPlayTone(NOTE_C5, 80);
    delayms(30);
    BuzzerPlayTone(NOTE_E5, 80);
}

static void Key1_OnLong(void)
{
    BuzzerPlayTone(NOTE_G4, 100);
    delayms(50);
    BuzzerPlayTone(NOTE_E4, 100);
    delayms(50);
    BuzzerPlayTone(NOTE_C4, 200);

    PedometerClearSteps();
    SpeedCalcReset();
    CaloriesReset();
    TrainingLoadReset();
    target_reached = 0;
}

static void Key2_OnSingle(void)
{
    if(target_steps < 50000)
        target_steps += 500;
    target_reached = 0;
}

static void Key2_OnDouble(void)
{
    if(stopwatch_active == 0)
    {
        stopwatch_active = 1;
        stopwatch_start = elapsed_ms;
        BuzzerPlayTone(NOTE_C5, 50);
    }
    else if(stopwatch_active == 1)
    {
        stopwatch_active = 2;
        stopwatch_ms += (elapsed_ms - stopwatch_start);
        BuzzerPlayTone(NOTE_E4, 50);
    }
    else
    {
        stopwatch_active = 1;
        stopwatch_start = elapsed_ms;
        BuzzerPlayTone(NOTE_C5, 50);
    }
}

static void Key2_OnTriple(void)
{
    stopwatch_active = 0;
    stopwatch_ms = 0;
    BuzzerPlayTone(NOTE_G4, 50);
    delayms(30);
    BuzzerPlayTone(NOTE_C4, 100);
}

static void Key2_OnLong(void)
{
    if(demo_mode == 0)
    {
        demo_mode = 1;
        demo_phase_start_ms = elapsed_ms;
        demo_last_step_ms = elapsed_ms;

        // Set steps close to target for goal demo (target - 15 steps)
        // Goal will be reached ~7.5 seconds into walking phase
        {
            uint32_t demo_start_steps = (target_steps > 15) ? (target_steps - 15) : 0;
            PedometerClearSteps();
            PedometerAddSteps(demo_start_steps);
            target_reached = 0;  // Reset so goal notification triggers
        }

        // Sync speed calculator to prevent spike
        SpeedCalcSync(PedometerGetSteps(), elapsed_ms);

        OledClear();
        if(language_cn)
        {
            const uint8_t yanshi[] = {CN_YAN2, CN_SHI3, CN_MO, CN_SHI};
            OledShowChineseString(32, 2, yanshi, 4);
        }
        else
        {
            OledShowString8x16(16, 2, "DEMO MODE");
        }
        OledShowString6x8(24, 5, "Long KEY2=Stop");
        BuzzerPlayTone(NOTE_C5, 100);
        delayms(50);
        BuzzerPlayTone(NOTE_E5, 100);
        delayms(50);
        BuzzerPlayTone(NOTE_G5, 100);
        delayms(1000);
    }
    else
    {
        demo_mode = 0;
        LedOff(LED1);

        OledClear();
        if(language_cn)
        {
            const uint8_t yanshi_stop[] = {CN_YAN2, CN_SHI3, CN_TING};
            OledShowChineseString(40, 2, yanshi_stop, 3);
        }
        else
        {
            OledShowString8x16(8, 2, "DEMO STOPPED");
        }
        BuzzerPlayTone(NOTE_G4, 100);
        delayms(50);
        BuzzerPlayTone(NOTE_E4, 100);
        delayms(50);
        BuzzerPlayTone(NOTE_C4, 100);
        delayms(1000);
    }
}

// KEY3 callbacks
static void Key3_OnSingle(void)
{
    // Decrease target by 500
    if(target_steps > 500)
        target_steps -= 500;
    // Note: Do NOT reset target_reached here
}

static void Key3_OnDouble(void)
{
    // Toggle pace mode
    pace_mode = !pace_mode;
    BuzzerPlayTone(NOTE_A4, 50);
    delayms(30);
    BuzzerPlayTone(NOTE_A4, 50);
}

static void Key3_OnLong(void)
{
    // Save to Flash
    SaveToFlash();
}

// Setup key callbacks
static void SetupKeyCallbacks(void)
{
    KeyCallbacks_t cb;

    // KEY1: double=language, triple=test steps, long=reset
    cb.onSingle = 0;  // No action on single press
    cb.onDouble = Key1_OnDouble;
    cb.onTriple = Key1_OnTriple;
    cb.onLong = Key1_OnLong;
    KeyHandlerSetCallbacks(1, &cb);

    // KEY2: single=+500, double=stopwatch, triple=reset SW, long=demo
    cb.onSingle = Key2_OnSingle;
    cb.onDouble = Key2_OnDouble;
    cb.onTriple = Key2_OnTriple;
    cb.onLong = Key2_OnLong;
    KeyHandlerSetCallbacks(2, &cb);

    // KEY3: single=-500, double=pace mode, long=save
    cb.onSingle = Key3_OnSingle;
    cb.onDouble = Key3_OnDouble;
    cb.onTriple = 0;  // No action on triple press
    cb.onLong = Key3_OnLong;
    KeyHandlerSetCallbacks(3, &cb);
}

static void UpdateDemoMode(void)
{
    uint32_t interval;

    if(demo_mode == 0)
        return;

    // if need to advance to next phase
    if(elapsed_ms - demo_phase_start_ms >= DEMO_PHASE_DURATION_MS)
    {
        demo_phase_start_ms = elapsed_ms;
        demo_mode++;
        if(demo_mode > 3)
            demo_mode = 1;  // Cycle: walk -> run -> idle -> walk...

        if(demo_mode == 1)
        {
            uint32_t demo_start_steps;
            sedentary_reminded = 0;

            demo_start_steps = (target_steps > 15) ? (target_steps - 15) : 0;
            PedometerClearSteps();
            PedometerAddSteps(demo_start_steps);
            target_reached = 0;

            SpeedCalcSync(PedometerGetSteps(), elapsed_ms);
        }

        BuzzerPlayTone(NOTE_A4, 50);
    }

 
    switch(demo_mode)
    {
        case 1:
            interval = DEMO_WALK_INTERVAL_MS;
            break;
        case 2:
            interval = DEMO_RUN_INTERVAL_MS;
            break;
        case 3:
        default:
            interval = 0;
            break;
    }

    if(interval > 0 && (elapsed_ms - demo_last_step_ms >= interval))
    {
        demo_last_step_ms = elapsed_ms;
        PedometerAddSteps(1);

        LedToggle(LED1);
    }
}

// Check for sedentary (no activity for 30 minutes, or 5 seconds in demo idle)
static void CheckSedentary(uint32_t steps)
{
    uint32_t timeout;
    uint8_t is_demo_idle;

    is_demo_idle = (demo_mode == 3); 
    timeout = is_demo_idle ? DEMO_SEDENTARY_MS : SEDENTARY_TIMEOUT_MS;

    if(steps > last_activity_steps)
    {
        last_activity_ms = elapsed_ms;
        last_activity_steps = steps;
        sedentary_reminded = 0;
        return;
    }

    if(!sedentary_reminded && (elapsed_ms - last_activity_ms >= timeout))
    {
        int i;
        const uint8_t gaidong[] = {CN_GAI, CN_DONG, CN_DONG, CN_EXCL};
        const uint8_t wuhuodong[] = {CN_FEN, CN_ZHONG, CN_WU, CN_HUO, CN_DONG};

        sedentary_reminded = 1;

        OledClear();
        if(language_cn)
        {
            // Time to move!
            OledShowChineseString(32, 2, gaidong, 4);
            if(is_demo_idle)
            {
                // Demo mode indicator
                OledShowString6x8(24, 5, "[DEMO]");
            }
            else
            {
                // 30 minutes inactive
                OledShowString6x8(0, 5, "30");
                OledShowChineseString(12, 5, wuhuodong, 5);
            }
        }
        else
        {
            OledShowString8x16(8, 2, "Move Around!");
            if(is_demo_idle)
            {
                OledShowString6x8(32, 5, "[DEMO MODE]");
            }
            else
            {
                OledShowString6x8(16, 5, "30 min inactive");
            }
        }

   
        BuzzerPlayTone(NOTE_C4, 100);
        delayms(100);
        BuzzerPlayTone(NOTE_E4, 100);
        delayms(100);
        BuzzerPlayTone(NOTE_G4, 100);
        delayms(100);
        BuzzerPlayTone(NOTE_C5, 200);

        for(i = 0; i < 3; i++)
        {
            LedOn(LED1); LedOn(LED2); LedOn(LED3); LedOn(LED4);
            delayms(200);
            LedOff(LED1); LedOff(LED2); LedOff(LED3); LedOff(LED4);
            delayms(200);
        }

        delayms(1000);
    }
}

// Check if target reached and trigger buzzer
static void CheckTarget(uint32_t steps)
{
    if(!target_reached && steps >= target_steps)
    {
        target_reached = 1;

        OledClear();
        if(language_cn)
        {
            // CongratsAchieved
            const uint8_t gongxi[] = {CN_GONG, CN_XI, CN_DA2, CN_CHENG, CN_EXCL};
            OledShowChineseString(24, 2, gongxi, 5);
        }
        else
        {
            OledShowString8x16(8, 2, "Goal Reached!");
        }

        BuzzerGoalMelody();
        delayms(1500);
    }
}

// Main program
int main(void)
{
    MPU6050_DataTypeDef mpu_data;
    uint32_t steps;
    uint16_t speed_x10;
    uint16_t cadence;
    uint32_t calories;
    uint32_t training_load;
    uint8_t percent;

    uint32_t last_update_ms = 0;
    uint32_t last_display_ms = 0;

    // Hardware initialization
    LedInit();
    KeyInit();
    Tim3Init(0);  // 50Hz sampling for pedometer
    UartInit();
    OledInit();
    MPU6050Init();
    BuzzerInit();

    PedometerInit();
    SpeedCalcInit(user_height);
    CaloriesInit(user_weight);
    TrainingLoadInit();
    KeyHandlerInit();
    SetupKeyCallbacks();

    FlashDataInit();
    LoadFromFlash();

    // Sync speed calculator with loaded step count to prevent spike
    SpeedCalcSync(PedometerGetSteps(), elapsed_ms);

    NvicCfg();

    LedOn(LED4);
    LedOff(LED2);

    // 
    OledClear();
    {
        const uint8_t line1[] = {CN_YAN, CN_SHAN, CN_DA, CN_XUE};  // 
        const uint8_t line2[] = {CN_QIAN, CN_RU, CN_SHI, CN_SAN, CN_JI, CN_XIANG, CN_MU};  // 
        const uint8_t line3[] = {CN_LI, CN_ZHE, CN_COMMA, CN_GAO, CN_XIN, CN_YU};  // 

        OledShowChineseString(32, 0, line1, 4);
        OledShowChineseString(8, 2, line2, 7);
        OledShowChineseString(16, 4, line3, 6);
    }
    delayms(2500);

    while(1)
    {
        // Sample accelerometer at 50Hz, TIM3
        if(sample_flag)
        {
            sample_flag = 0;
            elapsed_ms += 20;  // 20ms per sample

            // update, to fix the running
            UpdateDemoMode();
					
            MPU6050ReadData(&mpu_data);

            if(PedometerProcessSample(mpu_data.AccelDataX,
                                       mpu_data.AccelDataY,
                                       mpu_data.AccelDataZ))
            {
                LedToggle(LED2);
            }
        }

        // calc every 500ms
        if(elapsed_ms - last_update_ms >= 500)
        {
            last_update_ms = elapsed_ms;

            steps = PedometerGetSteps();

            SpeedCalcUpdate(steps, elapsed_ms);
            speed_x10 = SpeedCalcGetSpeedKmh10();
            cadence = SpeedCalcGetCadence();

            CaloriesUpdate(speed_x10, 500);
            calories = CaloriesGetTotal();

            TrainingLoadUpdate(cadence, 500);
            training_load = TrainingLoadGetTotal();

            {
                char uart_buf[64];
                sprintf(uart_buf, "Steps:%lu Spd:%u.%u Cad:%u Cal:%lu Load:%lu\r\n",
                        steps, speed_x10/10, speed_x10%10, cadence, calories, training_load);
                UartSendString(USART1, uart_buf);
            }

            CheckTarget(steps);
            CheckSedentary(steps);

            // LED4 flash in sync with cadence (only when actively walking)
            // Track step changes to detect true activity
            if(steps != last_step_count_for_led)
            {
                last_step_count_for_led = steps;
                last_step_change_ms = elapsed_ms;
            }

            // Only flash if steps changed in last 2 seconds AND cadence >= 30
            if(cadence >= 30 && (elapsed_ms - last_step_change_ms < 2000))
            {
                // Flash interval = 30000 / cadence (toggle twice per step)
                uint32_t flash_interval = 30000 / cadence;
                if(flash_interval < 100) flash_interval = 100;  // Min 100ms

                if(elapsed_ms - last_led4_toggle_ms >= flash_interval)
                {
                    last_led4_toggle_ms = elapsed_ms;
                    led4_state = !led4_state;
                    if(led4_state)
                        LedOn(LED4);
                    else
                        LedOff(LED4);
                }
            }
            else
            {
                // Idle - LED4 solid on as power indicator
                LedOn(LED4);
                led4_state = 1;
            }
        }

        // Update display every 200ms (~5Hz)
        if(elapsed_ms - last_display_ms >= 200)
        {
            last_display_ms = elapsed_ms;

            steps = PedometerGetSteps();
            speed_x10 = SpeedCalcGetSpeedKmh10();
            calories = CaloriesGetTotal();
            training_load = TrainingLoadGetTotal();

            // Calculate progress percentage
            if(target_steps > 0)
                percent = (steps * 100) / target_steps;
            else
                percent = 0;
            if(percent > 100) percent = 100;

            UpdateDisplay(steps, speed_x10, calories, training_load, percent);
        }

        // Auto-save check: every 5 minutes OR every 500 steps
        {
            uint32_t current_steps = PedometerGetSteps();
            uint8_t should_save = 0;

            if(elapsed_ms - last_autosave_ms >= AUTOSAVE_INTERVAL_MS)
            {
                should_save = 1;
            }
            else if(current_steps >= last_saved_steps + AUTOSAVE_STEP_DELTA)
            {
                should_save = 1;
            }

            if(should_save)
            {
                AutoSaveToFlash();
            }
        }

        // Handle key inputs
        KeyHandlerProcess(elapsed_ms);
    }
}