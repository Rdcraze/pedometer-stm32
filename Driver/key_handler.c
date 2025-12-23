/*
************************************************
* File: key_handler.c
* Description: Key input handler with multi-press
* and long-press detection
* Clean state machine for each key
************************************************
*/
#include "key_handler.h"
#include "key.h"
#include "buzzer.h"

// Per-key state structure
typedef struct {
    uint8_t press_count;
    uint32_t first_press_time;
    uint32_t hold_start;
    uint8_t was_pressed;
    uint8_t long_handled;
} KeyState_t;

// State for each key (KEY1, KEY2, KEY3)
static KeyState_t key_state[3];

// Callbacks for each key
static KeyCallbacks_t key_callbacks[3];

// Activity flag for low power detection
static uint8_t activity_detected = 0;

// Current timestamp (updated each process call)
static uint32_t current_ms = 0;

// Forward declarations for internal functions
static void ProcessSingleKey(uint8_t key_idx, uint8_t is_pressed);
static void CheckMultiPressTimeout(uint8_t key_idx);
static void ExecuteCallback(uint8_t key_idx, KeyEvent_t event);

// Initialize key handler
void KeyHandlerInit(void)
{
    uint8_t i;
    for (i = 0; i < 3; i++)
    {
        key_state[i].press_count = 0;
        key_state[i].first_press_time = 0;
        key_state[i].hold_start = 0;
        key_state[i].was_pressed = 0;
        key_state[i].long_handled = 0;

        key_callbacks[i].onSingle = 0;
        key_callbacks[i].onDouble = 0;
        key_callbacks[i].onTriple = 0;
        key_callbacks[i].onLong = 0;
    }
    activity_detected = 0;
}

// Set callbacks for a specific key (key_num: 1, 2, or 3)
void KeyHandlerSetCallbacks(uint8_t key_num, KeyCallbacks_t *callbacks)
{
    if (key_num >= 1 && key_num <= 3 && callbacks != 0)
    {
        key_callbacks[key_num - 1] = *callbacks;
    }
}

// Process all keys - call this regularly from main loop
void KeyHandlerProcess(uint32_t elapsed_ms)
{
    Key_t key;
    uint8_t i;

    current_ms = elapsed_ms;
    key = GetKey();

    // Process each key
    ProcessSingleKey(0, (key == KEY1));
    ProcessSingleKey(1, (key == KEY2));
    ProcessSingleKey(2, (key == KEY3));

    // Check multi-press timeouts for all keys
    for (i = 0; i < 3; i++)
    {
        CheckMultiPressTimeout(i);
    }
}

// Check if any key activity occurred since last check
uint8_t KeyHandlerHasActivity(void)
{
    uint8_t result = activity_detected;
    activity_detected = 0;
    return result;
}

// Process a single key's state
static void ProcessSingleKey(uint8_t key_idx, uint8_t is_pressed)
{
    KeyState_t *ks = &key_state[key_idx];

    if (is_pressed)
    {
        activity_detected = 1;

        if (!ks->was_pressed)
        {
            // Key just pressed
            ks->was_pressed = 1;
            ks->hold_start = current_ms;
            ks->long_handled = 0;
        }
        else if (!ks->long_handled && (current_ms - ks->hold_start >= KEY_LONG_PRESS_MS))
        {
            // Long press detected
            ks->long_handled = 1;
            ks->press_count = 0;  // Cancel any multi-press
            ExecuteCallback(key_idx, KEY_EVENT_LONG);
        }
    }
    else
    {
        // Key released or not pressed
        if (ks->was_pressed && !ks->long_handled)
        {
            // Short press released - count it
            activity_detected = 1;

            if (ks->press_count == 0 || (current_ms - ks->first_press_time < KEY_MULTI_PRESS_TIMEOUT_MS))
            {
                if (ks->press_count == 0)
                    ks->first_press_time = current_ms;

                ks->press_count++;
                BuzzerKeyClick();
            }
            else
            {
                // Timeout - this is a new first press
                ks->press_count = 1;
                ks->first_press_time = current_ms;
                BuzzerKeyClick();
            }
        }
        ks->was_pressed = 0;
    }
}

// Check if multi-press timeout has elapsed and execute action
static void CheckMultiPressTimeout(uint8_t key_idx)
{
    KeyState_t *ks = &key_state[key_idx];

    if (ks->press_count > 0 && (current_ms - ks->first_press_time >= KEY_MULTI_PRESS_TIMEOUT_MS))
    {
        // Timeout reached - execute action based on press count
        if (ks->press_count == 1)
            ExecuteCallback(key_idx, KEY_EVENT_SINGLE);
        else if (ks->press_count == 2)
            ExecuteCallback(key_idx, KEY_EVENT_DOUBLE);
        else if (ks->press_count >= 3)
            ExecuteCallback(key_idx, KEY_EVENT_TRIPLE);

        ks->press_count = 0;
    }
}

// Execute the appropriate callback for a key event
static void ExecuteCallback(uint8_t key_idx, KeyEvent_t event)
{
    KeyCallbacks_t *cb = &key_callbacks[key_idx];
    KeyActionCallback fn = 0;

    switch (event)
    {
        case KEY_EVENT_SINGLE: fn = cb->onSingle; break;
        case KEY_EVENT_DOUBLE: fn = cb->onDouble; break;
        case KEY_EVENT_TRIPLE: fn = cb->onTriple; break;
        case KEY_EVENT_LONG:   fn = cb->onLong;   break;
        default: break;
    }

    if (fn != 0)
        fn();
}