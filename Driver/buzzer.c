/*
************************************************
* File: buzzer.c
* Description: Passive buzzer driver using TIM1_CH1 PWM
* Pin: PA8 (TIM1_CH1)
************************************************
*/
#include "buzzer.h"

// Delay function (external)
extern void delayms(uint32_t msnum);

// Initialize buzzer with TIM1_CH1 PWM
void BuzzerInit(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
    TIM_OCInitTypeDef TIM_OCInitStructure;

    // Enable clocks
    RCC_AHB1PeriphClockCmd(BUZZER_RCC, ENABLE);
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM1, ENABLE);

    // Configure PA8 as TIM1_CH1 alternate function
    GPIO_PinAFConfig(BUZZER_PORT, GPIO_PinSource8, GPIO_AF_TIM1);

    GPIO_InitStructure.GPIO_Pin = BUZZER_PIN;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
    GPIO_Init(BUZZER_PORT, &GPIO_InitStructure);

    // TIM1 configuration for PWM
    // APB2 = 168MHz, TIM1 clock = 168MHz
    // For 2kHz: Period = 168MHz / 2000 = 84000
    // Use prescaler of 84-1, period of 1000-1 for 2kHz
    TIM_TimeBaseStructure.TIM_Prescaler = 84 - 1;      // 168MHz / 84 = 2MHz
    TIM_TimeBaseStructure.TIM_Period = 1000 - 1;       // 2MHz / 1000 = 2kHz
    TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
    TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;
    TIM_TimeBaseStructure.TIM_RepetitionCounter = 0;
    TIM_TimeBaseInit(TIM1, &TIM_TimeBaseStructure);

    // PWM Mode configuration: Channel 1
    TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;
    TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
    TIM_OCInitStructure.TIM_OutputNState = TIM_OutputNState_Disable;
    TIM_OCInitStructure.TIM_Pulse = 0;  // Start with 0% duty (off)
    TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;
    TIM_OCInitStructure.TIM_OCNPolarity = TIM_OCNPolarity_High;
    TIM_OCInitStructure.TIM_OCIdleState = TIM_OCIdleState_Reset;
    TIM_OCInitStructure.TIM_OCNIdleState = TIM_OCNIdleState_Reset;
    TIM_OC1Init(TIM1, &TIM_OCInitStructure);

    TIM_OC1PreloadConfig(TIM1, TIM_OCPreload_Enable);
    TIM_ARRPreloadConfig(TIM1, ENABLE);

    // Enable TIM1 main output (required for advanced timers)
    TIM_CtrlPWMOutputs(TIM1, ENABLE);

    // Enable TIM1
    TIM_Cmd(TIM1, ENABLE);
}

// Turn buzzer on (50% duty cycle)
void BuzzerOn(void)
{
    TIM_SetCompare1(TIM1, 500);  // 50% duty cycle
}

// Turn buzzer off
void BuzzerOff(void)
{
    TIM_SetCompare1(TIM1, 0);  // 0% duty cycle
}

// Beep for specified duration in milliseconds
void BuzzerBeep(uint16_t duration_ms)
{
    BuzzerOn();
    delayms(duration_ms);
    BuzzerOff();
}

// Set buzzer frequency
void BuzzerSetFreq(uint16_t freq)
{
    uint32_t period;
    if (freq < 100) freq = 100;
    if (freq > 10000) freq = 10000;

    // TIM1 clock = 2MHz after prescaler
    period = 2000000 / freq;
    TIM_SetAutoreload(TIM1, period - 1);
    TIM_SetCompare1(TIM1, period / 2);  // 50% duty
}

// Play a tone at specified frequency for duration
void BuzzerPlayTone(uint16_t freq, uint16_t duration_ms)
{
    BuzzerSetFreq(freq);
    BuzzerOn();
    delayms(duration_ms);
    BuzzerOff();
}

// Short click sound for key press feedback
void BuzzerKeyClick(void)
{
    BuzzerPlayTone(KEY_CLICK_FREQ, KEY_CLICK_MS);
}

// Goal celebration melody: C4-E4-E5
void BuzzerGoalMelody(void)
{
    BuzzerPlayTone(NOTE_C4, 400);   // C4 - 150ms
    delayms(50);
    BuzzerPlayTone(NOTE_E4, 400);   // E4 - 150ms
    delayms(50);
    BuzzerPlayTone(NOTE_G4, 400);
		delayms(50);
		BuzzerPlayTone(NOTE_C5, 800);
		delayms(50);
}