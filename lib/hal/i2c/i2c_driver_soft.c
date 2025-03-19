//  ***************************************************************************
/// @file    i2c_driver_stm32f0.h
/// @author  Techart Micro Systems (ES, DM)
//  ***************************************************************************
#include "general/hal/i2c/i2c_driver.h"
#include <string.h>
#include <stdlib.h>
#include "general/hal/i2c/i2c_hwdef_stm32.h"
#include "general/hal/systimer/systimer.h"
#include "general/hal/sysclk/sysclk.h"
#include "general/hal/gpio/gpio.h"



#define I2C_COUNT (1)


typedef enum {
    // Flags
    I2C_BUSY_FLAG         = 0x80, // Busy flag - transaction is in process now
    // OFF modes
    I2C_OFF               = 0x00, // Off - I2C is inactive
    // MASTER modes
    I2C_MASTER_IDLE       = 0x10, // Master - idle, no active transactions
    I2C_MASTER_TX_START   = 0x91, // Master - start for transmit (wait for SLA ACKed/NACKed)
    I2C_MASTER_TX_PROCESS = 0x92, // Master - data transmission
    I2C_MASTER_RX_START   = 0x93, // Master - start for receive (wait for SLA ACKed/NACKed)
    I2C_MASTER_RX_PROCESS = 0x94, // Master - data reception
    I2C_MASTER_STOP       = 0x95, // Master - wait for stop
    // SLAVE modes
    I2C_SLAVE_IDLE        = 0x20, // Slave - idle, no active transactions
    I2C_SLAVE_RX          = 0xA1, // Slave - addressed with read
    I2C_SLAVE_TX          = 0xA2, // Slave - addressed with write
} i2c_state_t;


typedef struct {
    volatile i2c_state_t       state;
    uint8_t                    slave_address;
    uint8_t                    data_cnt;
    volatile i2c_transaction_t *transaction;
    error_t                    transaction_result;
    volatile uint8_t           *data_ptr;

    i2c_speed_t                speed_hz;
    gpio_pin_t                 scl_pin;
    gpio_pin_t                 sda_pin;
} i2c_internal_t;


static error_t i2c_enable(i2c_internal_t *i2c);
static error_t i2c_disable(i2c_internal_t *i2c);




//  ***************************************************************************
/// @brief   Initializes I2C software unit and hardware peripherals
/// @param   handle - pointer to I2C handle
/// @param   i2c_hw - I2C_TypeDef structure defining I2C hardware
/// @param   i2c_settings_t
/// @retval  handle
/// @return  @ref error_t
//  ***************************************************************************
error_t i2c_init(handle_t *handle, void *hwid, const i2c_settings_t *settings) {
    *handle = malloc(sizeof(i2c_internal_t));
    if (*handle == NULL) return E_SOFTWARE_FLAG | E_SOURCE_I2C | E_OUT_OF_MEMORY;
    i2c_list[i2c_id] = i2c;
    handle->state         = I2C_MASTER_IDLE;
    handle->transaction   = NULL;
    handle->speed_hz = settings->speed_hz;
    handle->scl_pin = settings->scl_pin;
    handle->sda_pin = settings->sda_pin;

    i2c_enable(i2c);

    return E_OK;
}


//  ***************************************************************************
/// @brief   Deinitializes I2C software unit and hardware peripheral
/// @param   handle - I2C handle
/// @return  @ref error_t
/// @details This function includes disabling of pins alternate functions, I2C clocks, I2C peripheral and interrupts
//  ***************************************************************************
error_t i2c_deinit(handle_t handle) {
    if (handle == NULL) return E_SOFTWARE_FLAG | E_SOURCE_I2C | E_NO_DEVICE;
    if (handle->state & I2C_BUSY_FLAG) return E_SOURCE_I2C | E_BUSY;

    i2c_disable(i2c);
    free(handle);

    return E_OK;
}


