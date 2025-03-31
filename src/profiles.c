//  ***************************************************************************
/// @file    profiles.c
//  ***************************************************************************
#include "profiles.h"


profile_t profiles[MAX_PROFILES_QTY];
uint8_t active_profiles_qty; 



void profiles_init(void) {
    uint32_t flash_address, i;


    // Load profiles from flash
    flash_address = PROFILES_FLASH_ADDR_BASE;
    for (i = 0; i < MAX_PROFILES_QTY; i++) {
        flash_read_bytes(flash_address, (uint8_t*)&profiles[i], sizeof(profile_t));
        profiles[i].name[PROFILE_MAX_NAME_SIZE] = '\0';    // guaranted EOL
        flash_address += sizeof(profile_t);
    }

    // Check profiles
    for (active_profiles_qty = 0; active_profiles_qty < MAX_PROFILES_QTY; active_profiles_qty++) {
        if (profiles[active_profiles_qty].name[0] == '\0') break;
    }

    ////if (active_profiles_qty == 0) ////eh
}


void profiles_save_all_to_flash(void) {
    uint32_t flash_address, i;
    bool result;


    flash_address = PROFILES_FLASH_ADDR_BASE;
    result = flash_erase_page(flash_address);
    if (!result){
        ////eh
        return;
    }
        
    for (i = 0; i < MAX_PROFILES_QTY; i++) {
        result = flash_write_bytes(flash_address, (uint8_t*)&profiles[i], sizeof(profile_t));
        if (!result){
            ////eh
            return;
        }
        flash_address += sizeof(profile_t);
    }
}
