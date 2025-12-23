/*
************************************************
* File: key_handler.h
* Description: Key input handler with multi-press
* and long-press detection
* Separates key detection from action execution
************************************************
*/
#ifndef __KEY_HANDLER_H
#define __KEY_HANDLER_H

#include "stm32f4xx.h"

// Key event types
typedef enum {
    KEY_EVENT_NONE = 0,
    KEY_EVENT_SINGLE,
    KEY_EVENT_DOUBLE,
    KEY_EVENT_TRIPLE,
    KEY_EVENT_LONG
} KeyEvent_t;

// Timing configuration
#define KEY_MULTI_PRESS_TIMEOUT_MS  600   // Time window for multi-press
#define KEY_LONG_PRESS_MS           1500  // Long press threshold

// Callback function types
typedef void (*KeyActionCallback)(void);

// Callback structure for each key
typedef struct {
    KeyActionCallback onSingle;
    KeyActionCallback onDouble;
    KeyActionCallback onTriple;
    KeyActionCallback onLong;
} KeyCallbacks_t;

// Function declarations
void KeyHandlerInit(void);
void KeyHandlerSetCallbacks(uint8_t key_num, KeyCallbacks_t *callbacks);
void KeyHandlerProcess(uint32_t elapsed_ms);

// Check if any key activity occurred (for low power wake detection)
uint8_t KeyHandlerHasActivity(void);

#endif