//  ***************************************************************************
/// @brief   Starts processing of I2C transaction (asynchronous interface)
/// @param   handle      - pointer to I2C handle
/// @param   transaction - pointer to @ref i2c_transaction_t
/// @return  @ref error_t
/// @details Transaction will be processed in interrupts
//  ***************************************************************************
error_t i2c_transfer_begin(handle_t handle, i2c_transaction_t *transaction) {
    // Unsupported !
    ERROR_FATAL(i2c_transfer_begin, __LINE__); break;
}


//  ***************************************************************************
/// @brief   Checks transaction processing status (asynchronous interface)
/// @param   handle           - pointer to I2C handle
/// @param   transaction      - pointer to @ref i2c_transaction_t
/// @param   stop_transaction - transaction stop request
/// @return  @ref error_t
//  ***************************************************************************
error_t i2c_transfer_end(handle_t handle, i2c_transaction_t *transaction, bool stop_transaction) {
    // Unsupported !
    ERROR_FATAL(i2c_transfer_begin, __LINE__); break;
}


//  ***************************************************************************
/// @brief   Terminates processing of transaction (asynchronous interface)
/// @param   handle      - pointer to I2C handle
/// @param   transaction - pointer to @ref i2c_transaction_t
/// @return  @ref error_t
/// @details This function MUST be called if transaction timeout is expired \n
///          Transaction processing will be terminated, I2C peripheral will be reset
//  ***************************************************************************
error_t i2c_transfer_terminate(handle_t handle, i2c_transaction_t *transaction) {
    // Unsupported !
    ERROR_FATAL(i2c_transfer_begin, __LINE__); break;
}


//  ***************************************************************************
/// @brief   Performs full transaction processing (synchronous interface)
/// @param   handle      - pointer to I2C handle
/// @param   transaction - pointer to @ref i2c_transaction_t
/// @param   retries     - number of retries
/// @param   timeout     - operation timeout (for all retries, not for each one)
/// @return  @ref error_t
/// @details Make sure not to pass milliseconds value (such as 100) as timeout argument,
///          timeout argument must be result of timer_start() function \n
///          Transaction is processed in interrupts, caller thread will wait for completion status or timeout
//  ***************************************************************************
error_t i2c_transfer(handle_t handle, i2c_transaction_t *transaction, uint32_t retries, timer_t timeout) {
    i2c_internal_t *i2c;
    error_t        result;
    uint32_t       retry;


    i2c = (i2c_internal_t*)handle;
    if (i2c == NULL) return E_SOFTWARE_FLAG | E_SOURCE_I2C | E_NO_DEVICE;


    for (retry = 0; retry < retries; retry++) {
        // Begin transfer
        while (1) {
            result = i2c_transfer_begin(handle, transaction); // Begin I2C transfer
            if (result == E_OK) break; // Transfer launched successfully -> Proceed
            // Timeout expired -> Terminate
            if (timer_triggered(timeout)) {
                result = E_SOURCE_I2C | E_TIMEOUT;
                goto failed;
            }
        }

        // Finish transfer
        while (1) {
            if (timer_triggered(timeout)) {
                result = i2c_transfer_end(handle, transaction, true); // Request I2C transfer status
                break;
            }
            else {
                result = i2c_transfer_end(handle, transaction, false); // Request I2C transfer status
                if (ERROR_GET_CODE(result) != E_ASYNC_WAIT) break;
            }
        }

        if (timer_triggered(timeout) || (result == E_OK)) break;
    }

    if ((retry < retries) && (result == E_OK) && (i2c->transaction_result == E_OK)) return E_OK;

failed:
    if (result != E_OK) return result;
    return E_SOURCE_I2C | i2c->transaction_result;
}




