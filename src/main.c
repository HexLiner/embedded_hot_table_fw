#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>
#include "hal/gpio/gpio.h"
#include "hal/sysclk/sysclk.h"
#include "hal/systimer/systimer.h"
#include "mcu_clock.h"
#include "hal/i2c/i2c_driver.h"
////#include "ssd1306.h"


#include "usbd_core.h"
#include "stm32f0xx_hal_pcd.h"
#include "usbd_desc.h"
#include "usbd_cdc.h"
#include "usbd_cdc_if_template.h"


USBD_HandleTypeDef USBD_Device;

extern PCD_HandleTypeDef hpcd;



#define SSD1306_SLAVE_ADDR (0b01111000)  // or 0b01111010  - 0 1 1 1 1 0 SA0 R/W# 

handle_t i2c_handle;
i2c_settings_t i2c_settings = {
    .speed_hz = 400000,
    .scl_pin = PA1,
    .sda_pin = PA0
};



int main (void) {

    mcu_clock_set_normal_config();
    sysclk_enable_peripheral(GPIOA);
    sysclk_enable_peripheral(GPIOB);
    sysclk_enable_peripheral(SYSCFG);
    SYSCFG->CFGR1 |= 1 << SYSCFG_CFGR1_PA11_PA12_RMP_Pos;
    delay_ms(2000);



    USBD_Init(&USBD_Device, &VCP_Desc, 0);

    // Add Supported Class
    USBD_RegisterClass(&USBD_Device, &USBD_CDC);

    // Add CDC Interface Clas
    USBD_CDC_RegisterInterface(&USBD_Device, &USBD_CDC_Template_fops);

    // Start Device Process
    USBD_Start(&USBD_Device);
    
    while (1);









/*
    delay_ms(1000);
    i2c_init(&i2c_handle, NULL, &i2c_settings);
    ssd1306_init(i2c_handle, SSD1306_SLAVE_ADDR, 1000);
    delay_ms(1000);
    ssd1306_clear();
    delay_ms(1000);
    ssd1306_print_str("Hallo wrld", SSD1306_FOUNT_MODE_K2, 0, 0);*/

    


    /*
    sysclk_enable_peripheral(GPIOA);
    
    // TIM14_CH1
    sysclk_enable_peripheral(TIM14);
    TIM14->CCMR1 = 6 << TIM_CCMR1_OC1M_Pos; //  PWM mode 1 - Channel 1 is active as long as TIMx_CNT < TIMx_CCR1 else inactive
    TIM14->CCER = 1 << TIM_CCER_CC1E_Pos;
    TIM14->PSC = 0;
    TIM14->ARR = 6857;   // 7 KHz
    TIM14->CCR1 = 6857 / 2;
    TIM14->CR1 = TIM_CR1_CEN;

    timer_t timer;
    uint32_t state = 0;
    uint32_t i;
    timer = timer_start_ms(1000);
    while(1) {
        if (timer_triggered(timer)) {
            timer = timer_start_ms(1000);
            if (state == 0) {
                state++;
                gpio_config_pins(PA4, GPIO_MODE_OUTPUT_PP, GPIO_PULL_NONE, GPIO_SPEED_HIGH, 0, true);
            }
            else if (state == 1) {
                state++;
                gpio_reset_pins(PA4);
            }
            else if (state == 2) {
                state++;
                gpio_config_pins(PA4, GPIO_MODE_ALT_FUNCTION_PP, GPIO_PULL_NONE, GPIO_SPEED_HIGH, 4, false);
            }
            else if (state == 3) {
                state++;
                TIM14->CCMR1 = 4 << TIM_CCMR1_OC1M_Pos;
            }
            else if (state == 4) {
                state++;
                TIM14->CCMR1 = 6 << TIM_CCMR1_OC1M_Pos;
            }
            else if (state == 5) {
                state++;
                TIM14->CCMR1 = 4 << TIM_CCMR1_OC1M_Pos;
            }
            else if (state == 6) {
                state++;
                TIM14->CCMR1 = 6 << TIM_CCMR1_OC1M_Pos;
            }
            else if (state == 7) {
                state++;
                TIM14->CCMR1 = 4 << TIM_CCMR1_OC1M_Pos;
            }
            else if (state == 8) {
                state = 0;
                TIM14->CCMR1 = 6 << TIM_CCMR1_OC1M_Pos;
            }
        }
    }*/

}


void USB_IRQHandler(void);
void USB_IRQHandler(void)
{
  HAL_PCD_IRQHandler(&hpcd);
}