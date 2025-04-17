//  ***************************************************************************
/// @file    i2c_driver_stm32f0.h
//  ***************************************************************************
#include "hal/i2c_driver.h"
#include <stdlib.h>
#include "common/mcu.h"
#include "hal/systimer.h"
#include "hal/sysclk.h"
#include "hal/gpio.h"


#define SDA_UP        i2c_int->sda_port->BSRR = (1 << i2c_int->sda_pin)
#define SDA_DOWN      i2c_int->sda_port->BRR = (1 << i2c_int->sda_pin)
#define SCL_UP        i2c_int->scl_port->BSRR = (1 << i2c_int->scl_pin)
#define SCL_DOWN      i2c_int->scl_port->BRR = (1 << i2c_int->scl_pin)

#define SDA_IS_UP     i2c_int->sda_port->IDR & (1 << i2c_int->sda_pin)
#define SDA_IS_DOWN   !(i2c_int->sda_port->IDR & (1 << i2c_int->sda_pin))
#define SCL_IS_UP     i2c_int->scl_port->IDR & (1 << i2c_int->scl_pin)
#define SCL_IS_DOWN   !(i2c_int->scl_port->IDR & (1 << i2c_int->scl_pin))

#define I2C_DELAY     i2c_clock_delay(i2c_int)


typedef struct {
    timer_t timeout_timer;
    GPIO_TypeDef *sda_port;
    uint32_t sda_pin;
    GPIO_TypeDef *scl_port;
    uint32_t scl_pin;
    TIM_TypeDef* delay_tim;
} i2c_driver_soft_int_t;


static void i2c_enable(i2c_t *i2c);
static void i2c_disable(i2c_t *i2c);
static bool i2c_start(i2c_driver_soft_int_t *i2c_int);
static void i2c_stop(i2c_driver_soft_int_t *i2c_int);
static bool i2c_write_byte(i2c_driver_soft_int_t *i2c_int, uint8_t data);
static bool i2c_read_byte(i2c_driver_soft_int_t *i2c_int, uint8_t *recv_data, uint8_t is_ack);
static error_t i2c_clock_delay_init(i2c_t *i2c);
static void i2c_clock_delay(i2c_driver_soft_int_t *i2c_int);




//  ***************************************************************************
/// @brief   Init I2C
/// @param   i2c
/// @retval  i2c
/// @return  @ref error_t
//  ***************************************************************************
error_t i2c_init(i2c_t *i2c) {
    i2c_driver_soft_int_t *i2c_int;


    i2c_enable(i2c);

    i2c_int = (i2c_driver_soft_int_t*)&i2c->int_stack;
    i2c_int->sda_port = (GPIO_TypeDef*)gpio_get_peripheral(i2c->sda_pin);
    i2c_int->sda_pin = (uint32_t)gpio_get_pin_n(i2c->sda_pin);
    i2c_int->scl_port = (GPIO_TypeDef*)gpio_get_peripheral(i2c->scl_pin);
    i2c_int->scl_pin = (uint32_t)gpio_get_pin_n(i2c->scl_pin);
    i2c_int->delay_tim = (TIM_TypeDef*)i2c->peripheral;

    return i2c_clock_delay_init(i2c);
}


//  ***************************************************************************
/// @brief  I2C handler
/// @param  i2c
/// @return none
//  ***************************************************************************
void i2c_handler(i2c_t *i2c) {

}


//  ***************************************************************************
/// @brief   Starts processing of I2C transaction (asynchronous interface)
/// @param   i2c
/// @param   transaction
/// @return  @ref error_t
/// @details Transaction will be processed in interrupts
//  ***************************************************************************
error_t i2c_transfer_begin(i2c_t *i2c, i2c_transaction_t *transaction) {
    // Unsupported !
    #ifdef LIB_DEBUG_EH
    error_fatal((uintptr_t)i2c_transfer_begin, __LINE__);
    #endif   // LIB_DEBUG_EH
    return E_CANCELLED;
}


