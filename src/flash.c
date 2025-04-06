//  ***************************************************************************
/// @file    flash.c
//  ***************************************************************************
#include "flash.h"


bool flash_init(void) {
    //// check CRC
    return true;
}


bool flash_update_crc(void) {
    ////
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


bool flash_read_u16(uint32_t address, uint16_t *data) {
    return flash_read_bytes(address, (uint8_t*)data, 2);
}


bool flash_read_u32(uint32_t address, uint32_t *data) {
    return flash_read_bytes(address, (uint8_t*)data, 4);
}


bool flash_read_bytes(uint32_t address, uint8_t *data, uint32_t data_size) {
    if ((address + data_size) > FLASH_SIZE) return false;
    address += FLASH_REAL_BASE_PAGE_ADDRESS;
    return int_flash_driver_read_bytes(address, data, data_size);
}
