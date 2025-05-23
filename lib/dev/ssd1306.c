#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include "dev/ssd1306.h"
#include "hal/i2c_driver.h"
#include "lib_config.h"


#define SSD1306_CONTRAST             (0x81)
#define SSD1306_DISPLAYALL_ON_RESUME (0xA4)
#define SSD1306_DISPLAYALL_ON        (0xA5)
#define SSD1306_INVERTED_OFF         (0xA6)
#define SSD1306_INVERTED_ON          (0xA7)
#define SSD1306_DISPLAY_OFF          (0xAE)
#define SSD1306_DISPLAY_ON           (0xAF)
#define SSD1306_DISPLAYOFFSET        (0xD3)
#define SSD1306_COMPINS              (0xDA)
#define SSD1306_VCOMDETECT           (0xDB)
#define SSD1306_DISPLAYCLOCKDIV      (0xD5)
#define SSD1306_PRECHARGE            (0xD9)
#define SSD1306_MULTIPLEX            (0xA8)
#define SSD1306_LOWCOLUMN            (0x00)
#define SSD1306_HIGHCOLUMN           (0x10)
#define SSD1306_STARTLINE            (0x40)
#define SSD1306_MEMORYMODE           (0x20)
#define SSD1306_MEMORYMODE_HORZONTAL (0x00)
#define SSD1306_MEMORYMODE_VERTICAL  (0x01)
#define SSD1306_MEMORYMODE_PAGE      (0x10)
#define SSD1306_COLUMNADDR           (0x21)
#define SSD1306_PAGEADDR             (0x22)
#define SSD1306_COM_REMAP_OFF        (0xC0)
#define SSD1306_COM_REMAP_ON         (0xC8)
#define SSD1306_CLUMN_REMAP_OFF      (0xA0)
#define SSD1306_CLUMN_REMAP_ON       (0xA1)
#define SSD1306_CHARGEPUMP           (0x8D)
#define SSD1306_EXTERNALVCC          (0x01)
#define SSD1306_SWITCHCAPVCC         (0x02)


