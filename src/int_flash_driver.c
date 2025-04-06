//  ***************************************************************************
/// @file    int_flash_driver.c
//  ***************************************************************************
#include "int_flash_driver.h"


#define WAIT_LOOP_INTERATION_COUNT (30000 * 3)   ////


static __ramfunc void flash_lock(void);
static __ramfunc void flash_unlock(void);
static __ramfunc bool flash_wait_operation_complete(void);
static __ramfunc bool flash_write_word(uint32_t address, uint16_t word);




//  ***************************************************************************
/// @brief  Enable FLASH protection
/// @note   Activate protection level 1
/// @param  none
/// @return true - success, false - fail
//  ***************************************************************************
bool int_flash_driver_enable_protect(void) {
    /*
    uint16_t* rdp_option_byte_address = (uint16_t*)OB_BASE;
    uint16_t rdp_option_byte_value = *rdp_option_byte_address;
    
    // Check flash protection state
    if ((rdp_option_byte_value & 0xFF) != 0xAA) {
        return true; // Flash memory already protected
    }
    
    
    uint32_t irq_state = __get_interrupt_state();
    __disable_interrupt();
    flash_unlock();
    
    bool result = false;
    do {
        // Unlock write option bytes
        FLASH->OPTKEYR = FLASH_OPTKEY1;
        FLASH->OPTKEYR = FLASH_OPTKEY2;
        
        // Erase option bytes
        FLASH->CR |= FLASH_CR_OPTWRE;
        FLASH->CR |= FLASH_CR_OPTER;
        FLASH->CR |= FLASH_CR_STRT;
        
        result = flash_wait_operation_complete();
        FLASH->CR &= ~FLASH_CR_OPTER;
        if (result == false) {
            break;
        }
        
        // Verify erase operation
        rdp_option_byte_value = *rdp_option_byte_address;
        result = (rdp_option_byte_value == 0xFFFF);
        if (result == false) {
            break;
        }
        
        // Write new option byte value
        FLASH->CR |= FLASH_CR_OPTWRE;
        FLASH->CR |= FLASH_CR_OPTPG;
        
        (*rdp_option_byte_address) = 0x0022;
        result = flash_wait_operation_complete();
        FLASH->CR &= ~FLASH_CR_OPTPG;
        if (result == false) {
            break;
        }
        
        // Verify write operation
        rdp_option_byte_value = *rdp_option_byte_address;
        result = ((rdp_option_byte_value & 0xFF) == 0x22);
    
    } while (0);
    
    
    flash_lock();
    __set_interrupt_state(irq_state);

    return result;
    */

    return false;
}


//  ***************************************************************************
/// @brief  Erase FLASH page
/// @param  address
/// @return true - success, false - fail
//  ***************************************************************************
__ramfunc bool int_flash_driver_erase_page(uint32_t address) {
    flash_unlock();

    // Erase page
    FLASH->CR |= FLASH_CR_PER;
    FLASH->AR = address;
    FLASH->CR |= FLASH_CR_STRT;

    // Wait operation complete
    bool result = flash_wait_operation_complete();
    FLASH->CR &= ~FLASH_CR_PER;

    flash_lock();

    return result;
}


__ramfunc bool int_flash_driver_write_bytes(uint32_t address, const uint8_t *buffer, uint32_t size) {
    int32_t buff_index;
    uint16_t word;


    if ((address & 0x01) || (size & 0x01)) return false;
    

    buff_index = 0;
    while (size != 0) {
        word = buffer[buff_index + 1];
        word = word << 8;
        word |= buffer[buff_index];
        size -= 2;
        buff_index += 2;

        if (!flash_write_word(address, word)) return false;
        address += 2;
    }

    return true;
}


//  ***************************************************************************
/// @brief  Read data from FLASH
/// @param  address
/// @param  buffer
/// @param  size
/// @retval buffer
/// @return true - success, false - fail
//  ***************************************************************************
bool int_flash_driver_read_bytes(uint32_t address, uint8_t *buffer, uint32_t size) {
    if ((address & 0x01) || (size & 0x01)) return false;
    memcpy(buffer, (uint8_t*)(address), size);
    return true;
}




//  ***************************************************************************
/// @brief  Lock FLASH for write operation
/// @param  none
/// @return none
//  ***************************************************************************
static __ramfunc void flash_lock(void) {
    FLASH->CR |= FLASH_CR_LOCK;
}


//  ***************************************************************************
/// @brief  Unlock FLASH for write operation
/// @param  none
/// @return none
//  ***************************************************************************
static __ramfunc void flash_unlock(void) {
    FLASH->KEYR = FLASH_OPTKEY1;
    FLASH->KEYR = FLASH_OPTKEY2;
}


//  ***************************************************************************
/// @brief  Wait current operation complete
/// @param  none
/// @return true - success, false - fail
//  ***************************************************************************
static __ramfunc bool flash_wait_operation_complete(void) {
    // Wait operation complete
    for (uint32_t i = 0; FLASH->SR & FLASH_SR_BSY; ++i) {
        if (i > WAIT_LOOP_INTERATION_COUNT) {
            return false;
        }
    }

    // Wait result bits
    uint32_t status_mask = FLASH_SR_EOP | FLASH_SR_WRPERR | FLASH_SR_PGERR;
    for (uint32_t i = 0; (FLASH->SR & status_mask) == 0; ++i) {
        if (i > WAIT_LOOP_INTERATION_COUNT) {
            return false;
        }
    }

    // Check errors
    if (FLASH->SR & (FLASH_SR_WRPERR | FLASH_SR_PGERR)) {
        // Reset all result bits
        FLASH->SR |= FLASH_SR_EOP | FLASH_SR_WRPERR | FLASH_SR_PGERR;
        return false;
    }

    // Reset all result bits
    FLASH->SR |= FLASH_SR_EOP | FLASH_SR_WRPERR | FLASH_SR_PGERR;
    return true;
}


//  ***************************************************************************
/// @brief  Write word to FLASH
/// @param  address
/// @param  word
/// @return true - success, false - fail
//  ***************************************************************************
static __ramfunc bool flash_write_word(uint32_t address, uint16_t word) {
    bool result;


    flash_unlock();
    // Write word
    FLASH->CR |= FLASH_CR_PG;
    *((uint16_t*)address) = word;
    // Wait operation complete
    result = flash_wait_operation_complete();
    FLASH->CR &= ~FLASH_CR_PG;
    flash_lock();
    if (!result) return false;
    
    // Check written data
    uint16_t *read_word = (uint16_t*)address;
    return *read_word == word;
}
