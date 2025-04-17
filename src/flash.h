//  ***************************************************************************
/// @file    flash.h
/// @brief   Higher FLASH functions
//  ***************************************************************************
#ifndef _FLASH_H_
#define _FLASH_H_

#include <stdint.h>
#include <stdbool.h>
#include "hal/int_flash_driver.h"
#include "common/crc_calc.h"
#include "error_handling.h"


#define FLASH_REAL_BASE_PAGE_ADDRESS (0x08007C00)
#define FLASH_PAGES_QTY              (1)

#define FLASH_SIZE                   (INT_FLASH_PAGE_SIZE * FLASH_PAGES_QTY)
#define FLASH_SIZE_FOR_CRC           (FLASH_SIZE - 4)
#define FLASH_CRC32_ADDR             (FLASH_SIZE - 4)


extern bool flash_init(void);
extern bool flash_update_crc(void);
extern bool flash_erase(void);

extern bool flash_write_u16(uint32_t address, uint16_t data);
extern bool flash_write_u32(uint32_t address, uint32_t data);
extern bool flash_write_bytes(uint32_t address, const uint8_t *data, uint32_t data_size);

extern bool flash_read_u8(uint32_t address, uint16_t *data);
extern bool flash_read_u16(uint32_t address, uint16_t *data);
extern bool flash_read_u32(uint32_t address, uint32_t *data);
extern bool flash_read_bytes(uint32_t address, uint8_t *data, uint32_t data_size);


#endif // _FLASH_H_
