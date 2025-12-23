/*
************************************************
* File: buzzer.h
* Description: Passive buzzer driver header
* Pin: PA8 (TIM1_CH1 PWM)
************************************************
*/
#ifndef __BUZZER_H
#define __BUZZER_H

#include "stm32f4xx.h"

// Buzzer port definitions
#define BUZZER_RCC      RCC_AHB1Periph_GPIOA
#define BUZZER_PORT     GPIOA
#define BUZZER_PIN      GPIO_Pin_8

// Default buzzer frequency (Hz)
#define BUZZER_FREQ     2000

// Musical note frequencies (Hz)
#define NOTE_C4         262     // Middle C
#define NOTE_D4         294
#define NOTE_E4         330
#define NOTE_F4         349
#define NOTE_G4         392
#define NOTE_A4         440
#define NOTE_B4         494
#define NOTE_C5         523
#define NOTE_D5         587
#define NOTE_E5         659
#define NOTE_F5         698
#define NOTE_G5         784

// Key click sound
#define KEY_CLICK_FREQ  1500
#define KEY_CLICK_MS    30

// Function declarations
void BuzzerInit(void);
void BuzzerOn(void);
void BuzzerOff(void);
void BuzzerBeep(uint16_t duration_ms);
void BuzzerSetFreq(uint16_t freq);
void BuzzerPlayTone(uint16_t freq, uint16_t duration_ms);
void BuzzerKeyClick(void);
void BuzzerGoalMelody(void);

#endif