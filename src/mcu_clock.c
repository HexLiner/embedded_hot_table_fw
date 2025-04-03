//  ***************************************************************************
/// @file    mcu_clock.c
/// @author  Techart Micro Systems (DM)
//  ***************************************************************************

#include "mcu_clock.h"
#include <stdbool.h>
#include <stdint.h>
#include "common/mcu.h"
#include "common/error.h"
#include "hal/sysclk/sysclk.h"
#include "hal/systimer/systimer.h"
#include "error_handling.h"


#define SYSCLK_SOURCE_NOTREADY_TIMEOUT_MS (100)

static const sysclk_extcfg_t sysclk_extcfg = {
    .lse_freq_hz      = 1,
    .hse_freq_hz      = 16'000'000,
};




//  ***************************************************************************
/// @brief      Set normal configuration for MCU clock
/// @param      none
/// @return     error_t
/// @note       If this function returned error, then system is now running with safe clock configuration.
//  ***************************************************************************
error_t mcu_clock_set_normal_config(void) {
    timer_t  timeout;
    uint32_t rcc_cfgr_new_value;
    bool is_hse_error;


    mcu_clock_set_safe_config();


    // Set flash latency (slowest)
    FLASH->ACR |= 1 << FLASH_ACR_LATENCY_Pos;  // p.59

    // Clock security system enable
    RCC->CR |= RCC_CR_CSSON;

    // Enable HSE
    RCC->CR &= ~RCC_CR_HSEBYP_Msk;
    RCC->CR |= RCC_CR_HSEON;
    // Wait for HSE ready
    is_hse_error = false;
    timeout = timer_start_ms(SYSCLK_SOURCE_NOTREADY_TIMEOUT_MS);
    while (!(RCC->CR & RCC_CR_HSERDY)) {
        if (timer_triggered(timeout)) {
            // HSE doesn't work
            is_hse_error = true;
            eh_set_fail_ext_oscillator_error();
            break;
        }
    }


    // Set PLL config
    // PLLCLK = PLLsrc / PREDIV * PLLMUL
    //     PREDIV = 0 -> input clock not divided
    //     PREDIV = 1 -> input clock divided by 2
    //     ...
    //     PREDIV = 15 -> input clock divided by 16
    //     PLLMUL = 0 -> PLL input clock x 2
    //     ...
    //     PLLMUL = 14 -> PLL input clock x 16
    RCC->CFGR &= ~(RCC_CFGR_PLLMUL_Msk | RCC_CFGR_PLLXTPRE_Msk | RCC_CFGR_PLLSRC_Msk);
    if (!is_hse_error) {
        // PLLCLK = HSE / 2 * 6 = 48 MHz

        RCC->CFGR2 = 1 << RCC_CFGR2_PREDIV_Pos;
        RCC->CFGR |= (PLL_SRC_HSE << RCC_CFGR_PLLSRC_Pos) |
                     (4 << RCC_CFGR_PLLMUL_Pos);
    }
    else {
        // PLLCLK = HSI / 1 * 6 = 48 MHz

        RCC->CFGR2 = 0 << RCC_CFGR2_PREDIV_Pos;
        RCC->CFGR |= (PLL_SRC_HSI << RCC_CFGR_PLLSRC_Pos) |
                     (4 << RCC_CFGR_PLLMUL_Pos);
    }

    // Enable PLL
    RCC->CR |= RCC_CR_PLLON;
    // Wait for PLL ready
    timeout = timer_start_ms(SYSCLK_SOURCE_NOTREADY_TIMEOUT_MS);
    while (!(RCC->CR & RCC_CR_PLLRDY)) {
        if (timer_triggered(timeout)) {
            // PLL doesn't work
            eh_set_fail_fw_error(E_FAILED);
            mcu_clock_set_safe_config();
            return E_FAILED;
        }
    }


    // Switch SYSCLK to PLLCLK
    SysTick->CTRL &= ~SysTick_CTRL_ENABLE_Msk;   // Disable SysTick for safely switch SYSCLK source (see #12681-2)
    rcc_cfgr_new_value = RCC->CFGR & ~(RCC_CFGR_SW_Msk | RCC_CFGR_HPRE_Msk | RCC_CFGR_PPRE_Msk);
    rcc_cfgr_new_value |= (SYSCLK_SRC_PLLCLK << RCC_CFGR_SW_Pos);      // = 48 MHz
    // Set buses prescaler
    rcc_cfgr_new_value |= (0 << RCC_CFGR_HPRE_Pos) |   // AHB = 48 / 1 = 48 MHz(MAX)
                          (0 << RCC_CFGR_PPRE_Pos);    // APB = AHB / 1 = 48 MHz(MAX)
    RCC->CFGR = rcc_cfgr_new_value;

    systimer_init();
    SysTick->CTRL |= SysTick_CTRL_ENABLE_Msk;    // Enable SysTick
    // Wait for SW switch to PLL
    timeout = timer_start_ms(SYSCLK_SOURCE_NOTREADY_TIMEOUT_MS);
    while (((RCC->CFGR & RCC_CFGR_SWS) >> RCC_CFGR_SWS_Pos) != SYSCLK_SRC_PLLCLK) {
        if (timer_triggered(timeout)) {
            // SW doesn't switch
            eh_set_fail_fw_error(E_FAILED);
            mcu_clock_set_safe_config();
            return E_FAILED;
        }
    }

    // Set proper flash latency
    FLASH->ACR = 1 << FLASH_ACR_LATENCY_Pos;  // p.59 - One wait state, if 24 MHz < SYSCLK ≤ 48 MHz

    // Set peripherals clock sources
    RCC->CFGR3 &= ~(RCC_CFGR3_I2C1SW_Msk | RCC_CFGR3_USART1SW_Msk | RCC_CFGR3_USBSW_Msk);
    RCC->CFGR3 |= (I2C1_SRC_SYSCLK << RCC_CFGR3_I2C1SW_Pos) |
                  (UART1_SRC_APB << RCC_CFGR3_USART1SW_Pos) |
                  (USB_SRC_PLLCLK << RCC_CFGR3_USBSW_Pos);

    NVIC_EnableIRQ(RCC_IRQn);

    return E_OK;
}


