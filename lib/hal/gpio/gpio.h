//  ***************************************************************************
/// @file    gpio.h
/// @brief   GPIO driver
//  ***************************************************************************
#ifndef _GPIO_H_
#define _GPIO_H_

#include <stdint.h>
#include <stdbool.h>
#include "common/mcu.h"
#include "common/lib_base.h"


typedef enum {
    GPIO_MODE_INPUT,           ///< Digital input mode
    GPIO_MODE_OUTPUT_PP,       ///< Digital output mode, push-pull operation
    GPIO_MODE_OUTPUT_OD,       ///< Digital output mode, open-drain operation
    GPIO_MODE_ANALOG,          ///< Analog input mode
    GPIO_MODE_ALT_FUNCTION,    ///< Alternate function mode (push-pull operation or peripheral-defined operation)
    GPIO_MODE_ALT_FUNCTION_PP, ///< Alternate function mode, push-pull operation
    GPIO_MODE_ALT_FUNCTION_OD, ///< Alternate function mode, open-drain operation
} gpio_mode_t;

typedef enum {
    GPIO_PULL_NONE, ///< No pulls
    GPIO_PULL_UP,   ///< Pull up
    GPIO_PULL_DOWN, ///< Pull down
} gpio_pull_t;

typedef enum {
    GPIO_SPEED_LOW,    ///< Low speed operation (long/slow slope)
    GPIO_SPEED_MEDIUM, ///< Medium speed operation (medium slope)
    GPIO_SPEED_HIGH,   ///< High speed operation (short/fast slope)
} gpio_speed_t;

#ifdef GPIOA
#define GPIOA_ID                            (1ul << (16 + 0))
#define PA0                                 ((gpio_pin_t)((GPIOA_ID) | (1ul <<  0)))
#define PA1                                 ((gpio_pin_t)((GPIOA_ID) | (1ul <<  1)))
#define PA2                                 ((gpio_pin_t)((GPIOA_ID) | (1ul <<  2)))
#define PA3                                 ((gpio_pin_t)((GPIOA_ID) | (1ul <<  3)))
#define PA4                                 ((gpio_pin_t)((GPIOA_ID) | (1ul <<  4)))
#define PA5                                 ((gpio_pin_t)((GPIOA_ID) | (1ul <<  5)))
#define PA6                                 ((gpio_pin_t)((GPIOA_ID) | (1ul <<  6)))
#define PA7                                 ((gpio_pin_t)((GPIOA_ID) | (1ul <<  7)))
#define PA8                                 ((gpio_pin_t)((GPIOA_ID) | (1ul <<  8)))
#define PA9                                 ((gpio_pin_t)((GPIOA_ID) | (1ul <<  9)))
#define PA10                                ((gpio_pin_t)((GPIOA_ID) | (1ul << 10)))
#define PA11                                ((gpio_pin_t)((GPIOA_ID) | (1ul << 11)))
#define PA12                                ((gpio_pin_t)((GPIOA_ID) | (1ul << 12)))
#define PA13                                ((gpio_pin_t)((GPIOA_ID) | (1ul << 13)))
#define PA14                                ((gpio_pin_t)((GPIOA_ID) | (1ul << 14)))
#define PA15                                ((gpio_pin_t)((GPIOA_ID) | (1ul << 15)))
#endif

#ifdef GPIOB
#define GPIOB_ID                            (1ul << (16 + 1))
#define PB0                                 ((gpio_pin_t)((GPIOB_ID) | (1ul <<  0)))
#define PB1                                 ((gpio_pin_t)((GPIOB_ID) | (1ul <<  1)))
#define PB2                                 ((gpio_pin_t)((GPIOB_ID) | (1ul <<  2)))
#define PB3                                 ((gpio_pin_t)((GPIOB_ID) | (1ul <<  3)))
#define PB4                                 ((gpio_pin_t)((GPIOB_ID) | (1ul <<  4)))
#define PB5                                 ((gpio_pin_t)((GPIOB_ID) | (1ul <<  5)))
#define PB6                                 ((gpio_pin_t)((GPIOB_ID) | (1ul <<  6)))
#define PB7                                 ((gpio_pin_t)((GPIOB_ID) | (1ul <<  7)))
#define PB8                                 ((gpio_pin_t)((GPIOB_ID) | (1ul <<  8)))
#define PB9                                 ((gpio_pin_t)((GPIOB_ID) | (1ul <<  9)))
#define PB10                                ((gpio_pin_t)((GPIOB_ID) | (1ul << 10)))
#define PB11                                ((gpio_pin_t)((GPIOB_ID) | (1ul << 11)))
#define PB12                                ((gpio_pin_t)((GPIOB_ID) | (1ul << 12)))
#define PB13                                ((gpio_pin_t)((GPIOB_ID) | (1ul << 13)))
#define PB14                                ((gpio_pin_t)((GPIOB_ID) | (1ul << 14)))
#define PB15                                ((gpio_pin_t)((GPIOB_ID) | (1ul << 15)))
#endif

