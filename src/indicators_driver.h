//  ***************************************************************************
/// @file    indicators_driver.h
/// @brief   Indicators driver
//  ***************************************************************************
#ifndef _INDICATORS_DRIVER_H_
#define _INDICATORS_DRIVER_H_

#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>
#include "hal/gpio.h"
#include "hal/sysclk.h"
#include "hal/systimer.h"


extern void indicators_init(void);
extern void indicators_process(void);

extern void indicators_led_process(bool en_dis);
extern void indicators_led_error(bool en_dis);

extern void indicators_buzzer_beep_terminate(void);
extern void indicators_buzzer_short_beep(void);
extern void indicators_buzzer_process_done_beep(void);
extern void indicators_buzzer_error_beep(void);


#endif  // _INDICATORS_DRIVER_H_
