//  ***************************************************************************
/// @file    systimer.c
//  ***************************************************************************
#include "hal/systimer/systimer.h"
#include <stdlib.h>
#include "hal/sysclk/sysclk.h"
#include "common/mcu.h"


static uint32_t          core_freq_hz;
static volatile uint64_t systime_ms = 0;
static systimer_callback callback = NULL;




//  ***************************************************************************
/// @brief  Initialisation of System Timer
/// @note   1ms period is set
/// @param  none
/// @return none
//  ***************************************************************************
void systimer_init(void) {
    uint32_t systick_freq;


    sysclk_get_bus_freq(SYSCLK_CORE, &core_freq_hz);

    sysclk_get_peripheral_freq(SysTick, &systick_freq);
    SysTick_Config(systick_freq / 1000);
}


//  ***************************************************************************
/// @brief  System timer interrupt handler
/// @param  none
/// @return none
//  ***************************************************************************
void systimer_handler(void) {
    systime_ms++;
    if (callback != NULL) callback(systime_ms);
}


//  ***************************************************************************
/// @brief  Set callback funtcion for systimer tick
/// @note   Callback will be called once per systimer tick (1 kHz)
/// @param  callback_function - callback function, can be NULL
//  ***************************************************************************
void systimer_set_callback(systimer_callback callback_function) {
    callback = callback_function;
}


//  ***************************************************************************
/// @brief  Provide ms-resolution delay
/// @note   Granularity 1 ms, accuracy 1 ms
/// @param  ms - delay time, ms
/// @return none
//  ***************************************************************************
void delay_ms(uint32_t ms) {
    uint64_t start_time;


    start_time = systime_ms;
    while ((systime_ms - start_time) < ms) {
        asm("nop");
    }
}


//  ***************************************************************************
/// @brief  Get system timer value [ms]
/// @param  none
/// @return system timer value [ms]
//  ***************************************************************************
uint64_t get_time_ms(void) {
    return systime_ms;
}


//  ***************************************************************************
/// @brief  Start timer with specified time value
/// @param  time_ms - time value in milliseconds
/// @return timer_t - timer handle
//  ***************************************************************************
timer_t timer_start_ms(uint32_t time_ms) {
    return (systime_ms + time_ms);
}


//  ***************************************************************************
/// @brief  Restart timer with specified time value
/// @param  timer - timer handle to be restarted
/// @param  time_ms - time value in milliseconds
/// @return timer_t - timer handle
/// @note   This function is useful to create periodical timer without time gaps
//  ***************************************************************************
timer_t timer_restart_ms(timer_t timer, uint32_t time_ms) {
    return (timer + time_ms);
}


//  ***************************************************************************
/// @brief  Check timer triggered or not
/// @param  timer - timer handle
/// @return triggered status
//  ***************************************************************************
bool timer_triggered(timer_t timer) {
    return (systime_ms >= timer);
}
