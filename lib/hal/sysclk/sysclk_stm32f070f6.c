//  ***************************************************************************
/// @file    sysclk_stm32f070f6.c
/// @author  Techart Micro Systems (DM)
//  ***************************************************************************
#include "hal/sysclk/sysclk.h"
#include <stdint.h>
#include <stdbool.h>
#include "common/mcu.h"
#include "common/lib_base.h"
#include "common/error.h"


// See DS
#define HSE_MIN_FREQ_HZ (1  * 1000000)
#define HSE_MAX_FREQ_HZ (32 * 1000000)
#define LSE_MIN_FREQ_HZ (1)
#define LSE_MAX_FREQ_HZ (32  * 1000)


typedef struct {
    const sysclk_extcfg_t *extcfg;
} sysclk_internal_t;


static sysclk_internal_t sysclk_internal;


static void sysclk_get_peripheral_enr(const peripheral_t peripheral, volatile uint32_t **reg, uint32_t *pos);
static uint32_t sysclk_get_common_bus_freq(sysclk_bus_t bus);
static uint32_t sysclk_get_sysclk_bus_freq(sysclk_bus_t bus);
static uint32_t sysclk_get_pll_bus_freq(sysclk_bus_t bus);



//  ***************************************************************************
/// @brief      Sysclk init
/// @param      ext_cfg
/// @retval     none
/// @return     none
//  ***************************************************************************
void sysclk_init(const sysclk_extcfg_t *ext_cfg) {
    sysclk_internal.extcfg = ext_cfg;

    // Check HSE valid (or disabled)
    if (sysclk_internal.extcfg->hse_freq_hz != 0) {
        if (sysclk_internal.extcfg->hse_freq_hz < HSE_MIN_FREQ_HZ) ERROR_FATAL(sysclk_init, __LINE__);
        if (sysclk_internal.extcfg->hse_freq_hz > HSE_MAX_FREQ_HZ) ERROR_FATAL(sysclk_init, __LINE__);
    }
    // Check LSE valid (or disabled)
    if (sysclk_internal.extcfg->lse_freq_hz != 0) {
        if (sysclk_internal.extcfg->lse_freq_hz < LSE_MIN_FREQ_HZ) ERROR_FATAL(sysclk_init, __LINE__);
        if (sysclk_internal.extcfg->lse_freq_hz > LSE_MAX_FREQ_HZ) ERROR_FATAL(sysclk_init, __LINE__);
    }
}


static void sysclk_get_peripheral_enr(const peripheral_t peripheral, volatile uint32_t **reg, uint32_t *pos) {
    *pos = UINT32_MAX;

    *reg = &(RCC->AHBENR);
    switch ((uintptr_t)peripheral) {
        case (uintptr_t)GPIOF:         *pos = RCC_AHBENR_GPIOFEN;  return;
        case (uintptr_t)GPIOD:         *pos = RCC_AHBENR_GPIODEN;  return;
        case (uintptr_t)GPIOC:         *pos = RCC_AHBENR_GPIOCEN;  return;
        case (uintptr_t)GPIOB:         *pos = RCC_AHBENR_GPIOBEN;  return;
        case (uintptr_t)GPIOA:         *pos = RCC_AHBENR_GPIOAEN;  return;
        case (uintptr_t)CRC:           *pos = RCC_AHBENR_CRCEN;    return;
        case (uintptr_t)DMA1:          *pos = RCC_AHBENR_DMAEN;    return;
    }

    *reg = &(RCC->APB2ENR);
    switch ((uintptr_t)peripheral) {
        case (uintptr_t)SYSCFG:        *pos = RCC_APB2ENR_SYSCFGEN;  return;
        case (uintptr_t)ADC1:          *pos = RCC_APB2ENR_ADCEN;     return;
        case (uintptr_t)TIM1:          *pos = RCC_APB2ENR_TIM1EN;    return;
        case (uintptr_t)SPI1:          *pos = RCC_APB2ENR_SPI1EN;    return;
        case (uintptr_t)USART1:        *pos = RCC_APB2ENR_USART1EN;  return;
        case (uintptr_t)TIM16:         *pos = RCC_APB2ENR_TIM16EN;   return;
        case (uintptr_t)TIM17:         *pos = RCC_APB2ENR_TIM17EN;   return;
    }

    *reg = &(RCC->APB1ENR);
    switch ((uintptr_t)peripheral) {
        case (uintptr_t)TIM3:          *pos = RCC_APB1ENR_TIM3EN;    return;
        case (uintptr_t)TIM14:         *pos = RCC_APB1ENR_TIM14EN;   return;
        case (uintptr_t)USART2:        *pos = RCC_APB1ENR_USART2EN;  return;
        case (uintptr_t)I2C1:          *pos = RCC_APB1ENR_I2C1EN;    return;
        case (uintptr_t)USB:           *pos = RCC_APB1ENR_USBEN;     return;
        case (uintptr_t)PWR:           *pos = RCC_APB1ENR_PWREN;     return;
    }

    ////if (*pos == UINT32_MAX) err
}


