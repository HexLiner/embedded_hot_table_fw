//  ***************************************************************************
/// @file    gpio_stm32.c
/// @brief   GPIO driver
//  ***************************************************************************
#include "hal/gpio/gpio.h"
#include <stdlib.h>
#include "common/error.h"

// GPIO_STM32F0

#define PORT_MASK                           (0xFFFF0000ul)
#define PINS_MASK                           (0x0000FFFFul)




//  ***************************************************************************
/// @brief   Config pin(s)
/// @param   pins - @ref gpio_pin_t, pin(s) to be configured
/// @param   mode - @ref gpio_mode_t, pin operation mode
/// @param   pull - @ref gpio_pull_t, pin pull-up/pull-down config
/// @param   speed - @ref gpio_speed_t, pin slope control for output/alternate mode
/// @param   alt_func - pin alternate function
/// @param   output_state - pin state for output mode
/// @return  none
//  ***************************************************************************
void gpio_config_pins(gpio_pin_t pins, gpio_mode_t mode, gpio_pull_t pull, gpio_speed_t speed, uint32_t alt_func, bool output_state) {
    peripheral_t port;
    uint32_t     pin, pin_reg_pos;


    // Check port is valid
    port = gpio_get_peripheral(pins);
    if (port == NULL) ERROR_FATAL(gpio_config_pins, __LINE__);
    pins = pins & PINS_MASK;
    pin = 0;
    pins = pins >> 1;
    while (pins != 0) {
        pins = pins >> 1;
        pin++;
    }
    pin_reg_pos = pin * 2;


    // Calc register fields values
    uint32_t moder_mode, otyper_otype, ospeedr_ospeed, pupdr_pupd;
    switch (mode) {
        case GPIO_MODE_INPUT:           moder_mode = 0;    otyper_otype = 0;  break;
        case GPIO_MODE_OUTPUT_PP:       moder_mode = 1;    otyper_otype = 0;  break;
        case GPIO_MODE_OUTPUT_OD:       moder_mode = 1;    otyper_otype = 1;  break;
        case GPIO_MODE_ANALOG:          moder_mode = 3;    otyper_otype = 0;  break;
        case GPIO_MODE_ALT_FUNCTION:    moder_mode = 2;    otyper_otype = 0;  break;
        case GPIO_MODE_ALT_FUNCTION_PP: moder_mode = 2;    otyper_otype = 0;  break;
        case GPIO_MODE_ALT_FUNCTION_OD: moder_mode = 2;    otyper_otype = 1;  break;
        default:                        ERROR_FATAL(gpio_config_pins, __LINE__); break;
    }
    switch (pull) {
        case GPIO_PULL_NONE: pupdr_pupd = 0;  break;
        case GPIO_PULL_UP:   pupdr_pupd = 1;  break;
        case GPIO_PULL_DOWN: pupdr_pupd = 2;  break;
        default:             ERROR_FATAL(gpio_config_pins, __LINE__); break;
    }
    switch (speed) {
        case GPIO_SPEED_LOW:    ospeedr_ospeed = 0;  break;
        case GPIO_SPEED_MEDIUM: ospeedr_ospeed = 2;  break;
        case GPIO_SPEED_HIGH:   ospeedr_ospeed = 3;  break;
        default:                ERROR_FATAL(gpio_config_pins, __LINE__); break;
    }


    //// order ???
    ((GPIO_TypeDef*)port)->ODR     &= ~(1 << pin);
    ((GPIO_TypeDef*)port)->ODR     |= ((output_state ? 1 : 0) << pin);
    ((GPIO_TypeDef*)port)->OSPEEDR &= ~(0x03 << pin_reg_pos);
    ((GPIO_TypeDef*)port)->OSPEEDR |= (ospeedr_ospeed << pin_reg_pos);
    ((GPIO_TypeDef*)port)->AFR[pin >> 3] &= ~(4 << (pin & 0x7));
    ((GPIO_TypeDef*)port)->AFR[pin >> 3] |= (alt_func << (pin & 0x7));
    ((GPIO_TypeDef*)port)->PUPDR   &= ~(0x03 << pin_reg_pos);
    ((GPIO_TypeDef*)port)->PUPDR   |= (pupdr_pupd << pin_reg_pos);
    ((GPIO_TypeDef*)port)->OTYPER  |= (otyper_otype << pin);
    ((GPIO_TypeDef*)port)->MODER   &= ~(0x03 << pin_reg_pos);
    ((GPIO_TypeDef*)port)->MODER   |= (moder_mode << pin_reg_pos);
}


