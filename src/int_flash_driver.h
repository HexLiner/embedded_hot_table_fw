//  ***************************************************************************
/// @file    int_flash_driver.h
/// @brief   Internal FLASH driver
//  ***************************************************************************
#ifndef _INT_FLASH_DRIVER_H_
#define _INT_FLASH_DRIVER_H_

#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include "common/mcu.h"


#define FLASH_PAGE_SIZE (1024)


extern bool flash_enable_protect(void);
extern __ramfunc bool flash_erase_page(uint32_t flash_address);
extern __ramfunc bool flash_write_word(uint32_t flash_address, uint16_t data);
extern uint16_t flash_read_word_le(uint32_t flash_address);
extern uint16_t flash_read_word_be(uint32_t flash_address);
extern uint8_t flash_read_byte(uint32_t flash_address);



extern bool flash_write_bytes(uint32_t flash_address, const uint8_t *buffer, uint32_t size);
extern void flash_read_bytes(uint32_t flash_address, uint8_t *buffer, uint32_t size);


#endif // _INT_FLASH_DRIVER_H_
