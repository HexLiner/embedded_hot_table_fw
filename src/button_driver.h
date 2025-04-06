//  ***************************************************************************
/// @file    button_driver.h
/// @brief   Button driver
//  ***************************************************************************
#ifndef _BUTTON_DRIVER_H_
#define _BUTTON_DRIVER_H_

#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>
#include "common/error.h"
#include "hal/gpio/gpio.h"
#include "hal/sysclk/sysclk.h"
#include "hal/systimer/systimer.h"


#define BUTTON_MAX_QTY (2)

#define BUTTON_DEBOUNCE_TIME_MS   (100)
#define BUTTON_LONG_PRESS_TIME_MS (1000)


typedef enum {
    BT_PROCESS_STATE_UP,
    BT_PROCESS_STATE_UP_DEBOUNCE,
    BT_PROCESS_STATE_DOWN,
    BT_PROCESS_STATE_LONG_DOWN,
    BT_PROCESS_STATE_DOWN_DEBOUNCE,
} button_process_state_t;

typedef struct {
    // Public variables
    gpio_pin_t button_pin;
    bool is_inverse_button;
    // Private variables
    button_process_state_t button_process_state;
    timer_t debounce_timer;
    timer_t long_press_timer;
    bool is_button_press_event;
    bool is_button_long_press_event;
} button_t;


extern void button_init(button_t *button);
extern void button_process(void);

extern bool button_is_pressed(button_t *button);
extern bool button_is_press_event(button_t *button);
extern bool button_is_long_press_event(button_t *button);
extern void button_clear_events_flags(button_t *button);


#endif   // _BUTTON_DRIVER_H_
