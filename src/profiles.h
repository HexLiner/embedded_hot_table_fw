//  ***************************************************************************
/// @file    profiles.h
/// @brief   Device profiles
//  ***************************************************************************
#ifndef _PROFILES_H_
#define _PROFILES_H_

#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>
#include "registers.h"
#include "flash.h"
#include "error_handling.h"


typedef struct {
    uint16_t temperature_c;
    uint32_t duration_s;
    uint32_t fun_period_s;
    uint16_t fun_duty_cycle_pct;
} profile_stage_t;

typedef struct {
    uint8_t name[RG_PROFILE_NAME_SIZE + 1];
    profile_stage_t stages[RG_PROFILE_STAGES_QTY];
} profile_t;


extern profile_t profiles[RG_PROFILES_QTY];
extern uint8_t active_profiles_qty;


extern void profiles_init(void);


#endif   // _PROFILES_H_
