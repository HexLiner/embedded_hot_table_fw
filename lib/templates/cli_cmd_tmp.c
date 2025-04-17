//  ***************************************************************************
/// @file    cli_cmd.c
//  ***************************************************************************
#include "cli_cmd.h"
#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>
#include "usb_cdc.h"
#include "common/cli.h"
#include "common/parsers.h"
#include "common/error.h"
#include "hal/systimer.h"

#include "registers.h"
#include "outputs_driver.h"


static error_t cli_cmd_reboot(uint32_t argc, const uint8_t **argv, cli_call_state_t state);
static error_t cli_cmd_rr(uint32_t argc, const uint8_t **argv, cli_call_state_t state);
static error_t cli_cmd_wr(uint32_t argc, const uint8_t **argv, cli_call_state_t state);
static error_t cli_cmd_clidbg(uint32_t argc, const uint8_t **argv, cli_call_state_t state);
static error_t cli_cmd_tlog(uint32_t argc, const uint8_t **argv, cli_call_state_t state);
static error_t cli_cmd_tconf(uint32_t argc, const uint8_t **argv, cli_call_state_t state);

static bool pars_string_to_s32_and_check(const uint8_t *str, int32_t *digit, int32_t min, int32_t max);
static bool pars_string_to_u32_and_check(const uint8_t *str, uint32_t *digit, uint32_t min, uint32_t max);


const cli_cmd_t cli_cmds[] = {
    {
        .name = "reboot",
        .usage = "",
        .func = cli_cmd_reboot
    },
    {
        .name = "rr",
        .usage = "ADDR",
        .func = cli_cmd_rr
    },
    {
        .name = "wr",
        .usage = "ADDR VAL",
        .func = cli_cmd_wr
    },
    {
        .name = "clidbg",
        .usage = "en",
        .func = cli_cmd_clidbg
    },
    {
        .name = "tlog",
        .usage = "PERIOD_MS",
        .func = cli_cmd_tlog
    },
    {
        .name = "tconf",
        .usage = "[ACT_MS DELAY_MS HYST_ON_C HYST_OFF_C]",
        .func = cli_cmd_tconf
    }
};




void cli_cmd_init(void) {
    usb_cdc_init();
    cli_init(usb_cdc_send_data, usb_cdc_receive_data, cli_cmds, (sizeof(cli_cmds) / sizeof(cli_cmd_t)));
}


void cli_cmd_process(void) {
    cli_process();
}




static error_t cli_cmd_reboot(uint32_t argc, const uint8_t **argv, cli_call_state_t state)  {
    NVIC_SystemReset();
    return E_OK;
}


static error_t cli_cmd_rr(uint32_t argc, const uint8_t **argv, cli_call_state_t state) {
    uint32_t addr;
    uint16_t reg_value;


    if (argc != 2) return E_INVALID_ARG;
    if (!pars_string_to_u32_and_check(argv[1], &addr, 0, RG_MAX_REG_ADDR)) return E_INVALID_ARG;

    if (regs_read_reg(addr, &reg_value)) {
        cli_safe_printf("%d", reg_value);
    }
    else {
        return E_FAILED;
    }
    return E_OK;
}


static error_t cli_cmd_wr(uint32_t argc, const uint8_t **argv, cli_call_state_t state) {
    uint32_t addr;
    uint32_t reg_value;


    if (argc != 3) return E_INVALID_ARG;
    if (!pars_string_to_u32_and_check(argv[1], &addr, 0, RG_MAX_REG_ADDR)) return E_INVALID_ARG;
    if (!pars_string_to_u32_and_check(argv[2], &reg_value, 0, UINT16_MAX)) return E_INVALID_ARG;

    if (!regs_write_reg(addr, reg_value)) {
        return E_FAILED;
    }
    return E_OK;
}


static error_t cli_cmd_clidbg(uint32_t argc, const uint8_t **argv, cli_call_state_t state) {
    if (argc != 2) return E_INVALID_ARG;
    if (!pars_is_there_template_in_string(argv[1], "en")) return E_INVALID_ARG;

    is_cli_dbg_mode = true;
    return E_OK;
}


static error_t cli_cmd_tlog(uint32_t argc, const uint8_t **argv, cli_call_state_t state) {
    static uint32_t log_period_ms;
    static timer_t log_timer;


    if (state == CLI_CALL_FIRST) {
        if (argc != 2) return E_INVALID_ARG;
        if (!pars_string_to_u32_and_check(argv[1], &log_period_ms, 0, 0)) return E_INVALID_ARG;

        cli_safe_printf("Temp_c; Heat_en");
        log_timer = timer_start_ms(log_period_ms);
        return E_ASYNC_WAIT;
    }
    if (state == CLI_CALL_REPEATED) {
        if (timer_triggered(log_timer)) {
            log_timer = timer_restart_ms(log_timer, log_period_ms);
            cli_safe_printf("\r\n%d; %d", heater_current_temperature_c, is_heater_pin_en);
            return E_ASYNC_WAIT;
        }

        return E_ASYNC_WAIT;
    }
    return E_OK;
}


static error_t cli_cmd_tconf(uint32_t argc, const uint8_t **argv, cli_call_state_t state) {
    uint32_t active_time_ms;
    uint32_t delay_time_ms;
    uint32_t hist_on_c;
    uint32_t hist_off_c;


    if (argc == 1) {
        cli_safe_printf("act_ms = %d\r\ndel_ms = %d\r\nhon_c = %d\r\nnoff_c = %d", heater_active_time_ms, heater_delay_time_ms, heater_hist_on_c, heater_hist_off_c);
        return E_OK;
    }
    else if (argc == 5) {
        if (!pars_string_to_u32_and_check(argv[1], &active_time_ms, 0, 0)) return E_INVALID_ARG;
        if (!pars_string_to_u32_and_check(argv[2], &delay_time_ms, 0, 0)) return E_INVALID_ARG;
        if (!pars_string_to_u32_and_check(argv[3], &hist_on_c, 0, UINT8_MAX)) return E_INVALID_ARG;
        if (!pars_string_to_u32_and_check(argv[4], &hist_off_c, 0, UINT8_MAX)) return E_INVALID_ARG;
        
        heater_active_time_ms = active_time_ms;
        heater_delay_time_ms = delay_time_ms;
        heater_hist_on_c = hist_on_c;
        heater_hist_off_c = hist_off_c;
        return E_OK;
    }
    return E_INVALID_ARG;
}




static bool pars_string_to_s32_and_check(const uint8_t *str, int32_t *digit, int32_t min, int32_t max) {
    if (!pars_string_to_s32(str, digit)) return false;
    if (min == max) return true;
    if ((*digit < min) || (*digit > max)) return false;
    return true;
}


static bool pars_string_to_u32_and_check(const uint8_t *str, uint32_t *digit, uint32_t min, uint32_t max) {
    if (!pars_string_to_u32(str, digit)) return false;
    if (min == max) return true;
    if ((*digit < min) || (*digit > max)) return false;
    return true;
}
