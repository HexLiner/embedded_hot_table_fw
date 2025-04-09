//  ***************************************************************************
/// @file    mcu_clock.h
/// @brief   Function for setting MCU clock
//  ***************************************************************************
#ifndef _MCU_CLOCK_H_
#define _MCU_CLOCK_H_

#include "common/error.h"


extern void mcu_clock_hse_error_handler(void);

extern error_t mcu_clock_set_normal_config(void);
extern void mcu_clock_set_safe_config(void);



#endif  // _MCU_CLOCK_H_
