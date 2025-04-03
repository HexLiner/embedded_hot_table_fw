//  ***************************************************************************
/// @file    error_handling.c
//  ***************************************************************************
#include "error_handling.h"

uint16_t fail_code;
uint16_t warning_code;
error_t eh_fw_error_extended_code;


//  ***************************************************************************
/// @brief      Error handling module init
/// @param      none
/// @return     none
//  ***************************************************************************
void error_handling_init(void) {
    fail_code = 0;
    warning_code = 0;
    eh_fw_error_extended_code = E_OK;
}


void eh_set_fail_fw_error(error_t extended_code) {
    if (fail_code & FAIL_CODE_FW_ERROR) return;

    fail_code |= FAIL_CODE_FW_ERROR;
    if (ERROR_GET_SOFTWARE_FLAG(extended_code)) eh_set_fail_fw_error(extended_code);
    if (eh_fw_error_extended_code == E_OK) eh_fw_error_extended_code = extended_code;
}

void eh_set_fail_cfg_error(void) {
    fail_code |= FAIL_CODE_CFG_ERROR;
}

void eh_set_fail_ext_oscillator_error(void) {
    fail_code |= FAIL_CODE_EXT_OSCILLATOR_ERROR;
}

void eh_set_fail_lcd_error(void) {
    fail_code |= FAIL_CODE_LCD_ERROR;
}


void eh_set_warn_err_wdt_reset(void) {
    warning_code |= WARNING_CODE_ERR_WDT_RESET;
    eh_write_log_rec();
}