//  ***************************************************************************
/// @brief   Set pin(s) (switch to "1" state), simultaneous set for all specified pins
/// @param   pins - @ref gpio_pin_t, pin(s) to be set
/// @return  none
//  ***************************************************************************
void gpio_set_pins(gpio_pin_t pins) {
    peripheral_t port;


    port = gpio_get_peripheral(pins);
    if (port == NULL) ERROR_FATAL(gpio_set_pins, __LINE__);

    ((GPIO_TypeDef*)port)->BSRR = (pins & PINS_MASK) << 0;
}


//  ***************************************************************************
/// @brief   Reset pin(s) (switch to "0" state), simultaneous reset for all specified pins
/// @param   pins - @ref gpio_pin_t, pin(s) to be reset
/// @return  none
//  ***************************************************************************
void gpio_reset_pins(gpio_pin_t pins) {
    peripheral_t port;


    port = gpio_get_peripheral(pins);
    if (port == NULL) ERROR_FATAL(gpio_reset_pins, __LINE__);

    ((GPIO_TypeDef*)port)->BSRR = (pins & PINS_MASK) << 16;
}


//  ***************************************************************************
/// @brief   Reads pin(s) state
/// @param   pins - @ref gpio_pin_t, pin(s) to be read
/// @return  result - @ref gpio_pin_t, pin(s) read
//  ***************************************************************************
gpio_pin_t gpio_read_pins(gpio_pin_t pins) {
    peripheral_t port;
    uint32_t     read;
    gpio_pin_t   result;


    port = gpio_get_peripheral(pins);
    if (port == NULL) ERROR_FATAL(gpio_read_pins, __LINE__);

    read = ((GPIO_TypeDef*)port)->IDR;
    result = (pins & PORT_MASK) | (read & pins & PINS_MASK);

    return result;
}


//  ***************************************************************************
/// @brief   Decodes port (inside @ref gpio_pin_t) to peripheral base address pointer
/// @param   pins - @ref gpio_pin_t, pin(s) to be decoded
/// @return  peripheral base address pointer, NULL if error
//  ***************************************************************************
peripheral_t gpio_get_peripheral(gpio_pin_t pin) {
    gpio_pin_t port = (pin & PORT_MASK);

    #ifdef GPIOA
        if (port == GPIOA_ID) return GPIOA;
    #endif
    #ifdef GPIOB_ID
        if (port == GPIOB_ID) return GPIOB;
    #endif
    #ifdef GPIOC_ID
        if (port == GPIOC_ID) return GPIOC;
    #endif
    #ifdef GPIOD_ID
        if (port == GPIOD_ID) return GPIOD;
    #endif
    #ifdef GPIOE_ID
        if (port == GPIOE_ID) return GPIOE;
    #endif
    #ifdef GPIOF_ID
        if (port == GPIOF_ID) return GPIOF;
    #endif
    #ifdef GPIOG_ID
        if (port == GPIOG_ID) return GPIOG;
    #endif
    #ifdef GPIOH_ID
        if (port == GPIOH_ID) return GPIOH;
    #endif
    #ifdef GPIOI_ID
        if (port == GPIOI_ID) return GPIOI;
    #endif
    #ifdef GPIOJ_ID
        if (port == GPIOJ_ID) return GPIOJ;
    #endif
    #ifdef GPIOK_ID
        if (port == GPIOK_ID) return GPIOK;
    #endif

    return NULL;
}
