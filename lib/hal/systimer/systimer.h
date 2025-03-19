//  ***************************************************************************
/// @file    systimer.h
/// @brief   1 kHz SysClk timer
//  ***************************************************************************
#ifndef _SYSTIMER_H_
#define _SYSTIMER_H_

#include <stdint.h>
#include <stdbool.h>


typedef uint64_t timer_t;
typedef void (*systimer_callback)(uint64_t systime_ms);


extern void systimer_init(void);
extern void systimer_set_callback(systimer_callback callback_function);

extern uint64_t get_time_ms(void);

extern void delay_ms(uint32_t ms);

extern timer_t timer_start_ms(uint32_t time_ms);
extern timer_t timer_restart_ms(timer_t timer, uint32_t time_ms);
extern bool timer_triggered(timer_t timer);


#endif  // _SYSTIMER_H_
