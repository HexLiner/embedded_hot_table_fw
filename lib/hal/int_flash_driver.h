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


#define INT_FLASH_PAGE_SIZE (1024)


extern bool int_flash_driver_enable_protect(void);

extern __ramfunc bool int_flash_driver_erase_page(uint32_t address);
extern __ramfunc bool int_flash_driver_write_bytes(uint32_t address, const uint8_t *data, uint32_t data_size);
extern void int_flash_driver_read_bytes(uint32_t address, uint8_t *data, uint32_t data_size);


#endif // _INT_FLASH_DRIVER_H_