//  ***************************************************************************
/// @brief      Enable peripheral
/// @param      peripheral
/// @return     none
//  ***************************************************************************
void sysclk_enable_peripheral(const peripheral_t peripheral) {
    volatile uint32_t *reg;
    uint32_t msk;
    sysclk_get_peripheral_enr(peripheral, &reg, &msk);
    *reg |= msk;
}


//  ***************************************************************************
/// @brief      Disable peripheral
/// @param      peripheral
/// @return     none
//  ***************************************************************************
void sysclk_disable_peripheral(const peripheral_t peripheral) {
    volatile uint32_t *reg;
    uint32_t msk;
    sysclk_get_peripheral_enr(peripheral, &reg, &msk);
    *reg &= ~msk;
}


//  ***************************************************************************
/// @brief      Get bus frequency
/// @param      bus
/// @param      freq
/// @retval     freq - bus frequency in Hz
/// @return     none
//  ***************************************************************************
void sysclk_get_bus_freq(sysclk_bus_t bus, uint32_t *freq) {
    if (bus & COMMON_PART) {
        *freq = sysclk_get_common_bus_freq(bus);
    }
    else if (bus & SYSCLK_PART) {
        *freq = sysclk_get_sysclk_bus_freq(bus);
    }
    else if (bus & PLL_PART) {
        *freq = sysclk_get_pll_bus_freq(bus);
    }
}



