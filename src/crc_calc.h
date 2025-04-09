//  ***************************************************************************
/// @file    crc_calc.h
/// @brief   CRC calculate functions
//  ***************************************************************************
#ifndef _CRC_CALC_H_
#define _CRC_CALC_H_

#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include "common/mcu.h"


typedef struct {
    uint8_t resolution;
    uint32_t poly;    // or reversed poly, if CRC_REFOUT == true
    uint32_t init;
    bool refin;
    bool refout;
    uint32_t xorout;
} crc_calc_settings_t;



extern const crc_calc_settings_t crc_16_modbus;
extern const crc_calc_settings_t crc_hw_32_posix;


extern uint32_t crc_hw_clac(const crc_calc_settings_t *crc_settings, const uint8_t *data, uint32_t data_qty);
extern uint32_t crc_hw_continue_clac(const crc_calc_settings_t *crc_settings, uint32_t init, const uint8_t *data, uint32_t data_qty);

extern uint32_t crc_sw_clac(const crc_calc_settings_t *crc_settings, const uint8_t *data, uint32_t data_qty);
extern uint32_t crc_sw_continue_clac(const crc_calc_settings_t *crc_settings, uint32_t init, const uint8_t *data, uint32_t data_qty);


#endif // _CRC_CALC_H_
