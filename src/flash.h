//  ***************************************************************************
/// @file    flash.h
/// @brief   Higher FLASH functions
//  ***************************************************************************
#ifndef _FLASH_H_
#define _FLASH_H_

#include <stdint.h>
#include <stdbool.h>
#include "int_flash_driver.h"


#define FLASH_REAL_BASE_PAGE_ADDRESS (0x08007C00)
#define FLASH_PAGES_QTY              (1)

#define FLASH_CRC16_ADDR             ((INT_FLASH_PAGE_SIZE * FLASH_PAGES_QTY) - 2)
#define FLASH_SIZE                   (FLASH_CRC16_ADDR)


extern bool flash_init(void);
extern bool flash_update_crc(void);
extern bool flash_erase(void);

extern bool flash_write_u16(uint32_t address, uint16_t data);
extern bool flash_write_u32(uint32_t address, uint32_t data);
extern bool flash_write_bytes(uint32_t address, const uint8_t *data, uint32_t data_size);

extern bool flash_read_u16(uint32_t address, uint16_t *data);
extern bool flash_read_u32(uint32_t address, uint32_t *data);
extern bool flash_read_bytes(uint32_t address, uint8_t *data, uint32_t data_size);


#endif // _FLASH_H_
