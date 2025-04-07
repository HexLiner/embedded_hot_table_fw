//  ***************************************************************************
/// @file    crc_calc.c
//  ***************************************************************************
#include "crc_calc.h"



uint32_t crc_hw_clac(const uint8_t *data, uint32_t data_qty, crc_calc_settings_t *crc_settings) {
    return crc_hw_continue_clac(crc_settings->init, data, data_qty, crc_settings);
}


uint32_t crc_hw_continue_clac(uint32_t init, const uint8_t *data, uint32_t data_qty, crc_calc_settings_t *crc_settings) {
    uint8_t rev_in = 0;
    uint32_t crc;
    uint32_t crc_byte;
    
    uint8_t *ptr = (uint8_t*)(&CRC->DR);


    // 00: Bit order not affected
    // 01: Bit reversal done by byte
    // 10: Bit reversal done by half-word
    // 11: Bit reversal done by word
    if (crc_settings->refin) {
        if (crc_settings->resolution <= 8) rev_in = 1;
        else if (crc_settings->resolution <= 16) rev_in = 2;
        else if (crc_settings->resolution <= 32) rev_in = 3;
    }    

    RCC->AHBENR |= RCC_AHBENR_CRCEN;
    CRC->CR = (1 << CRC_CR_RESET_Pos) |
              (rev_in << CRC_CR_REV_IN_Pos) |
              (0 << CRC_CR_REV_OUT_Pos);     //// ???
    CRC->INIT = init;

    for (crc_byte = 0; crc_byte < data_qty; crc_byte++) {
        *ptr = data[crc_byte];
    }
    crc = CRC->DR;

    RCC->AHBENR &= ~RCC_AHBENR_CRCEN;

    crc ^= crc_settings->xorout;
    return crc;
}


uint32_t crc_clac(const uint8_t *data, uint32_t data_qty, crc_calc_settings_t *crc_settings) {
    return crc_continue_clac(crc_settings->init, data, data_qty, crc_settings);
}


uint32_t crc_continue_clac(uint32_t init, const uint8_t *data, uint32_t data_qty, crc_calc_settings_t *crc_settings) {
    uint32_t crc;
    uint32_t crc_byte, crc_bit;


    crc = init;
    if (crc_settings->refin) {
        for (crc_byte = 0; crc_byte < data_qty; crc_byte++) {
            crc ^= (data[crc_byte]);
            for (crc_bit = 0; crc_bit < crc_settings->resolution; crc_bit++) {
                if (crc & 0x01) {
                    crc = (crc >> 1) ^ crc_settings->poly;
                }
                else {
                    crc = (crc >> 1);
                }
            }
        }
    }
    else {
        for (crc_byte = 0; crc_byte < data_qty; crc_byte++) {
            crc ^= (data[crc_byte]);
            for (crc_bit = 0; crc_bit < crc_settings->resolution; crc_bit++) {
                if (crc & 0x80) {
                    crc = (crc << 1) ^ crc_settings->poly;
                }
                else {
                    crc = (crc << 1);
                }
            }
        }
    }

    crc ^= crc_settings->xorout;
    return crc;
}
