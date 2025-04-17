#ifndef SSD1306_H_
#define SSD1306_H_

#include <stdint.h>
#include <stdbool.h>
#include "hal/i2c_driver.h"
#include "lib_config.h"


#define SSD1306_SIMW_W             (5)
#define SSD1306_SIMW_W_WITH_SPACE  (SSD1306_SIMW_W + 1)
#define SSD1306_SIMW_H             (7)


typedef enum {
    SSD1306_FOUNT_MODE_K1 = 1,
    SSD1306_FOUNT_MODE_K2 = 2,
    SSD1306_FOUNT_MODE_K3 = 3,
} ssd1306_fount_mode_t;


extern void ssd1306_init(i2c_t *i2c, uint8_t i2c_addr, uint32_t i2c_timeout);
extern void ssd1306_process(void);
extern void ssd1306_standby(bool en_dis);
extern void ssd1306_clear();
extern void ssd1306_set_img(uint8_t *data, uint8_t x, uint8_t y, uint8_t w, uint8_t h);
extern void ssd1306_print_digit(uint16_t digit, uint8_t digit_max_len, bool is_visible_zeros, ssd1306_fount_mode_t ssd1306_fount_mode, uint8_t x, uint8_t y);
extern void ssd1306_print_str(const uint8_t *str, uint8_t min_str_size, ssd1306_fount_mode_t ssd1306_fount_mode, uint8_t x, uint8_t y);
extern void ssd1306_print_simw(uint8_t simw, ssd1306_fount_mode_t ssd1306_fount_mode, uint8_t x, uint8_t y);


#endif   // SSD1306_H_