// ascii 32 - 126
static const uint8_t fount[][5] = {
    {0xFF, 0xFF, 0xFF, 0xFF, 0xFF},    // full - not ASCII !
    {0x00, 0x00, 0x00, 0x00, 0x00},    // space
    {0x00, 0x0C, 0xBE, 0x0C, 0x00},    // !
    {0x0E, 0x06, 0x00, 0x0E, 0x06},    // "
    {0x48, 0xFC, 0x48, 0xFC, 0x48},    // #
    {0x48, 0x56, 0xD4, 0x24, 0x00},    // $
    {0xC6, 0x26, 0x10, 0xC8, 0xC6},    // %
    {0x6C, 0x92, 0xAC, 0x40, 0xA0},    // &
    {0x00, 0x0E, 0x06, 0x00, 0x00},    // '
    {0x00, 0x7C, 0x82, 0x00, 0x00},    // (
    {0x00, 0x82, 0x7C, 0x00, 0x00},    // )
    {0x10, 0x7C, 0x38, 0x7C, 0x10},    // *
    {0x10, 0x10, 0x7C, 0x10, 0x10},    // +
    {0x00, 0xE0, 0x60, 0x00, 0x00},    // ,
    {0x10, 0x10, 0x10, 0x10, 0x10},    // -
    {0x00, 0x60, 0x60, 0x00, 0x00},    // .
    {0x40, 0x20, 0x10, 0x08, 0x04},    // /
    {0x7C, 0xA2, 0x92, 0x8A, 0x7C},    // 0
    {0x00, 0x84, 0xFE, 0x80, 0x00},    // 1
    {0xC4, 0xA2, 0x92, 0x92, 0x8C},
    {0x44, 0x92, 0x92, 0x92, 0x6C},
    {0x30, 0x28, 0x24, 0xFE, 0x20},
    {0x5E, 0x92, 0x92, 0x92, 0x62},
    {0x78, 0x94, 0x92, 0x92, 0x60},
    {0x02, 0xE2, 0x12, 0x0A, 0x06},
    {0x6C, 0x92, 0x92, 0x92, 0x6C},
    {0x0C, 0x92, 0x92, 0x52, 0x3C},    // 9
    {0x00, 0x6C, 0x6C, 0x00, 0x00},    // :
    {0x00, 0xEC, 0x6C, 0x00, 0x00},    // ;
    {0x10, 0x28, 0x44, 0x82, 0x00},    // <
    {0x48, 0x48, 0x48, 0x48, 0x48},    // =
    {0x00, 0x82, 0x44, 0x28, 0x10},    // >
    {0x04, 0x02, 0xB2, 0x12, 0x0C},    // ?
    {0x7C, 0x82, 0xBA, 0xAA, 0x3C},    // @
    {0xFC, 0x22, 0x22, 0x22, 0xFC},    // A
    {0xFE, 0x92, 0x92, 0x92, 0x6C},
    {0x7C, 0x82, 0x82, 0x82, 0x44},
    {0xFE, 0x82, 0x82, 0x82, 0x7C},
    {0xFE, 0x92, 0x92, 0x92, 0x82},
    {0xFE, 0x12, 0x12, 0x12, 0x02},
    {0x7C, 0x82, 0x92, 0x92, 0xF4},    // G
    {0xFE, 0x10, 0x10, 0x10, 0xFE},    // H
    {0x00, 0x82, 0xFE, 0x82, 0x00},
    {0x60, 0x80, 0x80, 0x80, 0x7E},    // J
    {0xFE, 0x10, 0x28, 0x44, 0x82},
    {0xFE, 0x80, 0x80, 0x80, 0x80},
    {0xFE, 0x04, 0x08, 0x04, 0xFE},
    {0xFE, 0x04, 0x08, 0x10, 0xFE},
    {0x7C, 0x82, 0x82, 0x82, 0x7C},    // O
    {0xFE, 0x12, 0x12, 0x12, 0x0C},    // P
    {0x7C, 0x82, 0xA2, 0x42, 0xBC},
    {0xFE, 0x12, 0x12, 0x32, 0xCC},
    {0x4C, 0x92, 0x92, 0x92, 0x64},
    {0x02, 0x02, 0xFE, 0x02, 0x02},
    {0x7E, 0x80, 0x80, 0x80, 0x7E},
    {0x3E, 0x40, 0x80, 0x40, 0x3E},
    {0x7E, 0x80, 0x78, 0x80, 0x7E},
    {0xC6, 0x28, 0x10, 0x28, 0xC6},    // X
    {0x0E, 0x10, 0xE0, 0x10, 0x0E},    // Y
    {0xE2, 0x92, 0x8A, 0x86, 0x00},    // Z
    {0x00, 0xFE, 0x82, 0x82, 0x00},    // [
    {0x04, 0x08, 0x10, 0x20, 0x40},    
    {0x00, 0x82, 0x82, 0xFE, 0x00},    // ]
    {0x08, 0x04, 0x02, 0x04, 0x08},    // ^
    {0x80, 0x80, 0x80, 0x80, 0x80},    // _
    {0x00, 0x06, 0x0E, 0x00, 0x00},    // `
#if (SSD1306_USE_SMALL_REGISTER)
    {0x40, 0xA8, 0xA8, 0xA8, 0xF0},    // a
    {0xFE, 0x88, 0x88, 0x88, 0x70},
    {0x70, 0x88, 0x88, 0x88, 0x00},
    {0x70, 0x88, 0x88, 0x88, 0xFE},
    {0x70, 0xA8, 0xA8, 0xA8, 0x10},
    {0x10, 0xFC, 0x12, 0x12, 0x00},
    {0x10, 0xA8, 0xA8, 0xA8, 0x78},    // g
    {0xFE, 0x08, 0x08, 0xF0, 0x00},
    {0x00, 0x00, 0xFA, 0x80, 0x00},
    {0x40, 0x80, 0x88, 0x7A, 0x00},    // j
    {0xFE, 0x20, 0x50, 0x88, 0x00},
    {0x00, 0x00, 0xFE, 0x80, 0x00},
    {0xF8, 0x08, 0x30, 0x08, 0xF0},
    {0xF8, 0x08, 0x08, 0xF0, 0x00},
    {0x70, 0x88, 0x88, 0x88, 0x70},    // o
    {0xF8, 0x28, 0x28, 0x28, 0x10},    // p
    {0x70, 0x88, 0x88, 0x88, 0xF8},
    {0x88, 0xF0, 0x88, 0x08, 0x10},
    {0x10, 0xA8, 0xA8, 0xA8, 0x40},
    {0x08, 0x7C, 0x88, 0x48, 0x00},
    {0x78, 0x80, 0x40, 0xF8, 0x00},
    {0x38, 0x40, 0x80, 0x40, 0x38},
    {0x78, 0xC0, 0x60, 0xC0, 0x78},    // w
    {0x78, 0x80, 0x40, 0xF8, 0x00},    // x  - incorrect!!!!  ////
    {0x38, 0x40, 0xC0, 0x78, 0x00},    // y
    {0xC8, 0xA8, 0xA8, 0x98, 0x00},    // z
#endif   // SSD1306_USE_SMALL_REGISTER
    {0x10, 0x7C, 0x82, 0x82, 0x00},    // {
    {0x00, 0x00, 0xFF, 0x00, 0x00},    // |
    {0x00, 0x82, 0x82, 0x7C, 0x10},    // }
    {0x04, 0x02, 0x04, 0x02, 0x00},    // ~
};