#ifdef GPIOC
#define GPIOC_ID                            (1ul << (16 + 2))
#define PC0                                 ((gpio_pin_t)((GPIOC_ID) | (1ul <<  0)))
#define PC1                                 ((gpio_pin_t)((GPIOC_ID) | (1ul <<  1)))
#define PC2                                 ((gpio_pin_t)((GPIOC_ID) | (1ul <<  2)))
#define PC3                                 ((gpio_pin_t)((GPIOC_ID) | (1ul <<  3)))
#define PC4                                 ((gpio_pin_t)((GPIOC_ID) | (1ul <<  4)))
#define PC5                                 ((gpio_pin_t)((GPIOC_ID) | (1ul <<  5)))
#define PC6                                 ((gpio_pin_t)((GPIOC_ID) | (1ul <<  6)))
#define PC7                                 ((gpio_pin_t)((GPIOC_ID) | (1ul <<  7)))
#define PC8                                 ((gpio_pin_t)((GPIOC_ID) | (1ul <<  8)))
#define PC9                                 ((gpio_pin_t)((GPIOC_ID) | (1ul <<  9)))
#define PC10                                ((gpio_pin_t)((GPIOC_ID) | (1ul << 10)))
#define PC11                                ((gpio_pin_t)((GPIOC_ID) | (1ul << 11)))
#define PC12                                ((gpio_pin_t)((GPIOC_ID) | (1ul << 12)))
#define PC13                                ((gpio_pin_t)((GPIOC_ID) | (1ul << 13)))
#define PC14                                ((gpio_pin_t)((GPIOC_ID) | (1ul << 14)))
#define PC15                                ((gpio_pin_t)((GPIOC_ID) | (1ul << 15)))
#endif

#ifdef GPIOD
#define GPIOD_ID                            (1ul << (16 + 3))
#define PD0                                 ((gpio_pin_t)((GPIOD_ID) | (1ul <<  0)))
#define PD1                                 ((gpio_pin_t)((GPIOD_ID) | (1ul <<  1)))
#define PD2                                 ((gpio_pin_t)((GPIOD_ID) | (1ul <<  2)))
#define PD3                                 ((gpio_pin_t)((GPIOD_ID) | (1ul <<  3)))
#define PD4                                 ((gpio_pin_t)((GPIOD_ID) | (1ul <<  4)))
#define PD5                                 ((gpio_pin_t)((GPIOD_ID) | (1ul <<  5)))
#define PD6                                 ((gpio_pin_t)((GPIOD_ID) | (1ul <<  6)))
#define PD7                                 ((gpio_pin_t)((GPIOD_ID) | (1ul <<  7)))
#define PD8                                 ((gpio_pin_t)((GPIOD_ID) | (1ul <<  8)))
#define PD9                                 ((gpio_pin_t)((GPIOD_ID) | (1ul <<  9)))
#define PD10                                ((gpio_pin_t)((GPIOD_ID) | (1ul << 10)))
#define PD11                                ((gpio_pin_t)((GPIOD_ID) | (1ul << 11)))
#define PD12                                ((gpio_pin_t)((GPIOD_ID) | (1ul << 12)))
#define PD13                                ((gpio_pin_t)((GPIOD_ID) | (1ul << 13)))
#define PD14                                ((gpio_pin_t)((GPIOD_ID) | (1ul << 14)))
#define PD15                                ((gpio_pin_t)((GPIOD_ID) | (1ul << 15)))
#endif