//  ***************************************************************************
/// @brief  Force I2C clocking when SDA stuck low
/// @param  i2c - pointer to i2c internal structure
/// @return @ref error_t
/// @note   That function used when slave occupied SDA:
/// @note   i2c_transfer_end() returned E_I2C_ETX_SLAVE_ERROR
//  ***************************************************************************
error_t i2c_bus_clear(handle_t handle) {
    i2c_internal_t *i2c;
    uint32_t       us_period;
    error_t        result;
    uint8_t        i;


    i2c = (i2c_internal_t*)handle;
    if (i2c == NULL) return E_SOFTWARE_FLAG | E_SOURCE_I2C | E_NO_DEVICE;


    // Calculate SCL period
    us_period = (1000000 / i2c->speed_hz) + 1;

    // Turn off I2C peripheral and clock
    result = i2c_disable(i2c);
    if (result != E_OK) return E_SOFTWARE_FLAG | E_SOURCE_I2C | result;

    gpio_config_pins(i2c->scl_pin, GPIO_MODE_OUTPUT_OD, GPIO_PULL_UP, GPIO_SPEED_LOW, 0, false);

    // Toggle SCL 9 times
    for (i = 0; i < 10; i++) {
        gpio_reset_pins(i2c->scl_pin);
        delay_us(us_period);
        gpio_set_pins(i2c->scl_pin);
        delay_us(us_period);
    }

    // Reinit I2C
    result = i2c_enable(i2c);
    if (result != E_OK) return E_SOFTWARE_FLAG | E_SOURCE_I2C | result;

    result = i2c_set_speed(i2c, i2c->speed_hz);
    if (result != E_OK) return E_SOFTWARE_FLAG | E_SOURCE_I2C | result;

    result = i2c_enable_interrupts(i2c);
    if (result != E_OK) return E_SOFTWARE_FLAG | E_SOURCE_I2C | result;

    return i2c_switch_master(i2c);
}



//  ***************************************************************************
/// @brief   Turn on specified I2C module
/// @param   i2c - pointer to i2c internal structure
/// @return  @ref error_t
/// @details Function enable periph clock, config involved pins as AF, enable global interrupts
//  ***************************************************************************
static error_t i2c_enable(i2c_internal_t *i2c) {
    // Enable GPIO clocks
    sysclk_enable_peripheral(gpio_get_peripheral(i2c->scl_pin));
    sysclk_enable_peripheral(gpio_get_peripheral(i2c->sda_pin));

    gpio_config_pins(i2c->scl_pin, GPIO_MODE_OUTPUT_OD, GPIO_PULL_UP, GPIO_SPEED_LOW, 0, false);
    gpio_config_pins(i2c->sda_pin, GPIO_MODE_OUTPUT_OD, GPIO_PULL_UP, GPIO_SPEED_LOW, 0, false);

    return E_OK;
}


//  ***************************************************************************
/// @brief   Turn off specified I2C module
/// @note    This function reset I2C registers!!!
/// @param   i2c - pointer to i2c internal structure
/// @return  @ref error_t
/// @note    I2C registers values are retain on turn-off periph clock
/// @details Function disable periph clock, config involved pins as GPIO, disable global interrupts
//  ***************************************************************************
static error_t i2c_disable(i2c_internal_t *i2c) {
    gpio_config_pins(i2c->scl_pin, GPIO_MODE_INPUT, GPIO_PULL_NONE, GPIO_SPEED_LOW, 0, false);
    gpio_config_pins(i2c->sda_pin, GPIO_MODE_INPUT, GPIO_PULL_NONE, GPIO_SPEED_LOW, 0, false);

    return E_OK;
}





