//  ***************************************************************************
/// @file    system_operation.c
//  ***************************************************************************
#include "system_operation.h"


typedef enum {
    SO_PROCESS_STATE_INTRO = 0,
    SO_PROCESS_STATE_PROFILE_SELECTION,
    SO_PROCESS_STATE_PROCESS,
    SO_PROCESS_STATE_PROCESS_RESULT_SCREEN,
    SO_PROCESS_STATE_CLI_DBG,
    SO_PROCESS_STATE_FAIL,
} system_operation_process_state_t;


bool is_cli_dbg_mode;


static bool is_any_button_event(void);
static void clear_all_buttons_events_flags(void);


static button_t select_button, start_button;




void system_operation_init(void) {
    gui_init();
    indicators_init();
    button_init(&select_button, PA6, true);
    button_init(&start_button, PA7, true);
    outputs_init();

    is_cli_dbg_mode = false;
}


void system_operation_process(void) {
    static system_operation_process_state_t so_process_state = SO_PROCESS_STATE_INTRO;
    static bool is_state_init = true;
    static timer_t process_timer, process_stage_timer, update_process_screen_timer;
    static uint8_t profile_index, process_stage_index;
    static int32_t common_process_time_s;
    uint8_t error_msg[8];


    gui_process();
    indicators_process();
    outputs_process();
    button_process(&select_button);
    button_process(&start_button);


    if ((fail_code != 0) && (so_process_state != SO_PROCESS_STATE_FAIL)) {
        is_state_init = true;
        so_process_state = SO_PROCESS_STATE_FAIL;
    }
    if ((is_cli_dbg_mode) && (so_process_state != SO_PROCESS_STATE_CLI_DBG)) {
        is_state_init = true;
        so_process_state = SO_PROCESS_STATE_CLI_DBG;
    }


    switch (so_process_state) {
        case SO_PROCESS_STATE_INTRO:
            if (is_state_init) {
                gui_reset_standby_timer();
                gui_print_center_msg("/E/");
                process_timer = timer_start_ms(2000);
                is_state_init = false;
            }
            if (timer_triggered(process_timer)) {
                profile_index = 0;
                is_state_init = true;
                so_process_state = SO_PROCESS_STATE_PROFILE_SELECTION;
            }
            break;

        
        case SO_PROCESS_STATE_PROFILE_SELECTION:
            if (is_state_init) {
                gui_reset_standby_timer();
                gui_print_profiles_menu_screen(profile_index);
                clear_all_buttons_events_flags();
                is_state_init = false;
            }

            // Display wake-up
            if (gui_is_standby) {
                if (is_any_button_event()) gui_reset_standby_timer();
            }
            // Select next item
            else if (button_is_press_event(&select_button)) {
                profile_index++;
                if (profile_index >= active_profiles_qty) profile_index = 0;
                gui_reset_standby_timer();
                gui_print_profiles_menu_screen(profile_index);
            }
            // Start process
            else if (button_is_press_event(&start_button)) {
                is_state_init = true;
                so_process_state = SO_PROCESS_STATE_PROCESS;
            }
            break;


        case SO_PROCESS_STATE_PROCESS:
            if (is_state_init) {
                gui_reset_standby_timer();
                common_process_time_s = 0;
                for (process_stage_index = 0; process_stage_index < PROFILE_MAX_STAGES_QTY; process_stage_index++) {
                    common_process_time_s += profiles[profile_index].stages[process_stage_index].duration_s;
                }
                process_stage_index = 0;
                indicators_buzzer_short_beep();
                indicators_led_process(true);
                clear_all_buttons_events_flags();
            }

            // Break process
            if (button_is_long_press_event(&select_button) || button_is_long_press_event(&start_button)) {
                fun_dis();
                heater_dis();
                indicators_buzzer_short_beep();
                indicators_led_process(false);
                gui_print_center_msg("BREAK");
                process_timer = timer_start_ms(2000);
                is_state_init = true;
                so_process_state = SO_PROCESS_STATE_PROCESS_RESULT_SCREEN;
            }
            // Stage done
            else if (is_state_init || timer_triggered(process_stage_timer)) {
                // Next process stage
                if ((process_stage_index < PROFILE_MAX_STAGES_QTY) && (profiles[profile_index].stages[process_stage_index].duration_s > 0)) {
                    gui_print_process_screen_init(profiles[profile_index].stages[process_stage_index].temperature_c);
                    gui_update_process_screen(heater_current_temperature_c, common_process_time_s);

                    fun_en(profiles[profile_index].stages[process_stage_index].fun_period_s, profiles[profile_index].stages[process_stage_index].fun_duty_cycle_pct);
                    heater_en(profiles[profile_index].stages[process_stage_index].temperature_c);

                    process_stage_timer = timer_start_ms(profiles[profile_index].stages[process_stage_index].duration_s * 1000);
                    update_process_screen_timer = timer_start_ms(1000);
                    process_stage_index++;
                    is_state_init = false;
                }
                // Process done
                else {
                    fun_dis();
                    heater_dis();
                    indicators_buzzer_process_done_beep();
                    indicators_led_process(false);
                    gui_print_center_msg("DONE");
                    process_timer = timer_start_ms(10000);
                    is_state_init = true;
                    so_process_state = SO_PROCESS_STATE_PROCESS_RESULT_SCREEN;
                }
            }
            // Display wake-up
            else if (is_any_button_event()) {
                gui_reset_standby_timer();
            }
            // Update process screen
            else if (timer_triggered(update_process_screen_timer)) {
                update_process_screen_timer = timer_restart_ms(update_process_screen_timer, 1000);
                if (common_process_time_s > 0) common_process_time_s--;
                gui_update_process_screen(heater_current_temperature_c, common_process_time_s);
            }
            break;


        case SO_PROCESS_STATE_PROCESS_RESULT_SCREEN:
            if (is_state_init) {
                // process_timer is set from calling state !
                gui_reset_standby_timer();
                clear_all_buttons_events_flags();
                is_state_init = false;
            }
            if (timer_triggered(process_timer) || is_any_button_event()) {
                indicators_buzzer_beep_terminate();
                is_state_init = true;
                so_process_state = SO_PROCESS_STATE_PROFILE_SELECTION;
            }
            break;


        case SO_PROCESS_STATE_CLI_DBG:
            if (is_state_init) {
                fun_dis();
                heater_dis();

                gui_print_center_msg("CLI_DBG");
                gui_reset_standby_timer();
                indicators_buzzer_error_beep();
                indicators_led_error(true);
                clear_all_buttons_events_flags();
                is_state_init = false;
            }

            if (is_any_button_event()) {
                gui_reset_standby_timer();
            }
            break;


        case SO_PROCESS_STATE_FAIL:
            if (is_state_init) {
                fun_dis();
                heater_dis();

                snprintf(error_msg, sizeof(error_msg), "0x%04X", fail_code);
                gui_print_error(error_msg);
                gui_reset_standby_timer();
                indicators_buzzer_error_beep();
                indicators_led_error(true);
                clear_all_buttons_events_flags();
                is_state_init = false;
            }

            if (is_any_button_event()) {
                gui_reset_standby_timer();
            }
            break;
    }
}




static bool is_any_button_event(void) {
    bool result = false;

    result |= button_is_press_event(&start_button);
    result |= button_is_long_press_event(&start_button);
    result |= button_is_press_event(&select_button);
    result |= button_is_long_press_event(&select_button);

    return result;
}


static void clear_all_buttons_events_flags(void) {
    button_clear_events_flags(&start_button);
    button_clear_events_flags(&select_button);
}