#ifdef GPIOE
#define GPIOE_ID                            (1ul << (16 + 4))
#define PE0                                 ((gpio_pin_t)((GPIOE_ID) | (1ul <<  0)))
#define PE1                                 ((gpio_pin_t)((GPIOE_ID) | (1ul <<  1)))
#define PE2                                 ((gpio_pin_t)((GPIOE_ID) | (1ul <<  2)))
#define PE3                                 ((gpio_pin_t)((GPIOE_ID) | (1ul <<  3)))
#define PE4                                 ((gpio_pin_t)((GPIOE_ID) | (1ul <<  4)))
#define PE5                                 ((gpio_pin_t)((GPIOE_ID) | (1ul <<  5)))
#define PE6                                 ((gpio_pin_t)((GPIOE_ID) | (1ul <<  6)))
#define PE7                                 ((gpio_pin_t)((GPIOE_ID) | (1ul <<  7)))
#define PE8                                 ((gpio_pin_t)((GPIOE_ID) | (1ul <<  8)))
#define PE9                                 ((gpio_pin_t)((GPIOE_ID) | (1ul <<  9)))
#define PE10                                ((gpio_pin_t)((GPIOE_ID) | (1ul << 10)))
#define PE11                                ((gpio_pin_t)((GPIOE_ID) | (1ul << 11)))
#define PE12                                ((gpio_pin_t)((GPIOE_ID) | (1ul << 12)))
#define PE13                                ((gpio_pin_t)((GPIOE_ID) | (1ul << 13)))
#define PE14                                ((gpio_pin_t)((GPIOE_ID) | (1ul << 14)))
#define PE15                                ((gpio_pin_t)((GPIOE_ID) | (1ul << 15)))
#endif

#ifdef GPIOF
#define GPIOF_ID                            (1ul << (16 + 5))
#define PF0                                 ((gpio_pin_t)((GPIOF_ID) | (1ul <<  0)))
#define PF1                                 ((gpio_pin_t)((GPIOF_ID) | (1ul <<  1)))
#define PF2                                 ((gpio_pin_t)((GPIOF_ID) | (1ul <<  2)))
#define PF3                                 ((gpio_pin_t)((GPIOF_ID) | (1ul <<  3)))
#define PF4                                 ((gpio_pin_t)((GPIOF_ID) | (1ul <<  4)))
#define PF5                                 ((gpio_pin_t)((GPIOF_ID) | (1ul <<  5)))
#define PF6                                 ((gpio_pin_t)((GPIOF_ID) | (1ul <<  6)))
#define PF7                                 ((gpio_pin_t)((GPIOF_ID) | (1ul <<  7)))
#define PF8                                 ((gpio_pin_t)((GPIOF_ID) | (1ul <<  8)))
#define PF9                                 ((gpio_pin_t)((GPIOF_ID) | (1ul <<  9)))
#define PF10                                ((gpio_pin_t)((GPIOF_ID) | (1ul << 10)))
#define PF11                                ((gpio_pin_t)((GPIOF_ID) | (1ul << 11)))
#define PF12                                ((gpio_pin_t)((GPIOF_ID) | (1ul << 12)))
#define PF13                                ((gpio_pin_t)((GPIOF_ID) | (1ul << 13)))
#define PF14                                ((gpio_pin_t)((GPIOF_ID) | (1ul << 14)))
#define PF15                                ((gpio_pin_t)((GPIOF_ID) | (1ul << 15)))
#endif

