//  ***************************************************************************
/// @file    ring_buff.h
/// @brief   Ring buffer
//  ***************************************************************************

#ifndef _RING_BUFF_H_
#define _RING_BUFF_H_

#include <stdint.h>
#include <stdbool.h>
#include "common/error.h"


typedef struct {
    uint32_t size;
    uint8_t *buff_data;
    uint32_t read_index;
    uint32_t write_index;
    bool is_overflow;
} ring_buff_t;


static error_t ring_buff_init(ring_buff_t *buff, uint8_t *buff_data, uint32_t buff_data_size);
static error_t ring_buff_read(ring_buff_t *buff, uint8_t *data, bool *is_last_data_byte);
static error_t ring_buff_read_block(ring_buff_t *buff, uint8_t *data, uint32_t max_data_size, uint32_t *actual_data_size);
static error_t ring_buff_write(ring_buff_t *buff, uint8_t data);
static error_t ring_buff_write_block(ring_buff_t *buff, const uint8_t *data, uint32_t data_size);


#ednif    // _RING_BUFF_H_
