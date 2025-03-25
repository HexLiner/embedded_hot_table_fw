//  ***************************************************************************
/// @file    ring_buff.c
//  ***************************************************************************
#include "ring_buff.h"
#include <string.h>
#include <stdint.h>
#include <stdbool.h>
#include "common/error.h"


//  ***************************************************************************
/// @brief      Init ring buffer
/// @param      buff - pointer, can't be NULL
/// @param      buff_data - pointer, can't be NULL
/// @param      buff_data_size
/// @retval     none
/// @return     @ref error_t
//  ***************************************************************************
error_t ring_buff_init(ring_buff_t *buff, uint8_t *buff_data, uint32_t buff_data_size) {
    buff->size = buff_data_size;
    buff->buff_data = buff_data;
    buff->read_index = 0;
    buff->write_index = 0;
    buff->is_overflow = false;
    return E_OK;
}


//  ***************************************************************************
/// @brief      Read byte from ring buffer
/// @param      buff - pointer, can't be NULL
/// @param      data - pointer, can be NULL
/// @param      is_last_data_byte - pointer, can be NULL
/// @retval     data
/// @retval     is_last_data_byte
/// @return     @ref error_t
//  ***************************************************************************
error_t ring_buff_read(ring_buff_t *buff, uint8_t *data, bool *is_last_data_byte) {
    if ((buff->read_index == buff->write_index) && !buff->is_overflow) return E_NO_DATA;
    if (data != NULL) *data = buff->buff_data[buff->read_index];
    buff->read_index++;
    if (buff->read_index >= buff->size) buff->read_index = 0;
    buff->is_overflow = false;
    if (is_last_data_byte != NULL) {
        if (buff->read_index == buff->write_index) *is_last_data_byte = true;
        else *is_last_data_byte = false;
    }
    return E_OK;
}


//  ***************************************************************************
/// @brief      Read block from ring buffer
/// @param      buff - pointer, can't be NULL
/// @param      data - pointer, can be NULL
/// @param      max_data_size
/// @param      actual_data_size - pointer, can be NULL
/// @retval     data
/// @retval     actual_data_size
/// @return     @ref error_t
//  ***************************************************************************
error_t ring_buff_read_block(ring_buff_t *buff, uint8_t *data, uint32_t max_data_size, uint32_t *actual_data_size) {
    uint32_t first_part_size, seond_part_size;


    if ((buff->read_index == buff->write_index) && !buff->is_overflow) {
        if (actual_data_size != NULL) *actual_data_size = 0;
        return E_NO_DATA;
    }
    if (max_data_size == 0) return E_OK;

    if (buff->read_index >= buff->write_index) {
        first_part_size = buff->size - buff->read_index;
        seond_part_size = buff->write_index;
    }
    else {
        first_part_size = buff->write_index - buff->read_index;
        seond_part_size = 0;
    }

    if (first_part_size > max_data_size) first_part_size = max_data_size;
    max_data_size -= first_part_size;
    if (seond_part_size > max_data_size) seond_part_size = max_data_size;

    if (data != NULL) memcpy(&data[0], &buff->buff_data[buff->read_index], first_part_size);
    buff->read_index += first_part_size;
    if (buff->read_index >= buff->size) buff->read_index = 0;
    if ((seond_part_size != 0) && (data != NULL)) memcpy(&data[first_part_size], &buff->buff_data[0], seond_part_size);
    buff->read_index += seond_part_size;

    if (actual_data_size != NULL) *actual_data_size = first_part_size + seond_part_size;
    buff->is_overflow = false;
    return E_OK;
}


//  ***************************************************************************
/// @brief      Write byte to ring buffer
/// @param      buff - pointer, can't be NULL
/// @param      data
/// @retval     none
/// @return     @ref error_t
//  ***************************************************************************
error_t ring_buff_write(ring_buff_t *buff, uint8_t data) {
    if (buff->is_overflow) return E_OUT_OF_MEMORY;

    buff->buff_data[buff->write_index] = data;
    buff->write_index++;
    if (buff->write_index >= buff->size) buff->write_index = 0;

    if (buff->write_index == buff->read_index) {
        buff->is_overflow = true;
    }

    return E_OK;
}


//  ***************************************************************************
/// @brief      Write block to ring buffer
/// @param      buff - pointer, can't be NULL
/// @param      data
/// @param      data_size
/// @retval     none
/// @return     @ref error_t
//  ***************************************************************************
error_t ring_buff_write_block(ring_buff_t *buff, const uint8_t *data, uint32_t data_size) {
    uint32_t check_index;
    uint32_t first_part_size, seond_part_size;


    if (buff->is_overflow) return E_OUT_OF_MEMORY;
    check_index = buff->read_index;
    if (buff->write_index >= buff->read_index) check_index += buff->size;
    if ((buff->write_index + data_size) > check_index) return E_OUT_OF_MEMORY;

    if (buff->write_index >= buff->read_index) {
        first_part_size = buff->size - buff->write_index;
        seond_part_size = buff->read_index;
    }
    else {
        first_part_size = buff->read_index - buff->write_index;
        seond_part_size = 0;
    }

    if (first_part_size > data_size) first_part_size = data_size;
    data_size -= first_part_size;
    seond_part_size = data_size;

    memcpy(&buff->buff_data[buff->write_index], &data[0], first_part_size);
    buff->write_index += first_part_size;
    if (buff->write_index >= buff->size) buff->write_index = 0;
    if (seond_part_size != 0) memcpy(&buff->buff_data[0], &data[first_part_size], seond_part_size);
    buff->write_index += seond_part_size;

    if (buff->write_index == buff->read_index) buff->is_overflow = true;

    return E_OK;
}


void ring_buff_get_read_pos(ring_buff_t *buff, const uint8_t **data_ptr, uint32_t *data_size) {
    if (data_size != NULL){
        if ((buff->read_index == buff->write_index) && !buff->is_overflow) {
            *data_size = 0;
        }
        else if (buff->read_index >= buff->write_index) {
            *data_size = buff->size - buff->read_index;
        }
        else {
            *data_size = buff->write_index - buff->read_index;
        }
    }

    *data_ptr = &buff->buff_data[buff->read_index];
}


void ring_buff_clear(ring_buff_t *buff, uint32_t clear_size) {
    ring_buff_read_block(buff, NULL, clear_size, NULL); 
}
