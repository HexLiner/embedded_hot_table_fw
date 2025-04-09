//  ***************************************************************************
/// @file    crc_calc.c
//  ***************************************************************************
#include "crc_calc.h"



const crc_calc_settings_t crc_16_modbus = {
    .resolution = 16,
    .poly = 0x8005,
    .init = 0xffff,
    .refin = true,
    .refout = true,
    .xorout = 0x0,
};

const crc_calc_settings_t crc_hw_32_posix = {
    .resolution = 32,    // fixed value for HW CRC module
    .poly = 0x4c11db7,   // fixed value for HW CRC module
    .init = 0x0,
    .refin = false,
    .refout = false,
    .xorout = 0xffffffff,
};



uint32_t crc_hw_clac(const crc_calc_settings_t *crc_settings, const uint8_t *data, uint32_t data_qty) {
    return crc_hw_continue_clac(crc_settings, crc_settings->init, data, data_qty);
}


uint32_t crc_hw_continue_clac(const crc_calc_settings_t *crc_settings, uint32_t init, const uint8_t *data, uint32_t data_qty) {
    uint8_t rev_in;
    uint32_t crc;
    uint32_t crc_byte;
    uint8_t *crc_dr_u8 = (uint8_t*)(&CRC->DR);     // need for 8 bit input data
    // uint8_t *crc_dr_u16 = (uint16_t*)(&CRC->DR);     // need for 16 bit input data
    // uint8_t *crc_dr_u32 = (uint32_t*)(&CRC->DR);     // need for 32 bit input data


    rev_in = 0;
    if (crc_settings->refin) {
        rev_in = 1;   // need for 8 bit input data
        // rev_in = 2;   // need for 16 bit input data
        // rev_in = 3;   // need for 32 bit input data
    }    
    RCC->AHBENR |= RCC_AHBENR_CRCEN;
    CRC->CR = (1 << CRC_CR_RESET_Pos) |
              (rev_in << CRC_CR_REV_IN_Pos) |
              (crc_settings->refout << CRC_CR_REV_OUT_Pos);
    CRC->INIT = init;


    for (crc_byte = 0; crc_byte < data_qty; crc_byte++) {
        *crc_dr_u8 = data[crc_byte];
    }
    crc = CRC->DR;


    RCC->AHBENR &= ~RCC_AHBENR_CRCEN;

    crc ^= crc_settings->xorout;
    return crc;
}


uint32_t crc_sw_clac(const crc_calc_settings_t *crc_settings, const uint8_t *data, uint32_t data_qty) {
    return crc_sw_continue_clac(crc_settings, crc_settings->init, data, data_qty);
}


uint32_t crc_sw_continue_clac(const crc_calc_settings_t *crc_settings, uint32_t init, const uint8_t *data, uint32_t data_qty) {
    uint32_t crc;
    uint32_t data_index, crc_bit;
    uint32_t i;
    uint32_t poly_norm, init_norm;


    if (crc_settings->refout) {
        // Poly mirrored
        poly_norm = 0;
        for (i = 0; i < crc_settings->resolution; i++) {
            poly_norm = poly_norm << 1;
            poly_norm |= (crc_settings->poly >> i) & 0x1;
        }

        // Init mirrored
        init_norm = 0;
        for (i = 0; i < crc_settings->resolution; i++) {
            init_norm = init_norm << 1;
            init_norm |= (init >> i) & 0x1;
        }
    }
    else {
        poly_norm = crc_settings->poly;
        init_norm = init;
    }


    crc = init_norm;
    if (crc_settings->refin) {
        for (data_index = 0; data_index < data_qty; data_index++) {
            crc ^= (uint32_t)data[data_index];
            for (crc_bit = 0; crc_bit < 8; crc_bit++) {
                if (crc & 0x01) {
                    crc = (crc >> 1) ^ poly_norm;
                }
                else {
                    crc = (crc >> 1);
                }
            }
        }
    }
    else {
        for (data_index = 0; data_index < data_qty; data_index++) {
            crc ^= (uint32_t)data[data_index] << (crc_settings->resolution - 8);
            for (crc_bit = 0; crc_bit < 8; crc_bit++) {
                if (crc & (1 << (crc_settings->resolution - 1))) {
                    crc = (crc << 1) ^ poly_norm;
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
