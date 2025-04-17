//  ***************************************************************************
/// @file    gpio_p0.c
/// @brief   GPIO driver for P0 periphery
//  ***************************************************************************
#include "hal/gpio.h"
#include <stdlib.h>
#include "common/error.h"


#define PORT_MASK                           (0xFFFF0000ul)
#define PINS_MASK                           (0x0000FFFFul)


//  ***************************************************************************
/// @brief   Config one pin
/// @param   pins
/// @param   mode
/// @param   pull
/// @param   speed
/// @param   alt_func
/// @param   output_state
/// @return  none
//  ***************************************************************************
void gpio_config_pins(gpio_pin_t pins, gpio_mode_t mode, gpio_pull_t pull, gpio_speed_t speed, uint32_t alt_func, bool output_state) {
    GPIO_TypeDef *port;
    uint32_t     pin;
    uint32_t     pin_2_bits_reg_pos, afr_index, pin_afr_reg_pos;


    uint32_t moder_mode, otyper_otype, ospeedr_ospeed, pupdr_pupd;
    switch (mode) {
        case GPIO_MODE_INPUT:           moder_mode = 0;    otyper_otype = 0;  break;
        case GPIO_MODE_OUTPUT_PP:       moder_mode = 1;    otyper_otype = 0;  break;
        case GPIO_MODE_OUTPUT_OD:       moder_mode = 1;    otyper_otype = 1;  break;
        case GPIO_MODE_ANALOG:          moder_mode = 3;    otyper_otype = 0;  break;
        case GPIO_MODE_ALT_FUNCTION:    moder_mode = 2;    otyper_otype = 0;  break;
        case GPIO_MODE_ALT_FUNCTION_PP: moder_mode = 2;    otyper_otype = 0;  break;
        case GPIO_MODE_ALT_FUNCTION_OD: moder_mode = 2;    otyper_otype = 1;  break;
        #ifdef LIB_DEBUG_EH
        default:                        error_fatal((uintptr_t)gpio_config_pins, __LINE__); break;
        #endif   // LIB_DEBUG_EH
    }
    switch (pull) {
        case GPIO_PULL_NONE: pupdr_pupd = 0;  break;
        case GPIO_PULL_UP:   pupdr_pupd = 1;  break;
        case GPIO_PULL_DOWN: pupdr_pupd = 2;  break;
        #ifdef LIB_DEBUG_EH
        default:             error_fatal((uintptr_t)gpio_config_pins, __LINE__); break;
        #endif   // LIB_DEBUG_EH
    }
    switch (speed) {
        case GPIO_SPEED_LOW:    ospeedr_ospeed = 0;  break;
        case GPIO_SPEED_MEDIUM: ospeedr_ospeed = 2;  break;
        case GPIO_SPEED_HIGH:   ospeedr_ospeed = 3;  break;
        #ifdef LIB_DEBUG_EH
        default:                error_fatal((uintptr_t)gpio_config_pins, __LINE__); break;
        #endif   // LIB_DEBUG_EH
    }


    port = (GPIO_TypeDef*)gpio_get_peripheral(pins);
    pin = gpio_get_pin_n(pins);
    #ifdef LIB_DEBUG_EH
    if (port == NULL) error_fatal((uintptr_t)gpio_config_pins, __LINE__);
    if ((1 << pin) != (pins & PINS_MASK)) error_fatal((uintptr_t)gpio_config_pins, __LINE__);
    #endif   // LIB_DEBUG_EH
    pin_2_bits_reg_pos = pin << 1;
    afr_index = pin >> 3;
    pin_afr_reg_pos = afr_index << 5; // optimisation from (pin & 0x7) * 4;

    port->ODR     &= ~(1 << pin);
    port->ODR     |= ((output_state ? 1 : 0) << pin);
    port->OSPEEDR &= ~(0x03 << pin_2_bits_reg_pos);
    port->OSPEEDR |= (ospeedr_ospeed << pin_2_bits_reg_pos);
    port->AFR[afr_index] &= ~(0x0F << pin_afr_reg_pos);
    port->AFR[afr_index] |= (alt_func << pin_afr_reg_pos);
    port->PUPDR   &= ~(0x03 << pin_2_bits_reg_pos);
    port->PUPDR   |= (pupdr_pupd << pin_2_bits_reg_pos);
    port->OTYPER  |= (otyper_otype << pin);
    port->MODER   &= ~(0x03 << pin_2_bits_reg_pos);
    port->MODER   |= (moder_mode << pin_2_bits_reg_pos);
}


//  ***************************************************************************
/// @brief   Set pin(s) (switch to "1" state), simultaneous set for all specified pins
/// @param   pins
/// @return  none
//  ***************************************************************************
void gpio_set_pins(gpio_pin_t pins) {
    GPIO_TypeDef *port;


    port = (GPIO_TypeDef*)gpio_get_peripheral(pins);
    #ifdef LIB_DEBUG_EH
    if (port == NULL) error_fatal((uintptr_t)gpio_set_pins, __LINE__);
    #endif   // LIB_DEBUG_EH
    port->BSRR = pins & PINS_MASK;
}


//  ***************************************************************************
/// @brief   Reset pin(s) (switch to "0" state), simultaneous reset for all specified pins
/// @param   pins
/// @return  none
//  ***************************************************************************
void gpio_reset_pins(gpio_pin_t pins) {
    GPIO_TypeDef *port;


    port = (GPIO_TypeDef*)gpio_get_peripheral(pins);
    #ifdef LIB_DEBUG_EH
    if (port == NULL) error_fatal((uintptr_t)gpio_reset_pins, __LINE__);
    #endif   // LIB_DEBUG_EH
    port->BSRR = (pins & PINS_MASK) << 16;
}


//  ***************************************************************************
/// @brief   Reads pin(s) state
/// @param   pins
/// @return  gpio_pin_t
//  ***************************************************************************
gpio_pin_t gpio_read_pins(gpio_pin_t pins) {
    GPIO_TypeDef *port;
    uint32_t     read;
    gpio_pin_t   result;


    port = (GPIO_TypeDef*)gpio_get_peripheral(pins);
    #ifdef LIB_DEBUG_EH
    if (port == NULL) error_fatal((uintptr_t)gpio_read_pins, __LINE__);
    #endif   // LIB_DEBUG_EH
    read = port->IDR;
    result = (pins & PORT_MASK) | (read & pins & PINS_MASK);
    return result;
}


//  ***************************************************************************
/// @brief   Decodes gpio_pin_t to peripheral base address pointer
/// @param   pin
/// @return  peripheral base address pointer
//  ***************************************************************************
void *gpio_get_peripheral(gpio_pin_t pin) {
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


//  ***************************************************************************
/// @brief   Decodes gpio_pin_t to pin number
/// @param   pin
/// @return  pin number
//  ***************************************************************************
uint8_t gpio_get_pin_n(gpio_pin_t pin) {
    pin = pin & PINS_MASK;
    uint8_t pin_n = 0;
    pin = pin >> 1;
    while (pin != 0) {
        pin = pin >> 1;
        pin_n++;
    }
    return pin_n;
}
