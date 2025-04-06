//  ***************************************************************************
/// @file    profiles.c
//  ***************************************************************************
#include "profiles.h"


profile_t profiles[RG_PROFILES_QTY];
uint8_t active_profiles_qty; 



void profiles_init(void) {
    uint32_t flash_address, i, j;


    // Load profiles from flash
    flash_address = 0;
    for (i = 0; i < RG_PROFILES_QTY; i++) {
        flash_read_bytes(flash_address, profiles[i].name, RG_PROFILE_NAME_SIZE);
        profiles[i].name[RG_PROFILE_NAME_SIZE] = '\0';    // guaranted EOL
        flash_address += RG_PROFILE_NAME_SIZE;

        for (j = 0; j < RG_PROFILE_STAGES_QTY; j++) {
            flash_read_u16(flash_address, &profiles[i].stages[j].temperature_c);
            flash_address += RG_PROFILE_STAGE_TEMPERATURE_C_SIZE;
            flash_read_u32(flash_address, &profiles[i].stages[j].duration_s);
            flash_address += RG_PROFILE_STAGE_DURATION_S_SIZE;
            flash_read_u32(flash_address, &profiles[i].stages[j].fun_period_s);
            flash_address += RG_PROFILE_STAGE_FUN_PERIOD_S_SIZE;
            flash_read_u16(flash_address, &profiles[i].stages[j].fun_duty_cycle_pct);
            flash_address += RG_PROFILE_STAGE_FUN_DUTY_CYCLE_PCT_SIZE;
        }
    }

    // Check profiles
    for (active_profiles_qty = 0; active_profiles_qty < RG_PROFILES_QTY; active_profiles_qty++) {
        if ((profiles[active_profiles_qty].name[0] == '\0') || (profiles[active_profiles_qty].name[0] == 0xFF)) break;
    }

    if (active_profiles_qty == 0) eh_set_fail_cfg_error();
}