static bool is_gram_data_changed = true;
static uint8_t gram_buff[(SSD1306_W / 8) * SSD1306_H];

static i2c_transaction_t transaction;
static uint32_t i2c_timeout_int;
static i2c_t *i2c_int;

static void ssd1306_send_cmd(uint8_t command);
static void ssd1306_i2c_write_blocking(const uint8_t *data, uint8_t data_size);
static void dig_to_string(uint16_t digit, bool is_visible_zeros, uint8_t *string);




void ssd1306_init(i2c_t *i2c, uint8_t i2c_addr, uint32_t i2c_timeout) {
    uint8_t i;
    // this is a list of setup commands for the display
    const uint8_t setup[] = {
        SSD1306_DISPLAY_OFF,
        SSD1306_LOWCOLUMN,
        SSD1306_HIGHCOLUMN,
        SSD1306_STARTLINE,

        SSD1306_MEMORYMODE,
        SSD1306_MEMORYMODE_HORZONTAL,

        SSD1306_CONTRAST,
        0xFF,

        SSD1306_INVERTED_OFF,

        SSD1306_MULTIPLEX,
        63,

        SSD1306_DISPLAYOFFSET,
        0x00,

        SSD1306_DISPLAYCLOCKDIV,
        0x80,

        SSD1306_PRECHARGE,
        0x22,

        SSD1306_COMPINS,
        0x02,

        SSD1306_VCOMDETECT,
        0x40,

        SSD1306_CHARGEPUMP,
        0x14,

        SSD1306_DISPLAYALL_ON_RESUME,
        SSD1306_DISPLAY_ON
    };

    i2c_int = i2c;
    i2c_timeout_int = i2c_timeout;

    transaction.address = i2c_addr;
    transaction.rx_data = NULL;
    transaction.rx_size = 0;

    // send each one of the setup commands
    for (i = 0; i < sizeof(setup); i++) {
        ssd1306_send_cmd(setup[i]);
    }
}


