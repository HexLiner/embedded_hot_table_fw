//  ***************************************************************************
/// @file    system_operation.h
/// @brief   System operation
//  ***************************************************************************
#ifndef _SYSTEM_OPERATION_H_
#define _SYSTEM_OPERATION_H_

#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>
#include "common/error.h"
#include "hal/gpio.h"
#include "hal/systimer.h"
#include "button_driver.h"
#include "indicators_driver.h"
#include "gui.h"
#include "profiles.h"
#include "outputs_driver.h"
#include "error_handling.h"


extern bool is_cli_dbg_mode;


extern void system_operation_init(void);
extern void system_operation_process(void);


#endif   // _SYSTEM_OPERATION_H_
