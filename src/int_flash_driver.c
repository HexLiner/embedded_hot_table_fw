//  ***************************************************************************
/// @file    int_flash_driver.c
//  ***************************************************************************
#include "int_flash_driver.h"


#define WAIT_LOOP_INTERATION_COUNT (30000 * 3)   ////


static __ramfunc void flash_lock(void);
static __ramfunc void flash_unlock(void);
static __ramfunc bool flash_wait_operation_complete(void);




//  ***************************************************************************
/// @brief  Enable FLASH protection
/// @note   Activate protection level 1
/// @param  none
/// @return true - success, false - fail
//  ***************************************************************************
bool flash_enable_protect(void) {
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
/// @param  flash_address
/// @return true - success, false - fail
//  ***************************************************************************
__ramfunc bool flash_erase_page(uint32_t flash_address) {
    flash_unlock();

    // Erase page
    FLASH->CR |= FLASH_CR_PER;
    FLASH->AR = flash_address;
    FLASH->CR |= FLASH_CR_STRT;

    // Wait operation complete
    bool result = flash_wait_operation_complete();
    FLASH->CR &= ~FLASH_CR_PER;

    flash_lock();

    return result;
}


//  ***************************************************************************
/// @brief  Write word to FLASH
/// @note   Write little endian format (swap MSB and LSB)
/// @param  flash_address
/// @param  data
/// @return true - success, false - fail
//  ***************************************************************************
__ramfunc bool flash_write_word(uint32_t flash_address, uint16_t data) {
    bool result;


    flash_unlock();
    uint16_t word = (data << 8) | (data >> 8);
    // Write word
    FLASH->CR |= FLASH_CR_PG;
    *((uint16_t*)flash_address) = word;
    // Wait operation complete
    result = flash_wait_operation_complete();
    FLASH->CR &= ~FLASH_CR_PG;
    flash_lock();
    if (!result) return false;
    
    // Check written data
    uint8_t *pointer = (uint8_t*)flash_address;
    uint16_t read_word = (pointer[1] << 8) | pointer[0];
    
    return read_word == word;
}



__ramfunc bool flash_write_bytes(uint32_t flash_address, const uint8_t *buffer, uint32_t size) {
    int32_t buff_index;
    uint16_t word;


    buff_index = 0;
    if (flash_address & 0x01) {
        flash_address--;
        buff_index = -1;
    }

    while (size != 0) {
        size--;
        if (buff_index == -1) {
            word = 0xFF00;
            word |= buffer[0];
        }
        else if (size == 0) {
            word = buffer[buff_index];
            word = word << 8;
            word |= 0xFF;
        }
        else {
            word = buffer[buff_index];
            word = word << 8;
            word |= buffer[buff_index + 1];
            size--;
            buff_index++;
        }
        buff_index++;

        if (!flash_write_word(flash_address, word)) return false;
        flash_address += 2;
    }

    return true;
}


//  ***************************************************************************
/// @brief  Read data from FLASH
/// @param  flash_address
/// @param  buffer
/// @param  size
/// @retval buffer
/// @return none
//  ***************************************************************************
void flash_read_bytes(uint32_t flash_address, uint8_t *buffer, uint32_t size) {
    memcpy(buffer, (uint8_t*)(flash_address), size);
}


//  ***************************************************************************
/// @brief  Read single byte from FLASH
/// @param  flash_address
/// @return byte value
//  ***************************************************************************
uint8_t flash_read_byte(uint32_t flash_address) {
    uint8_t *pointer = (uint8_t*)flash_address;
    return pointer[0];
}


//  ***************************************************************************
/// @brief  Read single word from FLASH
/// @note   Value returned in little endian format
/// @param  flash_address
/// @return word value
//  ***************************************************************************
uint16_t flash_read_word_le(uint32_t flash_address) {
    uint8_t *pointer = (uint8_t*)flash_address;
    return (pointer[0] << 8) | pointer[1];
}


//  ***************************************************************************
/// @brief  Read single word from FLASH
/// @note   Value returned in big endian format
/// @param  flash_address
/// @return word value
//  ***************************************************************************
uint16_t flash_read_word_be(uint32_t flash_address) {
    uint8_t *pointer = (uint8_t*)flash_address;
    return (pointer[1] << 8) | pointer[0];
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