void ssd1306_process(void) {
    static bool is_set_area_proc = true;
    static uint32_t buff_index;
    uint8_t simw_data[17];


    if (is_set_area_proc) {
        if (!is_gram_data_changed) return;

        // set area
        ssd1306_send_cmd(SSD1306_COLUMNADDR);
        ssd1306_send_cmd(0);
        ssd1306_send_cmd(SSD1306_W - 1);
        ssd1306_send_cmd(SSD1306_PAGEADDR);
        ssd1306_send_cmd(0);
        ssd1306_send_cmd(SSD1306_H - 1);

        buff_index = 0;
        is_gram_data_changed = false;
        is_set_area_proc = false;
    }
    else {
        simw_data[0] = 0x40;   // is data
        memcpy(&simw_data[1], &gram_buff[buff_index], 16);
        ssd1306_i2c_write_blocking(simw_data, 17);
        buff_index += 16;
        if (buff_index > sizeof(gram_buff)) {
            is_set_area_proc = true;
        }
    }
}


// notes: GRAM data is not cleared. GRAM data is written in standby.
void ssd1306_standby(bool en_dis) {
    if (en_dis) ssd1306_send_cmd(SSD1306_DISPLAY_OFF);
    else ssd1306_send_cmd(SSD1306_DISPLAY_ON);
}


void ssd1306_clear(void) {
    memset(gram_buff, 0, sizeof(gram_buff));
}


void ssd1306_set_img(uint8_t *data, uint8_t x, uint8_t y, uint8_t w, uint8_t h) {
    uint32_t hi, wi;
    uint32_t buff_index_base, buff_index;
    uint32_t data_index;


    y = y >> 3;
    if (h & 0b111) h += 0b1000;
    h = h >> 3;
    buff_index_base = (y * SSD1306_W) + x;

    data_index = 0;
    for (hi = 0; hi < h; hi++) {
        buff_index = buff_index_base + (SSD1306_W * hi);
        for (wi = 0; wi < w; wi++) {
            gram_buff[buff_index] = data[data_index];
            buff_index++;
            data_index++;
        }
    }

    is_gram_data_changed = true;
}


void ssd1306_print_digit(uint16_t digit, uint8_t digit_max_len, bool is_visible_zeros, ssd1306_fount_mode_t ssd1306_fount_mode, uint8_t x, uint8_t y) {
    uint8_t lcd_string[6];


    dig_to_string(digit, is_visible_zeros, lcd_string);
    ssd1306_print_str(&lcd_string[5 - digit_max_len], 0, ssd1306_fount_mode, x, y);
}


// k = 1 -> 21 x 4 simw  (0, 6, 12, 18, 24, 30, 36, 42, 48, 54, 60, 66, 72, 78, 84, 90, 96, 102, 108, 114, 120)
// k = 2 -> 10 x 2 simw  (0, 12, 24, 36, 48, 60, 72, 84, 96, 108)
void ssd1306_print_str(const uint8_t *str, uint8_t min_str_size, ssd1306_fount_mode_t ssd1306_fount_mode, uint8_t x, uint8_t y) {
    uint8_t curr_x;
    uint8_t simw_qty;


    curr_x = x;
    while(*str != '\0') {
        ssd1306_print_simw(*str, ssd1306_fount_mode, curr_x, y);
        curr_x += (SSD1306_SIMW_W * (uint8_t)ssd1306_fount_mode) + (uint8_t)ssd1306_fount_mode;
        str++;
        simw_qty++;
    }
    while(simw_qty < min_str_size) {
        ssd1306_print_simw(' ', ssd1306_fount_mode, curr_x, y);
        curr_x += (SSD1306_SIMW_W * (uint8_t)ssd1306_fount_mode) + (uint8_t)ssd1306_fount_mode;
        simw_qty++;
    }
}


