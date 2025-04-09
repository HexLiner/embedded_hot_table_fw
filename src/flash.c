//  ***************************************************************************
/// @file    flash.c
//  ***************************************************************************
#include "flash.h"


bool flash_init(void) {
    uint8_t *flash_page_ptr = (uint8_t*)FLASH_REAL_BASE_PAGE_ADDRESS;
    uint32_t crc32_calc, crc32_real;


    crc32_calc = crc_hw_clac(&crc_hw_32_posix, flash_page_ptr, FLASH_SIZE_FOR_CRC);
    flash_read_u32(FLASH_CRC32_ADDR, &crc32_real);
    if (crc32_calc != crc32_real) {
        eh_set_fail_cfg_error();
        return false;
    }

    return true;
}


bool flash_update_crc(void) {
    uint8_t *flash_page_ptr = (uint8_t*)FLASH_REAL_BASE_PAGE_ADDRESS;
    uint32_t crc32_calc;


    crc32_calc = crc_hw_clac(&crc_hw_32_posix, flash_page_ptr, FLASH_SIZE_FOR_CRC);
    flash_write_u32(FLASH_CRC32_ADDR, crc32_calc);
    return true;
}


bool flash_erase(void) {
    uint32_t i, addr;


    addr = FLASH_REAL_BASE_PAGE_ADDRESS;
    for (i = 0; i < FLASH_PAGES_QTY; i++) {
        if (!int_flash_driver_erase_page(addr)) return false;
        addr += INT_FLASH_PAGE_SIZE;
    }
    return true;
}


bool flash_write_u16(uint32_t address, uint16_t data) {
    return flash_write_bytes(address, (uint8_t*)&data, 2);
}


bool flash_write_u32(uint32_t address, uint32_t data) {
    return flash_write_bytes(address, (uint8_t*)&data, 4);
}


bool flash_write_bytes(uint32_t address, const uint8_t *data, uint32_t data_size) {
    if ((address + data_size) > FLASH_SIZE) return false;
    address += FLASH_REAL_BASE_PAGE_ADDRESS;
    return int_flash_driver_write_bytes(address, data, data_size);
}


bool flash_read_u8(uint32_t address, uint16_t *data) {
    return flash_read_bytes(address, (uint8_t*)data, 1);
}


bool flash_read_u16(uint32_t address, uint16_t *data) {
    return flash_read_bytes(address, (uint8_t*)data, 2);
}


bool flash_read_u32(uint32_t address, uint32_t *data) {
    return flash_read_bytes(address, (uint8_t*)data, 4);
}


bool flash_read_bytes(uint32_t address, uint8_t *data, uint32_t data_size) {
    if ((address + data_size) > FLASH_SIZE) return false;
    address += FLASH_REAL_BASE_PAGE_ADDRESS;
    int_flash_driver_read_bytes(address, data, data_size);
    return true;
}
