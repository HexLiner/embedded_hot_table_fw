//  ***************************************************************************
/// @file    registers.c
//  ***************************************************************************
#include "registers.h"
#include "common/mcu.h"
#include "error_handling.h"
#include "flash.h"


uint16_t registers_ram[RG_RAM_REGS_QTY];




void regs_init(void) {
    registers_ram[RG_RAM_RO_REG_MEMORY_MAP_VERSION] = 0x0001;
    registers_ram[RG_RAM_RO_REG_DEVIE_ID_0] = 0x0001;
    registers_ram[RG_RAM_RO_REG_DEVIE_ID_1] = 0x0000;
    registers_ram[RG_RAM_RO_REG_DEVIE_VER_MINOR] = 0x0001;
    registers_ram[RG_RAM_RO_REG_DEVIE_VER_MAJOR] = 0x0001;
    registers_ram[RG_RAM_RO_REG_FAIL_CODE] = 0x0000;
    registers_ram[RG_RAM_RO_REG_WARN_CODE] = 0x0000;

    registers_ram[RG_RAM_RW_REG_CMD] = 0x0000;
}


void regs_process(void) {
    registers_ram[RG_RAM_RO_REG_FAIL_CODE] = fail_code;
    registers_ram[RG_RAM_RO_REG_WARN_CODE] = warning_code;

    switch (registers_ram[RG_RAM_RW_REG_CMD]) {
        case RG_CMD_REBOOT:
            NVIC_SystemReset();
            break;

        case RG_CMD_CLEAR_FLASH:
            flash_erase();
            break;

        case RG_CMD_UPDATE_FLASH_CRC:
            flash_update_crc();
            break;

        default:
            break;
    }
    registers_ram[RG_RAM_RW_REG_CMD] = 0;
}


bool regs_read_reg(uint32_t address, uint16_t *reg_value) {
    uint32_t flash_addr;


    if (address > RG_MAX_REG_ADDR) return false;

    if (address < RG_FLASH_REGS_ADDR_OFFSET) {
        *reg_value = registers_ram[address];
    }
    else {
        flash_addr = (address - RG_FLASH_REGS_ADDR_OFFSET) * 2;
        return flash_read_u16(flash_addr, reg_value);
    }
    return true;
}


bool regs_read_regs(uint32_t address, uint32_t regs_qty, uint16_t *regs_values) {
    uint32_t buff_index;


    if ((address + regs_qty) > RG_MAX_REG_ADDR) return false;

    for (buff_index = 0; buff_index < regs_qty; buff_index++) {
        if (!regs_read_reg(address, &regs_values[buff_index])) return false;
        address++;
    }
    return true;
}


bool regs_write_reg(uint32_t address, uint16_t reg_value) {
    uint32_t flash_addr;


    if (address > RG_MAX_REG_ADDR) return false;
    if (address < RG_RAM_RW_REGS_ADDR_OFFSET) return false;  // RAM RO space
    if ((address > RG_RAM_RW_REGS_ADDR_OFFSET) && (address < RG_FLASH_RW_REGS_ADDR_OFFSET)) return false;  // Flash RO space

    if (address < RG_FLASH_REGS_ADDR_OFFSET) {
        registers_ram[address] = reg_value;
    }
    else {
        flash_addr = (address - RG_FLASH_REGS_ADDR_OFFSET) * 2;
        return flash_write_u16(flash_addr, reg_value);
    }
    return true;
}


bool regs_write_regs(uint32_t address, uint32_t regs_qty, const uint16_t *regs_values) {
    uint32_t buff_index;


    if ((address + regs_qty) > RG_MAX_REG_ADDR) return false;
    if (address < RG_RAM_RW_REGS_ADDR_OFFSET) return false;  // RAM RO space included
    if ((address > RG_RAM_RW_REGS_ADDR_OFFSET) && (address < RG_FLASH_RW_REGS_ADDR_OFFSET)) return false;  // Flash RO space included
    if ((address < RG_FLASH_RO_REGS_ADDR_OFFSET) && ((address + regs_qty) > RG_FLASH_RO_REGS_ADDR_OFFSET)) return false;  // Flash RO space included

    for (buff_index = 0; buff_index < regs_qty; buff_index++) {
        if (!regs_write_reg(address, regs_values[buff_index])) return false;
        address++;
    }
    return true;
}
