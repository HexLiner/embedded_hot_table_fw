//  ***************************************************************************
/// @file    button_driver.c
//  ***************************************************************************
#include "button_driver.h"


void button_init(button_t *button, gpio_pin_t button_pin, bool is_inverse_button) {
    gpio_config_pins(button_pin, GPIO_MODE_INPUT, GPIO_PULL_NONE, GPIO_SPEED_LOW, 0, false);

    button->button_pin = button_pin;
    button->is_inverse_button = is_inverse_button;
    button->button_process_state = BT_PROCESS_STATE_UP;
    button->is_button_press_event = false;
    button->is_button_long_press_event = false;
}


void button_process(button_t *button) {
    bool is_button_down = button_is_pressed(button);

    switch (button->button_process_state) {
        case BT_PROCESS_STATE_UP:
            if (is_button_down) {
                button->button_process_state = BT_PROCESS_STATE_UP_DEBOUNCE;
                button->debounce_timer = timer_start_ms(BUTTON_DEBOUNCE_TIME_MS);
            }
            break;

        case BT_PROCESS_STATE_UP_DEBOUNCE:
            if (timer_triggered(button->debounce_timer)) {
                if (is_button_down) {
                    button->button_process_state = BT_PROCESS_STATE_DOWN;
                    button->long_press_timer = timer_start_ms(BUTTON_LONG_PRESS_TIME_MS);
                }
                else {
                    button->button_process_state = BT_PROCESS_STATE_UP;
                }
            }
            break;

        case BT_PROCESS_STATE_DOWN:
            if (!is_button_down) {
                button->button_process_state = BT_PROCESS_STATE_DOWN_DEBOUNCE;
                button->debounce_timer = timer_start_ms(BUTTON_DEBOUNCE_TIME_MS);
            }
            else if (timer_triggered(button->long_press_timer)) {
                button->button_process_state = BT_PROCESS_STATE_LONG_DOWN;
                button->is_button_long_press_event = true;
            }
            break;

        case BT_PROCESS_STATE_LONG_DOWN:
            if (!is_button_down) {
                button->button_process_state = BT_PROCESS_STATE_DOWN_DEBOUNCE;
                button->debounce_timer = timer_start_ms(BUTTON_DEBOUNCE_TIME_MS);
            }
            break;

        case BT_PROCESS_STATE_DOWN_DEBOUNCE:
            if (timer_triggered(button->debounce_timer)) {
                if (!is_button_down) {
                    if (!timer_triggered(button->long_press_timer)) button->is_button_press_event = true;
                    button->button_process_state = BT_PROCESS_STATE_UP;
                }
                else {
                    if (!timer_triggered(button->long_press_timer)) button->button_process_state = BT_PROCESS_STATE_DOWN;
                    else button->button_process_state = BT_PROCESS_STATE_LONG_DOWN;
                }
            }
            break; 
    }
}


bool button_is_pressed(button_t *button) {
    if (gpio_read_pins(button->button_pin) == button->button_pin) return !button->is_inverse_button;
    return button->is_inverse_button;
}


bool button_is_press_event(button_t *button) {
    bool is_event = button->is_button_press_event;
    button->is_button_press_event = false;
    return is_event;
}


bool button_is_long_press_event(button_t *button) {
    bool is_event = button->is_button_long_press_event;
    button->is_button_long_press_event = false;
    return is_event;
}


void button_clear_events_flags(button_t *button) {
    button->is_button_press_event = false;
    button->is_button_long_press_event = false;
}
