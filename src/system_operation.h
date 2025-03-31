//  ***************************************************************************
/// @file    system_operation.h
/// @brief   System operation
//  ***************************************************************************
#ifndef _SYSTEM_OPERATION_H_
#define _SYSTEM_OPERATION_H_

#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>
#include "common/error.h"
#include "hal/gpio/gpio.h"
#include "hal/systimer/systimer.h"
#include "int_flash_driver.h"
#include "button_driver.h"
#include "indicators_driver.h"
#include "gui.h"
#include "profiles.h"
#include "outputs_driver.h"


extern void system_operation_init(void);
extern void system_operation_process(void);


#endif   // _SYSTEM_OPERATION_H_
