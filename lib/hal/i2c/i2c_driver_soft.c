//  ***************************************************************************
/// @file    i2c_driver_stm32f0.h
//  ***************************************************************************
#include "hal/i2c/i2c_driver.h"
#include <stdlib.h>
#include "common/mcu.h"
#include "hal/systimer/systimer.h"
#include "hal/sysclk/sysclk.h"
#include "hal/gpio/gpio.h"


#define I2C_COUNT (1)
#define I2C_TIMER (TIM17)

#define SDA_UP        i2c_sda_port->BSRR = (1 << i2c_sda_pin)
#define SDA_DOWN      i2c_sda_port->BRR = (1 << i2c_sda_pin)
#define SCL_UP        i2c_scl_port->BSRR = (1 << i2c_scl_pin)
#define SCL_DOWN      i2c_scl_port->BRR = (1 << i2c_scl_pin)

#define SDA_IS_UP     i2c_sda_port->IDR & (1 << i2c_sda_pin)
#define SDA_IS_DOWN   !(i2c_sda_port->IDR & (1 << i2c_sda_pin))
#define SCL_IS_UP     i2c_scl_port->IDR & (1 << i2c_scl_pin)
#define SCL_IS_DOWN   !(i2c_scl_port->IDR & (1 << i2c_scl_pin))


typedef struct {
    gpio_pin_t scl_pin;
    gpio_pin_t sda_pin;
} i2c_internal_t;


static timer_t i2c_timeout_timer;
static GPIO_TypeDef *i2c_sda_port;
static uint8_t i2c_sda_pin;
static GPIO_TypeDef *i2c_scl_port;
static uint8_t i2c_scl_pin;


static void i2c_enable(i2c_internal_t *i2c);
static void i2c_disable(i2c_internal_t *i2c);
static bool i2c_start(void);
static void i2c_stop(void);
static bool i2c_write_byte(uint8_t data);
static bool i2c_read_byte(uint8_t *recv_data, uint8_t is_ack);
static error_t i2c_clock_delay_init(uint32_t i2c_speed_hz);
static void i2c_clock_delay(void);




//  ***************************************************************************
/// @brief   Init I2C
/// @param   handle
/// @param   hwid
/// @param   i2c_settings_t
/// @retval  handle
/// @return  @ref error_t
//  ***************************************************************************
error_t i2c_init(handle_t *handle, void *hwid, const i2c_settings_t *settings) {
    i2c_internal_t *i2c;
    

    *handle = malloc(sizeof(i2c_internal_t));
    if (*handle == NULL) return E_SOFTWARE_FLAG | E_SOURCE_I2C | E_OUT_OF_MEMORY;
    i2c = (i2c_internal_t*)*handle;
    i2c->scl_pin = settings->scl_pin;
    i2c->sda_pin = settings->sda_pin;

    i2c_enable(i2c);
    return i2c_clock_delay_init(settings->speed_hz);
}


//  ***************************************************************************
/// @brief   Deinitializes I2C software unit and hardware peripheral
/// @param   handle - I2C handle
/// @return  @ref error_t
/// @details This function includes disabling of pins alternate functions, I2C clocks, I2C peripheral and interrupts
//  ***************************************************************************
error_t i2c_deinit(handle_t handle) {
    i2c_internal_t *i2c;
    i2c = (i2c_internal_t*)handle;
    
    if (i2c == NULL) return E_SOFTWARE_FLAG | E_SOURCE_I2C | E_NO_DEVICE;

    i2c_disable(i2c);
    free(i2c);

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
    ERROR_FATAL(i2c_transfer_begin, __LINE__);
    return E_CANCELLED;
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
    ERROR_FATAL(i2c_transfer_begin, __LINE__);
    return E_CANCELLED;
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
    ERROR_FATAL(i2c_transfer_begin, __LINE__);
    return E_CANCELLED;
}


//  ***************************************************************************
/// @brief   Performs full transaction processing (synchronous interface)
/// @param   handle      - pointer to I2C handle
/// @param   transaction - pointer to @ref i2c_transaction_t
/// @param   retries     - number of retries
/// @param   timeout_ms  - operation timeout (for all retries, not for each one)
/// @return  @ref error_t
//  ***************************************************************************
error_t i2c_transfer(handle_t handle, i2c_transaction_t *transaction, uint32_t retries, uint32_t timeout_ms) {
    i2c_internal_t *i2c;
    error_t result;
    uint32_t retry;
    uint32_t i;
    bool is_rx_only = true;


    i2c = (i2c_internal_t*)handle;
    if (i2c == NULL) return E_SOFTWARE_FLAG | E_SOURCE_I2C | E_NO_DEVICE;


    i2c_timeout_timer = timer_start_ms(timeout_ms);
    i2c_sda_port = (GPIO_TypeDef*)gpio_get_peripheral(i2c->sda_pin);
    i2c_sda_pin = gpio_get_pin_n(i2c->sda_pin);
    i2c_scl_port = (GPIO_TypeDef*)gpio_get_peripheral(i2c->scl_pin);
    i2c_scl_pin = gpio_get_pin_n(i2c->scl_pin);

    result = E_OK;
    for (retry = 0; retry < retries; retry++) {
        if (timer_triggered(i2c_timeout_timer)) break;
        if (retry > 0) i2c_stop();

        if ((transaction->tx_size > 0) && (transaction->tx_data != NULL)) {
            is_rx_only = false;
            if (!i2c_start()) continue;
            if (!i2c_write_byte(transaction->address | 0)) continue;
            for(i = 0; i < transaction->tx_size; i++) {
                if (!i2c_write_byte(transaction->tx_data[i])) continue;     
            }
        }

        if ((transaction->rx_size > 0) && (transaction->rx_data != NULL)) {
            if (!i2c_start()) continue;
            if (is_rx_only) {
                if (!i2c_write_byte(transaction->address | 1)) continue;
            }
            for(i = 0; i < transaction->rx_size; i++) {
                if (!i2c_read_byte(&transaction->rx_data[i], (i != (transaction->rx_size - 1)))) continue;  
            }
        }

        break;
    }

    i2c_stop();

    if ((retry > retries) && (result == E_OK)) result = E_FAILED;
    return result;
}


