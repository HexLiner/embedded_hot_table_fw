#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>
#include "hal/gpio/gpio.h"
#include "hal/sysclk/sysclk.h"
#include "hal/systimer/systimer.h"
#include "mcu_clock.h"




int main (void) {

    mcu_clock_set_normal_config();

    while(1) {

    }

}
