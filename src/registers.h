//  ***************************************************************************
/// @file    registers.h
/// @brief   Device_registers
//  ***************************************************************************
#ifndef _REGISTERS_H_
#define _REGISTERS_H_

#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>



#define RG_RAM_REGS_ADDR_OFFSET        (0)  // m.b. const 0!
#define RG_RAM_RO_REGS_ADDR_OFFSET     (RG_RAM_REGS_ADDR_OFFSET)
#define RG_RAM_RO_REGS_QTY             (7)
#define RG_RAM_RW_REGS_ADDR_OFFSET     (RG_RAM_RO_REGS_ADDR_OFFSET + RG_RAM_RO_REGS_QTY)
#define RG_RAM_RW_REGS_QTY             (1)
#define RG_FLASH_REGS_ADDR_OFFSET      (RG_RAM_REGS_ADDR_OFFSET + RG_RAM_RW_REGS_ADDR_OFFSET + RG_RAM_RW_REGS_QTY)
#define RG_FLASH_RO_REGS_ADDR_OFFSET   (RG_FLASH_REGS_ADDR_OFFSET)
#define RG_FLASH_RO_REGS_QTY           (0)
#define RG_FLASH_RW_REGS_ADDR_OFFSET   (RG_FLASH_RO_REGS_ADDR_OFFSET + RG_FLASH_RO_REGS_QTY)
#define RG_FLASH_RW_REGS_QTY           (540)

#define RG_RAM_REGS_QTY                (RG_RAM_RO_REGS_QTY + RG_RAM_RW_REGS_QTY)
#define RG_FLASH_REGS_QTY              (RG_FLASH_RO_REGS_QTY + RG_FLASH_RW_REGS_QTY)
#define RG_MAX_REG_ADDR                (RG_FLASH_RW_REGS_ADDR_OFFSET + RG_FLASH_RW_REGS_QTY)



#define RG_RAM_RO_REG_MEMORY_MAP_VERSION             (0)
#define RG_RAM_RO_REG_DEVIE_ID_0                     (1)
#define RG_RAM_RO_REG_DEVIE_ID_1                     (2)
#define RG_RAM_RO_REG_DEVIE_VER_MINOR                (3)
#define RG_RAM_RO_REG_DEVIE_VER_MAJOR                (4)
#define RG_RAM_RO_REG_FAIL_CODE                      (5)
#define RG_RAM_RO_REG_WARN_CODE                      (6)

#define RG_RAM_RW_REG_CMD                            (7)

// Flash
#define RG_PROFILES_BASE_ADDR                        (8)
    #define RG_PROFILE_NAME_SIZE                         (18)
        #define RG_PROFILE_STAGE_TEMPERATURE_C_SIZE          (2)
        #define RG_PROFILE_STAGE_DURATION_S_SIZE             (4)
        #define RG_PROFILE_STAGE_FUN_PERIOD_S_SIZE           (4)
        #define RG_PROFILE_STAGE_FUN_DUTY_CYCLE_PCT_SIZE     (2)
    #define RG_PROFILE_STAGE_SIZE                        (RG_PROFILE_STAGE_TEMPERATURE_C_SIZE + RG_PROFILE_STAGE_DURATION_S_SIZE + RG_PROFILE_STAGE_FUN_PERIOD_S_SIZE + RG_PROFILE_STAGE_FUN_DUTY_CYCLE_PCT_SIZE)
    #define RG_PROFILE_STAGES_QTY                        (3)
#define RG_PROFILE_SIZE                               (RG_PROFILE_NAME_SIZE + (RG_PROFILE_STAGE_SIZE * RG_PROFILE_STAGES_QTY))
#define RG_PROFILES_QTY                               (10)




#define RG_CMD_NOP                 (0)
#define RG_CMD_REBOOT              (1)
#define RG_CMD_CLEAR_FLASH         (2)
#define RG_CMD_UPDATE_FLASH_CRC    (3)




extern uint16_t registers_ram[RG_RAM_REGS_QTY];


extern void regs_init(void);
extern void regs_process(void);

extern bool regs_read_reg(uint32_t address, uint16_t *reg_value);
extern bool regs_read_regs(uint32_t address, uint32_t regs_qty, uint16_t *regs_values);
extern bool regs_write_reg(uint32_t address, uint16_t reg_value);
extern bool regs_write_regs(uint32_t address, uint32_t regs_qty, const uint16_t *regs_values);


#endif   // _REGISTERS_H_