//  ***************************************************************************
/// @brief      Get peripheral frequency
/// @param      peripheral
/// @param      freq
/// @retval     freq - peripheral frequency in Hz
/// @return     none
//  ***************************************************************************
void sysclk_get_peripheral_freq(const peripheral_t peripheral, uint32_t *freq) {
    uint32_t peripheral_src;
    uint32_t peripheral_freq;
    bool     is_ahb_peripheral;
    bool     is_apb1_peripheral, is_apb2_peripheral;
    bool     is_apb1_timer, is_apb2_timer;


    is_ahb_peripheral =  (peripheral == SysTick   )   ||
                         (peripheral == GPIOA     )   ||
                         (peripheral == GPIOB     )   ||
                         (peripheral == GPIOC     )   ||
                         (peripheral == GPIOD     )   ||
                         (peripheral == GPIOF     )   ||
                         (peripheral == CRC       )   ||
                         (peripheral == DMA1      );

    is_apb2_peripheral = (peripheral == ADC       )   ||
                         (peripheral == SPI1      );

    is_apb2_timer      = (peripheral == TIM1      )   ||
                         (peripheral == TIM16     )   ||
                         (peripheral == TIM17     );

    is_apb1_peripheral = (peripheral == USART2);

    is_apb1_timer      = (peripheral == TIM3      )   ||
                         (peripheral == TIM14     );


    peripheral_freq = 0;
    if (is_ahb_peripheral) {
        sysclk_get_bus_freq(SYSCLK_BUS_AHB, &peripheral_freq);
    }
    else if (is_apb1_peripheral) {
        sysclk_get_bus_freq(SYSCLK_BUS_APB1, &peripheral_freq);
    }
    else if (is_apb2_peripheral) {
        sysclk_get_bus_freq(SYSCLK_BUS_APB2, &peripheral_freq);
    }
    else if (is_apb1_timer || is_apb2_timer) {
        sysclk_get_bus_freq(SYSCLK_BUS_APB1, &peripheral_freq);
        if (((RCC->CFGR & RCC_CFGR_PPRE) >> RCC_CFGR_PPRE_Pos) > 0x03) peripheral_freq = peripheral_freq * 2;
    }


    // USART 1
    if (peripheral == USART1) {
        peripheral_src = (RCC->CFGR3 & RCC_CFGR3_USART1SW) >> RCC_CFGR3_USART1SW_Pos;
        switch (peripheral_src) {
            case UART1_SRC_APB:
                sysclk_get_bus_freq(SYSCLK_BUS_APB1, &peripheral_freq);
                break;
            case UART1_SRC_SYSCLK:
                sysclk_get_bus_freq(SYSCLK_SYSCLK, &peripheral_freq);
                break;
            case UART1_SRC_LSE:
                sysclk_get_bus_freq(SYSCLK_LSE, &peripheral_freq);
                break;
            case UART1_SRC_HSI:
                sysclk_get_bus_freq(SYSCLK_HSI, &peripheral_freq);
                break;
        }
    }
    // I2C1
    if (peripheral == I2C1) {
        peripheral_src = (RCC->CFGR3 & RCC_CFGR3_I2C1SW) >> RCC_CFGR3_I2C1SW_Pos;
        switch (peripheral_src) {
            case I2C1_SRC_HSI:
                sysclk_get_bus_freq(SYSCLK_HSI, &peripheral_freq);
                break;
            case I2C1_SRC_SYSCLK:
                sysclk_get_bus_freq(SYSCLK_SYSCLK, &peripheral_freq);
                break;
        }
    }

    *freq = peripheral_freq;
}




//  ***************************************************************************
/// @brief      Get common bus frequency
/// @param      bus
/// @retval     none
/// @return     common bus frequency in Hz
//  ***************************************************************************
static uint32_t sysclk_get_common_bus_freq(sysclk_bus_t bus) {
    uint32_t bus_freq;


    if (bus == SYSCLK_HSI) {
        if (!(RCC->CR & RCC_CR_HSIRDY)) {
            bus_freq = 0;
        }
        else {
            bus_freq = SYSCLK_HSI_FREQ_HZ;
        }
    }
    if (bus == SYSCLK_LSI) {
        if (!(RCC->CSR & RCC_CSR_LSIRDY)) {
            bus_freq = 0;
        }
        else {
            bus_freq = SYSCLK_LSI_FREQ_HZ;
        }
    }
    else if (bus == SYSCLK_HSE) {
        if (!(RCC->CR & RCC_CR_HSERDY)) {
            bus_freq = 0;
        }
        else {
            bus_freq = sysclk_internal.extcfg->hse_freq_hz;
        }
    }
    else if (bus == SYSCLK_LSE) {
        if (!(RCC->BDCR & RCC_BDCR_LSERDY)) {
            bus_freq = 0;
        }
        else {
            bus_freq = sysclk_internal.extcfg->lse_freq_hz;
        }
    }

    return bus_freq;
}


