//  ***************************************************************************
/// @file    cli_cmd.c
//  ***************************************************************************
#include "cli_cmd.h"
#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>
#include "usb_cdc.h"
#include "cli.h"
#include "parsers.h"
#include "common/error.h"
#include "hal/systimer/systimer.h"
#include "profiles.h"


static error_t cli_cmd_reboot(uint32_t argc, const uint8_t **argv, cli_call_state_t state);
static error_t cli_cmd_pget(uint32_t argc, const uint8_t **argv, cli_call_state_t state);
static error_t cli_cmd_psetn(uint32_t argc, const uint8_t **argv, cli_call_state_t state);
static error_t cli_cmd_psets(uint32_t argc, const uint8_t **argv, cli_call_state_t state);
static error_t cli_cmd_psave(uint32_t argc, const uint8_t **argv, cli_call_state_t state);

const cli_cmd_t cli_cmds[] = {
    {
        .name = "reboot",
        .usage = "",
        .func = cli_cmd_reboot
    },
    {
        .name = "pget",
        .usage = "PROF_ID",
        .func = cli_cmd_pget
    },
    {
        .name = "psetn",
        .usage = "PROF_ID NAME",
        .func = cli_cmd_psetn
    },
    {
        .name = "psets",
        .usage = "PROF_ID STAGE_ID TEMP_C DURATION_S FUN_PERIOD_S FUN_DUTY_C_PCT",
        .func = cli_cmd_psets
    },
    {
        .name = "psave",
        .usage = "",
        .func = cli_cmd_psave
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


static error_t cli_cmd_pget(uint32_t argc, const uint8_t **argv, cli_call_state_t state) {
    int32_t profile_id;
    char string[40];
    uint32_t i;


    if (argc != 2) return E_INVALID_ARG;
    if (!pars_string_to_digit(argv[1], &profile_id) || (profile_id >= MAX_PROFILES_QTY)) return E_INVALID_ARG;

    cli_safe_print("\r\n");
    cli_safe_print(profiles[profile_id].name);
    for (i = 0; i < PROFILE_MAX_STAGES_QTY; i++) {
        cli_safe_print("\r\n---");
        snprintf(string, sizeof(string), "\r\nStage %d", i);    cli_safe_print(string);
        snprintf(string, sizeof(string), "\r\nT = %d", profiles[profile_id].stages[i].temperature_c);    cli_safe_print(string);
        snprintf(string, sizeof(string), "\r\nD = %d", profiles[profile_id].stages[i].duration_s);    cli_safe_print(string);
        snprintf(string, sizeof(string), "\r\nFp = %d", profiles[profile_id].stages[i].fun_period_s);    cli_safe_print(string);
        snprintf(string, sizeof(string), "\r\nFdc = %d", profiles[profile_id].stages[i].fun_duty_cycle_pct);    cli_safe_print(string);
    }
    cli_safe_print("\r\n");
    return E_OK;
}


static error_t cli_cmd_psetn(uint32_t argc, const uint8_t **argv, cli_call_state_t state) {
    int32_t profile_id;


    if (argc < 2) return E_INVALID_ARG;
    if (!pars_string_to_digit(argv[1], &profile_id) || (profile_id >= MAX_PROFILES_QTY)) return E_INVALID_ARG;
    if (argc == 3) {
        if (strlen(argv[2]) > PROFILE_MAX_NAME_SIZE) return E_INVALID_ARG;
        strcpy(profiles[profile_id].name, argv[2]);
    }
    else {
        profiles[profile_id].name[0] = '\0';
    }
    cli_safe_print("\r\n");
    return E_OK;
}


static error_t cli_cmd_psets(uint32_t argc, const uint8_t **argv, cli_call_state_t state) {
    int32_t profile_id, stage_id;
    int32_t temp, duration, fun_period, fun_duty_cycle;


    if (argc != 7) return E_INVALID_ARG;
    if (!pars_string_to_digit(argv[1], &profile_id) || (profile_id >= MAX_PROFILES_QTY)) return E_INVALID_ARG;
    if (!pars_string_to_digit(argv[2], &stage_id) || (stage_id >= PROFILE_MAX_STAGES_QTY)) return E_INVALID_ARG;
    if (!pars_string_to_digit(argv[3], &temp)) return E_INVALID_ARG;
    if (!pars_string_to_digit(argv[4], &duration)) return E_INVALID_ARG;
    if (!pars_string_to_digit(argv[5], &fun_period)) return E_INVALID_ARG;
    if (!pars_string_to_digit(argv[6], &fun_duty_cycle)) return E_INVALID_ARG;

    profiles[profile_id].stages[stage_id].temperature_c = temp;
    profiles[profile_id].stages[stage_id].duration_s = duration;
    profiles[profile_id].stages[stage_id].fun_period_s = fun_period;
    profiles[profile_id].stages[stage_id].fun_duty_cycle_pct = fun_duty_cycle;
    cli_safe_print("\r\n");
    return E_OK;
}


static error_t cli_cmd_psave(uint32_t argc, const uint8_t **argv, cli_call_state_t state) {
    profiles_save_all_to_flash();
    cli_safe_print("\r\n");
    return E_OK;
}
