#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>
#include "common/error.h"
#include "hal/gpio/gpio.h"
#include "hal/sysclk/sysclk.h"
#include "hal/systimer/systimer.h"
#include "mcu_clock.h"
#include "hal/i2c/i2c_driver.h"
#include "ssd1306.h"
#include "usb_cdc.h"
#include "cli.h"


#define SSD1306_SLAVE_ADDR (0b01111000)  // or 0b01111010  - 0 1 1 1 1 0 SA0 R/W# 

handle_t i2c_handle;
i2c_settings_t i2c_settings = {
    .speed_hz = 400000,
    .scl_pin = PA1,
    .sda_pin = PA0
};


error_t cli_cmd_lcd(uint32_t argc, const uint8_t **argv, cli_call_state_t state);
error_t cli_cmd_lcdp(uint32_t argc, const uint8_t **argv, cli_call_state_t state);



const cli_cmd_t cli_cmds[] = {
    {
        .name = "lcd",
        .usage = "lcd TEXT",
        .funk = cli_cmd_lcd
    },
    {
        .name = "lcdp",
        .usage = "lcdp QTY",
        .funk = cli_cmd_lcdp
    }
};


int main (void) {
    mcu_clock_set_normal_config();
    sysclk_enable_peripheral(GPIOA);
    sysclk_enable_peripheral(GPIOB);
    
    usb_cdc_init();
    cli_init(usb_cdc_send_data, usb_cdc_receive_data, cli_cmds, 2);
    delay_ms(5000);

    cli_safe_print("Hallo USB CLI!yhutrhgbdgbgbogjbgjbdgbjfgibjidfjbiodjf5t54y54yyyyyyyy45y45y54y45y54y54y45y45y54y45y45y45y45y54y45y54y5666\r\n");

    i2c_init(&i2c_handle, NULL, &i2c_settings);
    ssd1306_init(i2c_handle, SSD1306_SLAVE_ADDR, 1000);
    ssd1306_clear();

    while (1) {
        cli_process();
    }









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


error_t cli_cmd_lcd(uint32_t argc, const uint8_t **argv, cli_call_state_t state) {
    ssd1306_clear();
    if (argc > 1) {
        ssd1306_print_str(argv[1], SSD1306_FOUNT_MODE_K2, 0, 0);
    }
    else if (argc > 2) {
        cli_print("Incorrect arg!\r\n");
    }

    return E_OK;
}

error_t cli_cmd_lcdp(uint32_t argc, const uint8_t **argv, cli_call_state_t state) {
    static uint16_t cnt;
    static int32_t qty;
    static timer_t timer;


    if (state == CLI_CALL_FIRST) {
        if (argc == 2) {
            if (cli_string_to_digit(argv[1], &qty) != E_OK) {
                cli_print("Incorrect arg!\r\n");
                return E_OK;
            }
        }
        else {
            cli_print("Incorrect arg!\r\n");
            return E_OK;
        }

        ssd1306_clear();
        cnt = 0;
        ssd1306_print_digit(cnt, 3, SSD1306_FOUNT_MODE_K2, 0, 0);
        timer = timer_start_ms(500);
        return E_ASYNC_WAIT;
    }
    else if (state == CLI_CALL_REPEATED) {
        if (timer_triggered(timer)) {
            cnt++;
            ssd1306_print_digit(cnt, 3, SSD1306_FOUNT_MODE_K2, 0, 0);
            timer = timer_start_ms(500);
            if (cnt >= qty) return E_OK;
        }
        return E_ASYNC_WAIT;
    }
    else {
        ssd1306_clear();
        return E_OK;
    }

    return E_OK;
}
