//  ***************************************************************************
/// @file    gui.h
/// @brief   GUI
//  ***************************************************************************
#include "gui.h"


#define SSD1306_SLAVE_ADDR (0b01111000)  // or 0b01111010  - 0 1 1 1 1 0 SA0 R/W# 
#define DISPLAY_STANDBY_TIMEOUT_MS (60 * 1000)

static handle_t i2c_handle;
static i2c_settings_t i2c_settings = {
    .speed_hz = 1000000,
    .scl_pin = PA1,
    .sda_pin = PA0
};

const char *gui_menu_empty_str = "                ";
static bool is_profiles_menu_screen;
static uint8_t process_time_h_prev, process_time_m_prev, process_time_s_prev;
static timer_t standby_timer;
bool gui_is_standby;



void gui_init(void) {
    i2c_init(&i2c_handle, NULL, &i2c_settings);
    ssd1306_init(i2c_handle, SSD1306_SLAVE_ADDR, 1000);
    ssd1306_clear();

    is_profiles_menu_screen = false;

    gui_is_standby = false;
    standby_timer = timer_start_ms(DISPLAY_STANDBY_TIMEOUT_MS);
}


void gui_process(void) {
    if (timer_triggered(standby_timer) && !gui_is_standby) {
        ssd1306_standby(true);
        gui_is_standby = true;
    }
}

void gui_reset_standby_timer(void) {
    if (gui_is_standby) {
        ssd1306_standby(false);
        gui_is_standby = false;
    }
    standby_timer = timer_start_ms(DISPLAY_STANDBY_TIMEOUT_MS);
}


void gui_clear_screen(void) {
    ssd1306_clear();
}


void gui_print_center_msg(const uint8_t *msg) {
    uint8_t msg_size, x;


    msg_size = strlen(msg);
    if (msg_size > 10) return;
    msg_size = (SSD1306_SIMW_W_WITH_SPACE * SSD1306_FOUNT_MODE_K2) * msg_size;
    x = SSD1306_W - msg_size;
    x = x / 2;

    ssd1306_clear();
    ssd1306_print_str(msg, SSD1306_FOUNT_MODE_K2, x, 8);
    is_profiles_menu_screen = false;
}


void gui_print_profiles_menu_screen(uint8_t selected_item) {
    uint32_t i, y;
  
  
    if (!is_profiles_menu_screen) ssd1306_clear();
    is_profiles_menu_screen = true;
    if (selected_item >= active_profiles_qty) selected_item = 0;


    if (selected_item > 0) ssd1306_print_str(profiles[selected_item - 1].name, SSD1306_FOUNT_MODE_K1, 12, 0);
    else ssd1306_print_str(gui_menu_empty_str, SSD1306_FOUNT_MODE_K1, 12, 0);
    for (i = 0; i < 3; i++) {
        y = selected_item + i;
        if (y < active_profiles_qty) ssd1306_print_str(profiles[y].name, SSD1306_FOUNT_MODE_K1, 12, (8 * (i + 1)));
        else ssd1306_print_str(gui_menu_empty_str, SSD1306_FOUNT_MODE_K1, 12, (8 * (i + 1)));
    }
    ssd1306_print_simw('>', SSD1306_FOUNT_MODE_K1, 0, 8);
    ssd1306_print_simw(' ', SSD1306_FOUNT_MODE_K1, 6, 8);
}


void gui_print_process_screen_init(uint8_t temperature_set_c) {
    ssd1306_clear();

    ssd1306_print_simw('/', SSD1306_FOUNT_MODE_K2, 48, 0);
    ssd1306_print_digit(temperature_set_c, 3, false, SSD1306_FOUNT_MODE_K2, 60, 0);
    ssd1306_print_simw('C', SSD1306_FOUNT_MODE_K2, 96, 0);

    ssd1306_print_simw(':', SSD1306_FOUNT_MODE_K2, 36, 16);
    ssd1306_print_simw(':', SSD1306_FOUNT_MODE_K2, 72, 16);

    process_time_h_prev = 255;
    process_time_m_prev = 255;
    process_time_s_prev = 255;
    is_profiles_menu_screen = false;
}


void gui_update_process_screen(uint8_t temperature_curr_c, uint32_t process_time_s) {
    uint32_t process_time_h, process_time_m;


    ssd1306_print_digit(temperature_curr_c, 3, false, SSD1306_FOUNT_MODE_K2, 12, 0);


    process_time_h = process_time_s / (60 * 60);
    process_time_s -= process_time_h * (60 * 60);
    process_time_m = process_time_s / 60;
    process_time_s -= process_time_m * 60;

    if (process_time_h_prev != process_time_h) ssd1306_print_digit(process_time_h, 2, true, SSD1306_FOUNT_MODE_K2, 12, 16);
    if (process_time_m_prev != process_time_m) ssd1306_print_digit(process_time_m, 2, true, SSD1306_FOUNT_MODE_K2, 48, 16);
    if (process_time_s_prev != process_time_s) ssd1306_print_digit(process_time_s, 2, true, SSD1306_FOUNT_MODE_K2, 84, 16);
    process_time_h_prev = process_time_h;
    process_time_m_prev = process_time_m;
    process_time_s_prev = process_time_s;
}

