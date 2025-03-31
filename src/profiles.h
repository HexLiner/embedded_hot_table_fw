//  ***************************************************************************
/// @file    profiles.h
/// @brief   Device profiles
//  ***************************************************************************
#ifndef _PROFILES_H_
#define _PROFILES_H_

#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>
#include "int_flash_driver.h"


#define MAX_PROFILES_QTY         (10)
#define PROFILE_MAX_NAME_SIZE    (18)
#define PROFILE_MAX_STAGES_QTY   (3)
#define PROFILES_FLASH_ADDR_BASE (0x08007C00)


typedef struct {
    uint8_t  temperature_c;
    uint32_t duration_s;
    uint32_t fun_period_s;
    uint8_t  fun_duty_cycle_pct;
} profile_stage_t;

typedef struct {
    uint8_t name[PROFILE_MAX_NAME_SIZE + 1];
    profile_stage_t stages[PROFILE_MAX_STAGES_QTY];
} profile_t;


extern profile_t profiles[MAX_PROFILES_QTY];
extern uint8_t active_profiles_qty; 




extern void profiles_init(void);
extern void profiles_save_all_to_flash(void);


#endif   // _PROFILES_H_
