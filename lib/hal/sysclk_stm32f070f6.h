//  ***************************************************************************
/// @file    sysclk_stm32f070f6.h
/// @brief   System clock driver - sysclk_stm32f070f6
//  ***************************************************************************
#ifndef _SYSCLK_STM32F070F6_H_
#define _SYSCLK_STM32F070F6_H_

#include <stdint.h>

// Internal clock sources
#define SYSCLK_HSI_FREQ_HZ     (8 * 1000000)
#define SYSCLK_LSI_FREQ_HZ     (32 * 1000)

// sw
#define SYSCLK_SRC_HSI         (0)
#define SYSCLK_SRC_HSE         (1)
#define SYSCLK_SRC_PLLCLK      (2)

// pll src
#define PLL_SRC_HSI_DIV2       (0)
#define PLL_SRC_HSI            (1)
#define PLL_SRC_HSE            (2)

// USART 1 src
#define UART1_SRC_APB          (0x00)
#define UART1_SRC_SYSCLK       (0x01)
#define UART1_SRC_LSE          (0x02)
#define UART1_SRC_HSI          (0x03)

// I2C 1 src
#define I2C1_SRC_HSI          (0x00)
#define I2C1_SRC_SYSCLK       (0x01)

// USB src
#define USB_SRC_NONE          (0x00)
#define USB_SRC_PLLCLK        (0x01)


#define SYSCLK_PART            (0x10)
#define COMMON_PART            (0x20)
#define PLL_PART               (0x40)

typedef enum {
    SYSCLK_HSI        = (COMMON_PART | 0x00),
    SYSCLK_LSI        = (COMMON_PART | 0x01),
    SYSCLK_HSE        = (COMMON_PART | 0x02),
    SYSCLK_LSE        = (COMMON_PART | 0x03),
    SYSCLK_SYSCLK     = (SYSCLK_PART | 0x00),
    SYSCLK_CORE       = (SYSCLK_PART | 0x01),
    SYSCLK_BUS_AHB    = (SYSCLK_PART | 0x02),
    SYSCLK_BUS_APB1   = (SYSCLK_PART | 0x03),
    SYSCLK_BUS_APB2   = (SYSCLK_PART | 0x04),
    SYSCLK_PLL        = (PLL_PART    | 0x00)
} sysclk_bus_t;


typedef struct {
    uint32_t lse_freq_hz;
    uint32_t hse_freq_hz;    // 4.000.000 - 32.000.000 Hz
} sysclk_extcfg_t;


#endif // _SYSCLK_STM32F070F6_H_