void I2C_Start(void) {
    I2C_SDA_PORT->BSRR = (1<<I2C_SDA_PIN);
    _delay_us(TimeX);
    I2C_SCL_PORT->BSRR = (1<<I2C_SCL_PIN);
    _delay_us(TimeX);
    while(!(I2C_SDA_PORT->IDR & (1<<I2C_SDA_PIN))) {
        I2C_SCL_PORT->BRR = (1<<I2C_SCL_PIN);
        _delay_us(TimeX);
        I2C_SCL_PORT->BSRR = (1<<I2C_SCL_PIN);
        _delay_us(TimeX);
    }
    I2C_SDA_PORT->BRR = (1<<I2C_SDA_PIN);
    _delay_us(TimeX);
    I2C_SCL_PORT->BRR = (1<<I2C_SCL_PIN);
    _delay_us(TimeX);
}

void I2C_Stop(void) {
    I2C_SDA_PORT->BRR = (1<<I2C_SDA_PIN);
    _delay_us(TimeX);
    I2C_SCL_PORT->BSRR = (1<<I2C_SCL_PIN);
    _delay_us(TimeX);
    I2C_SDA_PORT->BSRR = (1<<I2C_SDA_PIN);
    _delay_us(TimeX);
}

uint8_t I2C_Write_Byte(uint8_t data) {
    uint8_t i;
    uint8_t ACK;


    for(i=0; i < 8 ; i++) {
    if(data & 0x80) {
        I2C_SDA_PORT->BSRR = (1 << I2C_SDA_PIN);
    }
    else
    {
        I2C_SDA_PORT->BRR = (1 << I2C_SDA_PIN);
    }
    _delay_us(TimeX);
    I2C_SCL_PORT->BSRR = (1<<I2C_SCL_PIN);
    _delay_us(TimeX);        
    I2C_SCL_PORT->BRR = (1<<I2C_SCL_PIN);
    data=data<<1;    
    }
    _delay_us(TimeX);        
    I2C_SCL_PORT->BSRR = (1<<I2C_SCL_PIN);
    _delay_us(TimeX);        
    ACK = !(I2C_SDA_PORT->IDR & (1<<I2C_SDA_PIN));
    I2C_SCL_PORT->BRR = (1<<I2C_SCL_PIN);
    I2C_SDA_PORT->BRR = (1<<I2C_SDA_PIN);
    return ACK;
}
void I2C_Write_Block(uint8_t address, uint8_t length)
{
        uint8_t i ;
        
        I2C_Start();
        I2C_Write_Byte(0xA0);
        I2C_Write_Byte(address);
        
        for(i=0;i<length;i++)
        {
        I2C_Write_Byte(I2C_Buffer[i]);      
        }
        I2C_Stop();
}

uint8_t I2C_Read_Byte(uint8_t ACK)
{
        uint8_t i;
        uint8_t data;
        
        I2C_SDA_PORT->BSRR = (1<<I2C_SDA_PIN);
      for(i=0;i<8;i++)
        {
        _delay_us(TimeX);
        I2C_SCL_PORT->BSRR = (1<<I2C_SCL_PIN);
        _delay_us(TimeX);       
        data<<=1;
      if(I2C_SDA_PORT->IDR & (1<<I2C_SDA_PIN))
        data++; 
        I2C_SCL_PORT->BRR = (1<<I2C_SCL_PIN);
        }
      if (ACK)
        I2C_SDA_PORT->BRR = (1<<I2C_SDA_PIN);
        _delay_us(TimeX);       
        I2C_SCL_PORT->BSRR = (1<<I2C_SCL_PIN);
        _delay_us(TimeX);       
        I2C_SCL_PORT->BRR = (1<<I2C_SCL_PIN);
        I2C_SDA_PORT->BSRR = (1<<I2C_SDA_PIN);
        return data;
}
void I2C_Read_Block(uint8_t address, uint8_t length)
{
        uint8_t i ;
        
        I2C_Start();
        I2C_Write_Byte(0xA0);
        I2C_Write_Byte(address);
        I2C_Start();
        I2C_Write_Byte(0xA1);
        for(i=0;i<length;i++)
        {
        I2C_Buffer[i]  = I2C_Read_Byte(i!=(length-1));      
        }
        I2C_Stop();
}