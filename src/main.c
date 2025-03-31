#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>
#include "common/error.h"
#include "hal/gpio/gpio.h"
#include "hal/sysclk/sysclk.h"
#include "hal/systimer/systimer.h"
#include "mcu_clock.h"
#include "profiles.h"
#include "cli.h"
#include "cli_cmd.h"
#include "system_operation.h"




uint8_t selected_item = 0;

int main (void) {
    mcu_clock_set_normal_config();
    sysclk_enable_peripheral(GPIOA);
    sysclk_enable_peripheral(GPIOB);
    
    profiles_init();
    cli_cmd_init();
    system_operation_init();
    cli_safe_print("\r\n/E/Hi!\r\n\r\n> ");


    while (1) {
        cli_cmd_process();
        system_operation_process();
    }
}
