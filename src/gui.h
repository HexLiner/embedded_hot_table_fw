//  ***************************************************************************
/// @file    gui.h
/// @brief   GUI
//  ***************************************************************************
#ifndef _GUI_H_
#define _GUI_H_

#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include "hal/gpio.h"
#include "hal/sysclk.h"
#include "hal/systimer.h"
#include "hal/i2c_driver.h"
#include "dev/ssd1306.h"
#include "registers.h"
#include "profiles.h"


extern bool gui_is_standby;


extern void gui_init(void);
extern void gui_process(void);

extern void gui_reset_standby_timer(void);
 
extern void gui_clear_screen(void);
extern void gui_print_center_msg(const uint8_t *msg);
 
extern void gui_print_profiles_menu_screen(uint8_t selected_item);
extern void gui_profiles_menu_item_down(void);
 
extern void gui_print_process_screen_init(uint8_t temperature_set_c);
extern void gui_update_process_screen(uint8_t temperature_curr_c, uint32_t process_time_s);

extern void gui_print_error(const uint8_t *error_msg);

#endif   // _GUI_H_
