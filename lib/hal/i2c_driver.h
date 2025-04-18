//  ***************************************************************************
/// @file    i2c_driver.h
/// @brief   I2C master driver
//  ***************************************************************************
#ifndef _I2C_DRIVER_H_
#define _I2C_DRIVER_H_

#include "common/error.h"
#include "hal/systimer.h"
#include "hal/gpio.h"


typedef struct {
    // Public
    void         *peripheral;   // I2C for HW driver, TIM for SW driver
    uint32_t     speed_hz;
    gpio_pin_t   scl_pin;
    gpio_pin_t   sda_pin;
    // Private
    uint32_t     int_stack[8];
} i2c_t;

typedef struct {
    uint8_t address;  // Slave address (left-aligned - mask 0xFE)
    const uint8_t *tx_data; // Transmit data buffer (NULL to disable transmit)
    uint8_t tx_size;  // Number of bytes to transmit (0 to disable transmit)
    uint8_t *rx_data; // Receive data buffer (NULL to disable receive)
    uint8_t rx_size;  // Number of bytes to receive (0 to disable receive)
} i2c_transaction_t;


extern error_t i2c_init(i2c_t *i2c);
extern void i2c_handler(i2c_t *i2c);

extern error_t i2c_transfer_begin(i2c_t *i2c, i2c_transaction_t *transaction);
extern error_t i2c_transfer_end(i2c_t *i2c, i2c_transaction_t *transaction, bool stop_transaction);
extern error_t i2c_transfer_terminate(i2c_t *i2c, i2c_transaction_t *transaction);
extern error_t i2c_bus_clear(i2c_t *i2c);
extern error_t i2c_transfer(i2c_t *i2c, i2c_transaction_t *transaction, uint32_t retries, uint32_t timeout_ms);


#endif // _I2C_DRIVER_H_
