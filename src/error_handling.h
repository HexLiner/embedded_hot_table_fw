//  ***************************************************************************
/// @file    error_handling.h
/// @brief   Error handling
//  ***************************************************************************
#ifndef _ERROR_HANDLING_H_
#define _ERROR_HANDLING_H_

#include <stdint.h>
#include <stdbool.h>
#include "common/error.h"


#define FAIL_CODE_FW_ERROR                (1 << 0)
#define FAIL_CODE_CFG_ERROR               (1 << 1)
#define FAIL_CODE_EXT_OSCILLATOR_ERROR    (1 << 2)
#define FAIL_CODE_MCU_OVERTEMPERATURE     (1 << 3)
#define FAIL_CODE_LCD_ERROR               (1 << 4)

#define WARNING_CODE_ERR_WDT_RESET        (1 << 0)




extern uint16_t fail_code;
extern uint16_t warning_code;
extern error_t eh_fw_error_extended_code;



extern void error_handling_init(void);

extern void eh_set_fail_fw_error(error_t extended_code);
extern void eh_set_fail_cfg_error(void);
extern void eh_set_fail_ext_oscillator_error(void);
extern void eh_set_fail_mcu_overtemperature(void);
extern void eh_set_fail_lcd_error(void);

extern void eh_set_warn_err_wdt_reset(void);


#endif  // _ERROR_HANDLING_H_