void ssd1306_print_simw(uint8_t simw, ssd1306_fount_mode_t ssd1306_fount_mode, uint8_t x, uint8_t y) {
    uint8_t simw_w, simw_h;
    uint8_t simw_data[20];
    uint8_t fount_data;
    uint8_t simw_data_index, simw_bit_cnt;
    uint8_t fount_data_index, fount_data_bit;
    int8_t k_cnt;
    uint8_t simw_pixel_msk;
    uint8_t i;


    if ((simw < 0x20) || (simw > 0x7E)) simw = '#';   // out of ASCII printable symbols range
    #if (SSD1306_USE_SMALL_REGISTER == 0)
    if ((simw >= 'a') && (simw <= 'z')) simw = simw - ('a' - 'A');
    else if (simw > 'z') simw = simw - ('z' - 'a' + 1);
    #endif   // SSD1306_USE_SMALL_REGISTER

    simw -= (0x20 - 1);   // array offset


    // init send data
    for (simw_data_index = 0; simw_data_index < 20; simw_data_index++) simw_data[simw_data_index] = 0;

    // simw scaling
    simw_data_index = 0;
    simw_bit_cnt = 0;
    simw_pixel_msk = 0xFF << (8 - ssd1306_fount_mode);
    for (k_cnt = (ssd1306_fount_mode - 1); k_cnt >= 0; k_cnt--) {
        for (fount_data_index = 0; fount_data_index < 5; fount_data_index++) {
            fount_data = fount[(uint8_t)simw][fount_data_index] << (4 * k_cnt);

            for (fount_data_bit = (8 / (ssd1306_fount_mode)); fount_data_bit > 0; fount_data_bit--) {
                if (fount_data & 0x80) simw_data[simw_data_index] |= (simw_pixel_msk >> simw_bit_cnt);
                simw_bit_cnt += ssd1306_fount_mode;
                if (simw_bit_cnt > 7) {
                    simw_bit_cnt = 0;
                    simw_data_index += ssd1306_fount_mode;
                }
                fount_data = fount_data << 1;
            }
        }
    }
    if (ssd1306_fount_mode == 2) {
        for (i = 1; i < 20; i += 2) {
            simw_data[i] = simw_data[i - 1];
        }
    }

    simw_w = SSD1306_SIMW_W * (uint8_t)ssd1306_fount_mode;
    simw_h = SSD1306_SIMW_H * (uint8_t)ssd1306_fount_mode;
    ssd1306_set_img(simw_data, x, y, simw_w, simw_h);
}




static void ssd1306_send_cmd(uint8_t command) {
    uint8_t data[2];


    data[0] = 0x00;  // 0x00 is a byte indicating to ssd1306 that a command is being sent
    data[1] = command;
    ssd1306_i2c_write_blocking(data, 2);
}


static void ssd1306_i2c_write_blocking(const uint8_t *data, uint8_t data_size) {
    transaction.tx_data = data;
    transaction.tx_size = data_size;
    i2c_transfer(i2c_int, &transaction, 5, i2c_timeout_int);
}


static void dig_to_string(uint16_t digit, bool is_visible_zeros, uint8_t *string) {
    uint16_t tmp;
    uint8_t zero_symb;


    if (is_visible_zeros) zero_symb = '0';
    else zero_symb = ' ';


    tmp = digit / 10000;
    string[0] = tmp + 0x30;
    if (tmp == 0) string[0] = zero_symb;

    tmp = tmp * 10000;
    digit -= tmp;
    tmp = digit / 1000;
    string[1] = tmp + 0x30;
    if ((tmp == 0) && (string[0] == zero_symb)) string[1] = zero_symb;

    tmp = tmp * 1000;
    digit -= tmp;
    tmp = digit / 100;
    string[2] = tmp + 0x30;
    if ((tmp == 0) && (string[1] == zero_symb)) string[2] = zero_symb;

    tmp = tmp * 100;
    digit -= tmp;
    tmp = digit / 10;
    string[3] = tmp + 0x30;
    if ((tmp == 0) && (string[2] == zero_symb)) string[3] = zero_symb;

    tmp = tmp * 10;
    digit -= tmp;
    string[4] = digit + 0x30;

    string[5] = '\0';
}
