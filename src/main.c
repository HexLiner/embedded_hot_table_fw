#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>
#include "common/error.h"
#include "hal/gpio/gpio.h"
#include "hal/sysclk/sysclk.h"
#include "hal/systimer/systimer.h"
#include "irq_handlers.h"
#include "mcu_clock.h"
#include "profiles.h"
#include "cli.h"
#include "cli_cmd.h"
#include "system_operation.h"
#include "error_handling.h"
#include "registers.h"
#include "flash.h"


/*
* редизайн буферов CLI

* медианный фильтр в драйвер АЦП

*/


uint8_t selected_item = 0;

int main (void) {
    irq_handlers_init();

    error_handling_init();
    
    mcu_clock_set_normal_config();
    sysclk_enable_peripheral(GPIOA);
    sysclk_enable_peripheral(GPIOB);
    
    flash_init();
    regs_init();
    profiles_init();
    cli_cmd_init();
    system_operation_init();
    cli_safe_printf("\r\n/E/ Hi! /\r\n\r\n> ");

    delay_ms(1000);


    while (1) {
        regs_process();
        cli_cmd_process();
        system_operation_process();
    }
}