#ifdef GPIOG
#define GPIOG_ID                            (1ul << (16 + 6))
#define PG0                                 ((gpio_pin_t)((GPIOG_ID) | (1ul <<  0)))
#define PG1                                 ((gpio_pin_t)((GPIOG_ID) | (1ul <<  1)))
#define PG2                                 ((gpio_pin_t)((GPIOG_ID) | (1ul <<  2)))
#define PG3                                 ((gpio_pin_t)((GPIOG_ID) | (1ul <<  3)))
#define PG4                                 ((gpio_pin_t)((GPIOG_ID) | (1ul <<  4)))
#define PG5                                 ((gpio_pin_t)((GPIOG_ID) | (1ul <<  5)))
#define PG6                                 ((gpio_pin_t)((GPIOG_ID) | (1ul <<  6)))
#define PG7                                 ((gpio_pin_t)((GPIOG_ID) | (1ul <<  7)))
#define PG8                                 ((gpio_pin_t)((GPIOG_ID) | (1ul <<  8)))
#define PG9                                 ((gpio_pin_t)((GPIOG_ID) | (1ul <<  9)))
#define PG10                                ((gpio_pin_t)((GPIOG_ID) | (1ul << 10)))
#define PG11                                ((gpio_pin_t)((GPIOG_ID) | (1ul << 11)))
#define PG12                                ((gpio_pin_t)((GPIOG_ID) | (1ul << 12)))
#define PG13                                ((gpio_pin_t)((GPIOG_ID) | (1ul << 13)))
#define PG14                                ((gpio_pin_t)((GPIOG_ID) | (1ul << 14)))
#define PG15                                ((gpio_pin_t)((GPIOG_ID) | (1ul << 15)))
#endif

#ifdef GPIOH
#define GPIOH_ID                            (1ul << (16 + 7))
#define PH0                                 ((gpio_pin_t)((GPIOH_ID) | (1ul <<  0)))
#define PH1                                 ((gpio_pin_t)((GPIOH_ID) | (1ul <<  1)))
#define PH2                                 ((gpio_pin_t)((GPIOH_ID) | (1ul <<  2)))
#define PH3                                 ((gpio_pin_t)((GPIOH_ID) | (1ul <<  3)))
#define PH4                                 ((gpio_pin_t)((GPIOH_ID) | (1ul <<  4)))
#define PH5                                 ((gpio_pin_t)((GPIOH_ID) | (1ul <<  5)))
#define PH6                                 ((gpio_pin_t)((GPIOH_ID) | (1ul <<  6)))
#define PH7                                 ((gpio_pin_t)((GPIOH_ID) | (1ul <<  7)))
#define PH8                                 ((gpio_pin_t)((GPIOH_ID) | (1ul <<  8)))
#define PH9                                 ((gpio_pin_t)((GPIOH_ID) | (1ul <<  9)))
#define PH10                                ((gpio_pin_t)((GPIOH_ID) | (1ul << 10)))
#define PH11                                ((gpio_pin_t)((GPIOH_ID) | (1ul << 11)))
#define PH12                                ((gpio_pin_t)((GPIOH_ID) | (1ul << 12)))
#define PH13                                ((gpio_pin_t)((GPIOH_ID) | (1ul << 13)))
#define PH14                                ((gpio_pin_t)((GPIOH_ID) | (1ul << 14)))
#define PH15                                ((gpio_pin_t)((GPIOH_ID) | (1ul << 15)))
#endif

#ifdef GPIOI
#define GPIOI_ID                            (1ul << (16 + 8))
#define PI0                                 ((gpio_pin_t)((GPIOI_ID) | (1ul <<  0)))
#define PI1                                 ((gpio_pin_t)((GPIOI_ID) | (1ul <<  1)))
#define PI2                                 ((gpio_pin_t)((GPIOI_ID) | (1ul <<  2)))
#define PI3                                 ((gpio_pin_t)((GPIOI_ID) | (1ul <<  3)))
#define PI4                                 ((gpio_pin_t)((GPIOI_ID) | (1ul <<  4)))
#define PI5                                 ((gpio_pin_t)((GPIOI_ID) | (1ul <<  5)))
#define PI6                                 ((gpio_pin_t)((GPIOI_ID) | (1ul <<  6)))
#define PI7                                 ((gpio_pin_t)((GPIOI_ID) | (1ul <<  7)))
#define PI8                                 ((gpio_pin_t)((GPIOI_ID) | (1ul <<  8)))
#define PI9                                 ((gpio_pin_t)((GPIOI_ID) | (1ul <<  9)))
#define PI10                                ((gpio_pin_t)((GPIOI_ID) | (1ul << 10)))
#define PI11                                ((gpio_pin_t)((GPIOI_ID) | (1ul << 11)))
#define PI12                                ((gpio_pin_t)((GPIOI_ID) | (1ul << 12)))
#define PI13                                ((gpio_pin_t)((GPIOI_ID) | (1ul << 13)))
#define PI14                                ((gpio_pin_t)((GPIOI_ID) | (1ul << 14)))
#define PI15                                ((gpio_pin_t)((GPIOI_ID) | (1ul << 15)))
#endif

