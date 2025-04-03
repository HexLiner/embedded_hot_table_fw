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
#include "error_handling.h"


/*
* PC скрипт для задания профилей
* интерфейс задания профилей (универсальный с fotolight)

* форматированный вывод CLI
* редизайн буферов CLI

* опрос внутреннего датчика температуры
* доделать драйвер АЦП
    * конфиг вычисления опорного напряжения
    * конфиг кол-во активных каналов
    * медианный фильтр
    
* перенос светодиода процесса на пин USB_PUP

* редизайн драйвера flash
* добавить CRC в карту памяти

*+ обработчик ошибок
*+ экран кода ошибки в GUI

* приоритеты IRQ

* тестовые CLI команды для управления нагревателем с логгером работы нагревателя
    * пререход в тестовый режим - clidbg en
    * запустить логгер с выводом в консоль (текущая температура, нагреватель вкл/выкл) - tlog PERIOD_MS
    * установить температуру - tset TEMPERATURE_C
    * установить режим вентилятора - fset PERIOD_S DUTY_CYCLE_PCT
*/


uint8_t selected_item = 0;

int main (void) {
    error_handling_init();
    
    mcu_clock_set_normal_config();
    sysclk_enable_peripheral(GPIOA);
    sysclk_enable_peripheral(GPIOB);
    
    profiles_init();
    cli_cmd_init();
    system_operation_init();
    cli_safe_print("\r\n/E/ Hi! /\r\n\r\n> ");


    while (1) {
        cli_cmd_process();
        system_operation_process();
    }
}
