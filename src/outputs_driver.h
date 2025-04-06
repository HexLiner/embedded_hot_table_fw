//  ***************************************************************************
/// @file    outputs_driver.h
/// @brief   Outputs driver
//  ***************************************************************************
#ifndef _OUTPUTS_DRIVER_H_
#define _OUTPUTS_DRIVER_H_

#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>
#include "hal/gpio/gpio.h"
#include "hal/systimer/systimer.h"
#include "int_adc_driver.h"
#include "error_handling.h"


#define HEATER_MAX_TEMP_C              (200)


extern uint8_t heater_current_temperature_c;
extern bool is_heater_pin_en;

extern uint32_t heater_active_time_ms;
extern uint32_t heater_delay_time_ms;
extern uint8_t heater_hist_on_c;
extern uint8_t heater_hist_off_c;


extern void outputs_init(void);
extern void outputs_process(void);

extern void heater_en(uint8_t target_temperature_c);
extern void heater_dis(void);
extern void fun_en(uint32_t period_s, uint8_t duty_cycle_pct);
extern void fun_dis(void);


#endif  // _OUTPUTS_DRIVER_H_