#ifdef GPIOJ
#define GPIOJ_ID                            (1ul << (16 + 9))
#define PJ0                                 ((gpio_pin_t)((GPIOJ_ID) | (1ul <<  0)))
#define PJ1                                 ((gpio_pin_t)((GPIOJ_ID) | (1ul <<  1)))
#define PJ2                                 ((gpio_pin_t)((GPIOJ_ID) | (1ul <<  2)))
#define PJ3                                 ((gpio_pin_t)((GPIOJ_ID) | (1ul <<  3)))
#define PJ4                                 ((gpio_pin_t)((GPIOJ_ID) | (1ul <<  4)))
#define PJ5                                 ((gpio_pin_t)((GPIOJ_ID) | (1ul <<  5)))
#define PJ6                                 ((gpio_pin_t)((GPIOJ_ID) | (1ul <<  6)))
#define PJ7                                 ((gpio_pin_t)((GPIOJ_ID) | (1ul <<  7)))
#define PJ8                                 ((gpio_pin_t)((GPIOJ_ID) | (1ul <<  8)))
#define PJ9                                 ((gpio_pin_t)((GPIOJ_ID) | (1ul <<  9)))
#define PJ10                                ((gpio_pin_t)((GPIOJ_ID) | (1ul << 10)))
#define PJ11                                ((gpio_pin_t)((GPIOJ_ID) | (1ul << 11)))
#define PJ12                                ((gpio_pin_t)((GPIOJ_ID) | (1ul << 12)))
#define PJ13                                ((gpio_pin_t)((GPIOJ_ID) | (1ul << 13)))
#define PJ14                                ((gpio_pin_t)((GPIOJ_ID) | (1ul << 14)))
#define PJ15                                ((gpio_pin_t)((GPIOJ_ID) | (1ul << 15)))
#endif

#ifdef GPIOK
#define GPIOK_ID                            (1ul << (16 + 10))
#define PK0                                 ((gpio_pin_t)((GPIOK_ID) | (1ul <<  0)))
#define PK1                                 ((gpio_pin_t)((GPIOK_ID) | (1ul <<  1)))
#define PK2                                 ((gpio_pin_t)((GPIOK_ID) | (1ul <<  2)))
#define PK3                                 ((gpio_pin_t)((GPIOK_ID) | (1ul <<  3)))
#define PK4                                 ((gpio_pin_t)((GPIOK_ID) | (1ul <<  4)))
#define PK5                                 ((gpio_pin_t)((GPIOK_ID) | (1ul <<  5)))
#define PK6                                 ((gpio_pin_t)((GPIOK_ID) | (1ul <<  6)))
#define PK7                                 ((gpio_pin_t)((GPIOK_ID) | (1ul <<  7)))
#define PK8                                 ((gpio_pin_t)((GPIOK_ID) | (1ul <<  8)))
#define PK9                                 ((gpio_pin_t)((GPIOK_ID) | (1ul <<  9)))
#define PK10                                ((gpio_pin_t)((GPIOK_ID) | (1ul << 10)))
#define PK11                                ((gpio_pin_t)((GPIOK_ID) | (1ul << 11)))
#define PK12                                ((gpio_pin_t)((GPIOK_ID) | (1ul << 12)))
#define PK13                                ((gpio_pin_t)((GPIOK_ID) | (1ul << 13)))
#define PK14                                ((gpio_pin_t)((GPIOK_ID) | (1ul << 14)))
#define PK15                                ((gpio_pin_t)((GPIOK_ID) | (1ul << 15)))
#endif

typedef uint32_t gpio_pin_t;



extern void gpio_config_pins(gpio_pin_t pins, gpio_mode_t mode, gpio_pull_t pull, gpio_speed_t speed, uint32_t alt_func, bool output_state);
extern peripheral_t gpio_get_peripheral(gpio_pin_t pin);

extern void gpio_set_pins(gpio_pin_t pins);
extern void gpio_reset_pins(gpio_pin_t pins);

extern gpio_pin_t gpio_read_pins(gpio_pin_t pins);




#endif // _GPIO_H_