//  ***************************************************************************
/// @brief   Checks transaction processing status (asynchronous interface)
/// @param   i2c
/// @param   transaction
/// @param   stop_transaction
/// @return  @ref error_t
//  ***************************************************************************
error_t i2c_transfer_end(i2c_t *i2c, i2c_transaction_t *transaction, bool stop_transaction) {
    // Unsupported !
    #ifdef LIB_DEBUG_EH
    error_fatal((uintptr_t)i2c_transfer_begin, __LINE__);
    #endif   // LIB_DEBUG_EH
    return E_CANCELLED;
}


//  ***************************************************************************
/// @brief   Terminates processing of transaction (asynchronous interface)
/// @param   i2c
/// @param   transaction
/// @return  @ref error_t
/// @details This function MUST be called if transaction timeout is expired \n
///          Transaction processing will be terminated, I2C peripheral will be reset
//  ***************************************************************************
error_t i2c_transfer_terminate(i2c_t *i2c, i2c_transaction_t *transaction) {
    // Unsupported !
    #ifdef LIB_DEBUG_EH
    error_fatal((uintptr_t)i2c_transfer_begin, __LINE__);
    #endif   // LIB_DEBUG_EH
    return E_CANCELLED;
}


//  ***************************************************************************
/// @brief   Performs full transaction processing (synchronous interface)
/// @param   i2c
/// @param   transaction
/// @param   retries     - number of retries
/// @param   timeout_ms  - operation timeout (for all retries, not for each one)
/// @return  @ref error_t
//  ***************************************************************************
error_t i2c_transfer(i2c_t *i2c, i2c_transaction_t *transaction, uint32_t retries, uint32_t timeout_ms) {
    i2c_driver_soft_int_t *i2c_int;
    error_t result;
    uint32_t retry;
    uint32_t i;
    bool is_rx_only = true;


    i2c_int = (i2c_driver_soft_int_t*)&i2c->int_stack;

    i2c_int->timeout_timer = timer_start_ms(timeout_ms);
    result = E_OK;
    for (retry = 0; retry <= retries; retry++) {
        if (timer_triggered(i2c_int->timeout_timer)) break;
        if (retry > 0) i2c_stop(i2c_int);

        if ((transaction->tx_size > 0) && (transaction->tx_data != NULL)) {
            is_rx_only = false;
            if (!i2c_start(i2c_int)) continue;
            if (!i2c_write_byte(i2c_int, transaction->address | 0)) continue;
            for(i = 0; i < transaction->tx_size; i++) {
                if (!i2c_write_byte(i2c_int, transaction->tx_data[i])) continue;     
            }
        }

        if ((transaction->rx_size > 0) && (transaction->rx_data != NULL)) {
            if (!i2c_start(i2c_int)) continue;
            if (is_rx_only) {
                if (!i2c_write_byte(i2c_int, transaction->address | 1)) continue;
            }
            for(i = 0; i < transaction->rx_size; i++) {
                if (!i2c_read_byte(i2c_int, &transaction->rx_data[i], (i != (transaction->rx_size - 1)))) continue;  
            }
        }

        break;
    }

    i2c_stop(i2c_int);

    if ((retry >= retries) && (result == E_OK)) result = E_FAILED;
    return result;
}


//  ***************************************************************************
/// @brief  Force I2C clocking when SDA stuck low
/// @param  i2c
/// @return @ref error_t
/// @note   That function used when slave occupied SDA:
/// @note   i2c_transfer_end() returned E_I2C_ETX_SLAVE_ERROR
//  ***************************************************************************
error_t i2c_bus_clear(i2c_t *i2c) {
    /*
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
    */
   return E_OK;
}




//  ***************************************************************************
/// @brief   Turn on specified I2C module
/// @param   i2c
/// @return  none
//  ***************************************************************************
static void i2c_enable(i2c_t *i2c) {
    ////
    sysclk_enable_peripheral(gpio_get_peripheral(i2c->scl_pin));
    sysclk_enable_peripheral(gpio_get_peripheral(i2c->sda_pin));

    gpio_config_pins(i2c->scl_pin, GPIO_MODE_OUTPUT_OD, GPIO_PULL_UP, GPIO_SPEED_LOW, 0, true);
    gpio_config_pins(i2c->sda_pin, GPIO_MODE_OUTPUT_OD, GPIO_PULL_UP, GPIO_SPEED_LOW, 0, true);
}