//  ***************************************************************************
/// @brief  Force I2C clocking when SDA stuck low
/// @param  i2c - pointer to i2c internal structure
/// @return @ref error_t
/// @note   That function used when slave occupied SDA:
/// @note   i2c_transfer_end() returned E_I2C_ETX_SLAVE_ERROR
//  ***************************************************************************
error_t i2c_bus_clear(handle_t handle) {
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
/// @param   i2c - pointer to i2c internal structure
/// @return  none
//  ***************************************************************************
static void i2c_enable(i2c_internal_t *i2c) {
    sysclk_enable_peripheral(gpio_get_peripheral(i2c->scl_pin));
    sysclk_enable_peripheral(gpio_get_peripheral(i2c->sda_pin));

    gpio_config_pins(i2c->scl_pin, GPIO_MODE_OUTPUT_OD, GPIO_PULL_UP, GPIO_SPEED_LOW, 0, true);
    gpio_config_pins(i2c->sda_pin, GPIO_MODE_OUTPUT_OD, GPIO_PULL_UP, GPIO_SPEED_LOW, 0, true);
}


//  ***************************************************************************
/// @brief   Turn off specified I2C module
/// @param   i2c - pointer to i2c internal structure
/// @return  none
//  ***************************************************************************
static void i2c_disable(i2c_internal_t *i2c) {
    gpio_config_pins(i2c->scl_pin, GPIO_MODE_INPUT, GPIO_PULL_NONE, GPIO_SPEED_LOW, 0, false);
    gpio_config_pins(i2c->sda_pin, GPIO_MODE_INPUT, GPIO_PULL_NONE, GPIO_SPEED_LOW, 0, false);
}


static bool i2c_start(void) {
    if (SDA_IS_DOWN) return false;
    if (SCL_IS_DOWN) return false;

    SDA_DOWN;
    i2c_clock_delay();
    SCL_DOWN;
    i2c_clock_delay();

    if (SDA_IS_UP) return false;
    if (SCL_IS_UP) return false;
    return true;
}


static void i2c_stop(void) {
    SDA_DOWN;
    i2c_clock_delay();
    SCL_UP;
    i2c_clock_delay();
    SDA_UP;
    i2c_clock_delay();
}


static bool i2c_write_byte(uint8_t data) {
    uint8_t i;
    uint8_t ack;


    // Send data
    for(i = 0; i < 8 ; i++) {
        if (data & 0x80) SDA_UP;
        else SDA_DOWN;
        i2c_clock_delay();
        SCL_UP;
        i2c_clock_delay();

        // Clock stretching processing
        while(SCL_IS_DOWN) {
            if (timer_triggered(i2c_timeout_timer)) return false;
        }
      
        SCL_DOWN;
        data = data << 1;    
    }

    // Recv ACK/NACK
    i2c_clock_delay();
    SDA_UP;
    i2c_clock_delay();
    SCL_UP;
    i2c_clock_delay();
    ack = SDA_IS_DOWN;
    SCL_DOWN;
    SDA_DOWN;
    return ack;
}


static bool i2c_read_byte(uint8_t *recv_data, uint8_t is_ack) {
    uint8_t i;
    uint8_t data;
    
    // Recv data
    SDA_UP;
    for(i = 0; i < 8; i++) {
        i2c_clock_delay();
        SCL_UP;
        i2c_clock_delay();
        
        // Clock stretching processing
        while(SCL_IS_DOWN) {
            if (timer_triggered(i2c_timeout_timer)) return false;
        }

        data<<=1;
        if(SDA_IS_UP) data++; 
        SCL_DOWN;
    }

    // Send ACK/NACK
    if (is_ack) SDA_DOWN;
    i2c_clock_delay();       
    SCL_UP;
    i2c_clock_delay();       
    SCL_DOWN;
    SDA_UP;

    *recv_data = data;

    return true;
}


static error_t i2c_clock_delay_init(uint32_t i2c_speed_hz) {
    uint32_t timer_clock_hz, arr;


    sysclk_enable_peripheral(I2C_TIMER);
    I2C_TIMER->CR1 = 1 << TIM_CR1_OPM_Pos;
    I2C_TIMER->PSC = 0;

    sysclk_get_peripheral_freq(I2C_TIMER, &timer_clock_hz);
    arr = timer_clock_hz / i2c_speed_hz;
    arr = arr >> 2;   // /2
    I2C_TIMER->ARR = arr;

    if (arr == 0) return E_INVALID_CONFIG;
    return E_OK;
}


static void i2c_clock_delay(void) {
    TIM17->SR = 0;
    TIM17->CR1 = TIM_CR1_CEN;
    while(!(TIM17->SR & TIM_SR_UIF)) ;
}