//  ***************************************************************************
/// @brief      Get bus frequency
/// @param      bus
/// @retval     none
/// @return     Bus frequency in Hz
//  ***************************************************************************
static uint32_t sysclk_get_sysclk_bus_freq(sysclk_bus_t bus) {
    uint32_t sysclk_src;
    uint32_t sysclk, ahb_freq;
    uint32_t ahb_hpre, apbx_ppre;
    uint32_t divider;
    uint32_t bus_freq;


    // Define SYSCLK source
    sysclk_src = RCC->CFGR & RCC_CFGR_SWS;
    // Define SYSCLK freq
    if (sysclk_src == RCC_CFGR_SWS_HSI) {
        sysclk = sysclk_get_common_bus_freq(SYSCLK_HSI);
    }
    else if (sysclk_src == RCC_CFGR_SWS_HSE) {
        sysclk = sysclk_get_common_bus_freq(SYSCLK_HSE);
    }
    else if (sysclk_src == RCC_CFGR_SWS_PLL) {
        sysclk = sysclk_get_pll_bus_freq(SYSCLK_PLL);
    }


    // Calc AHB freq
    // Convert AHB divider
    ahb_hpre = (RCC->CFGR & RCC_CFGR_HPRE) >> RCC_CFGR_HPRE_Pos;
    if (ahb_hpre < 0x08) {
        divider = 1;
    }
    // 2, 4, 8, 16
    else if (ahb_hpre < 0x0C) {
        divider = 1 << ((ahb_hpre & 0x07) + 1);
    }
    // 64, 128, 256, 512
    else {
        divider = 1 << ((ahb_hpre & 0x07) + 2);
    }
    ahb_freq = sysclk / divider;


    if (bus == SYSCLK_SYSCLK) {
        bus_freq = sysclk;
    }
    else if (bus == SYSCLK_CORE) {
        bus_freq = ahb_freq;
    }
    else if (bus == SYSCLK_BUS_AHB) {
        bus_freq = ahb_freq;
    }
    else if ((bus == SYSCLK_BUS_APB1) || (bus == SYSCLK_BUS_APB2)) {
        // Convert APB divider
        apbx_ppre = (RCC->CFGR & RCC_CFGR_PPRE) >> RCC_CFGR_PPRE_Pos;
        if (apbx_ppre < 0x04) {
            divider = 1;
        }
        // 2, 4, 8, 16
        else {
            divider = 1 << ((apbx_ppre & 0x03) + 1);
        }
        bus_freq = ahb_freq / divider;
    }
    else {
        bus_freq = 0;
    }

    return bus_freq;
}


//  ***************************************************************************
/// @brief      Get PLL bus frequency
/// @param      bus
/// @retval     none
/// @return     PLL bus frequency in Hz
//  ***************************************************************************
static uint32_t sysclk_get_pll_bus_freq(sysclk_bus_t bus) {
    uint32_t pll_src;
    bool is_pll_en;
    uint32_t pll_src_freq;
    uint32_t pll_prediv, pll_mul;
    uint32_t pll_freq_hz;


    // Define PLLs source
    pll_src = (RCC->CFGR & RCC_CFGR_PLLSRC) >> RCC_CFGR_PLLSRC_Pos;
    if (pll_src == PLL_SRC_HSI_DIV2) {
        pll_src_freq = sysclk_get_common_bus_freq(SYSCLK_HSI) / 2;
    }
    else if (pll_src == PLL_SRC_HSI) {
        pll_src_freq = sysclk_get_common_bus_freq(SYSCLK_HSI);
    }
    else if (pll_src == PLL_SRC_HSE) {
        pll_src_freq = sysclk_get_common_bus_freq(SYSCLK_HSE);
    }


    is_pll_en = (RCC->CR & RCC_CR_PLLON) >> RCC_CR_PLLON_Pos;
    if (!is_pll_en) return 0;

    // Get dividers
    pll_prediv = (RCC->CFGR2 & RCC_CFGR2_PREDIV) >> RCC_CFGR2_PREDIV_Pos;
    pll_prediv++;
    pll_mul = (RCC->CFGR & RCC_CFGR_PLLMUL) >> RCC_CFGR_PLLMUL_Pos;
    pll_mul += 2;

    // PLL = (PLLSRC / PLL_PREDIV) * PLL_MUL
    pll_freq_hz = pll_src_freq / pll_prediv;
    pll_freq_hz *= pll_mul;

    return pll_freq_hz;
}