//  ***************************************************************************
/// @brief      Set safe configuration for MCU clock
/// @param      none
/// @return     none
/// @note       Puts system to use internal clock source. Should be called if external clock isn't work.
//  ***************************************************************************
void mcu_clock_set_safe_config() {
    uint32_t rcc_cfgr_new_value;


    sysclk_init(&sysclk_extcfg);
    systimer_init();

    // Set flash latency (slowest)
    FLASH->ACR |= 1 << FLASH_ACR_LATENCY_Pos;  // p.59

    // Enable HSI
    RCC->CR |= RCC_CR_HSION;
    // Wait for HSI ready
    while (!(RCC->CR & RCC_CR_HSIRDY));

    // Switch SYSCLK to HSI
    SysTick->CTRL &= ~SysTick_CTRL_ENABLE_Msk;   // Disable SysTick for safely switch SYSCLK source (see #12681-2)
    rcc_cfgr_new_value = RCC->CFGR & ~(RCC_CFGR_SW_Msk | RCC_CFGR_HPRE_Msk | RCC_CFGR_PPRE_Msk);
    rcc_cfgr_new_value |= (RCC_CFGR_SW_HSI << RCC_CFGR_SW_Pos);      // = 8 MHz
    // Set buses prescaler
    rcc_cfgr_new_value |= (0 << RCC_CFGR_HPRE_Pos) |   // AHB = 8 / 1 = 8 MHz
                          (0 << RCC_CFGR_PPRE_Pos);    // APB = AHB / 1 = 8 MHz
    RCC->CFGR = rcc_cfgr_new_value;
    // Wait for SW switch to HSI
    while (((RCC->CFGR & RCC_CFGR_SWS) >> RCC_CFGR_SWS_Pos) != RCC_CFGR_SW_HSI);

    systimer_init();
    SysTick->CTRL |= SysTick_CTRL_ENABLE_Msk;    // Enable SysTick

    // Set proper flash latency
    FLASH->ACR = 0 << FLASH_ACR_LATENCY_Pos;

    // Set peripherals clock sources
    RCC->CFGR3 &= ~(RCC_CFGR3_I2C1SW_Msk | RCC_CFGR3_USART1SW_Msk | RCC_CFGR3_USBSW_Msk);
    RCC->CFGR3 |= (I2C1_SRC_SYSCLK << RCC_CFGR3_I2C1SW_Pos) |
                  (UART1_SRC_APB << RCC_CFGR3_USART1SW_Pos) | 
                  (USB_SRC_NONE << RCC_CFGR3_USBSW_Pos);;

    // Disable HSE and PLLs
    RCC->CR &= ~(RCC_CR_PLLON_Msk | RCC_CR_HSEON_Msk);
    // Clock security system disable
    RCC->CR &= RCC_CR_CSSON;
}





void RCC_IRQHandler(void);
void RCC_IRQHandler(void) {
    // HSE error interrupt
    if (RCC->CIR & RCC_CIR_CSSF) {
        eh_set_fail_ext_oscillator_error();
        mcu_clock_set_normal_config();
    }

    RCC->CIR = 0xFFFF;   // Clear flags
}