//  ***************************************************************************
/// @brief   Turn off specified I2C module
/// @param   i2c
/// @return  none
//  ***************************************************************************
static void i2c_disable(i2c_t *i2c) {
    gpio_config_pins(i2c->scl_pin, GPIO_MODE_INPUT, GPIO_PULL_NONE, GPIO_SPEED_LOW, 0, false);
    gpio_config_pins(i2c->sda_pin, GPIO_MODE_INPUT, GPIO_PULL_NONE, GPIO_SPEED_LOW, 0, false);
}


static bool i2c_start(i2c_driver_soft_int_t *i2c_int) {
    if (SDA_IS_DOWN) return false;
    if (SCL_IS_DOWN) return false;

    SDA_DOWN;
    I2C_DELAY;
    SCL_DOWN;
    I2C_DELAY;

    if (SDA_IS_UP) return false;
    if (SCL_IS_UP) return false;
    return true;
}


static void i2c_stop(i2c_driver_soft_int_t *i2c_int) {
    SDA_DOWN;
    I2C_DELAY;
    SCL_UP;
    I2C_DELAY;
    SDA_UP;
    I2C_DELAY;
}


static bool i2c_write_byte(i2c_driver_soft_int_t *i2c_int, uint8_t data) {
    uint8_t i;
    uint8_t ack;


    // Send data
    for(i = 0; i < 8 ; i++) {
        if (data & 0x80) SDA_UP;
        else SDA_DOWN;
        I2C_DELAY;
        SCL_UP;
        I2C_DELAY;

        // Clock stretching processing
        while(SCL_IS_DOWN) {
            if (timer_triggered(i2c_int->timeout_timer)) return false;
        }
      
        SCL_DOWN;
        data = data << 1;    
    }

    // Recv ACK/NACK
    I2C_DELAY;
    SDA_UP;
    I2C_DELAY;
    SCL_UP;
    I2C_DELAY;
    ack = SDA_IS_DOWN;
    SCL_DOWN;
    SDA_DOWN;
    return ack;
}


static bool i2c_read_byte(i2c_driver_soft_int_t *i2c_int, uint8_t *recv_data, uint8_t is_ack) {
    uint8_t i;
    uint8_t data;
    
    // Recv data
    SDA_UP;
    for(i = 0; i < 8; i++) {
        I2C_DELAY;
        SCL_UP;
        I2C_DELAY;
        
        // Clock stretching processing
        while(SCL_IS_DOWN) {
            if (timer_triggered(i2c_int->timeout_timer)) return false;
        }

        data<<=1;
        if(SDA_IS_UP) data++; 
        SCL_DOWN;
    }

    // Send ACK/NACK
    if (is_ack) SDA_DOWN;
    I2C_DELAY;       
    SCL_UP;
    I2C_DELAY;       
    SCL_DOWN;
    SDA_UP;

    *recv_data = data;

    return true;
}


static error_t i2c_clock_delay_init(i2c_t *i2c) {
    uint32_t timer_clock_hz, arr;


    sysclk_enable_peripheral(i2c->peripheral);
    ((TIM_TypeDef*)(i2c->peripheral))->CR1 = 1 << TIM_CR1_OPM_Pos;
    ((TIM_TypeDef*)(i2c->peripheral))->PSC = 0;

    sysclk_get_peripheral_freq(i2c->peripheral, &timer_clock_hz);
    arr = timer_clock_hz / i2c->speed_hz;
    arr = arr >> 2;   // /2
    ((TIM_TypeDef*)(i2c->peripheral))->ARR = arr;

    if (arr == 0) return E_INVALID_CONFIG;
    return E_OK;
}


static void i2c_clock_delay(i2c_driver_soft_int_t *i2c_int) {
    i2c_int->delay_tim->SR = 0;
    i2c_int->delay_tim->CR1 = TIM_CR1_CEN;
    while(!(i2c_int->delay_tim->SR & TIM_SR_UIF)) ;
}