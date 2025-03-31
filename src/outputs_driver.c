//  ***************************************************************************
/// @file    outputs_driver.c
//  ***************************************************************************
#include "outputs_driver.h"
#include "int_adc_driver.h"   ////


#define FUN_PIN    (PA2)
#define HEATER_PIN (PA3)


#define FUN_ON gpio_set_pins(FUN_PIN)
#define FUN_OFF gpio_reset_pins(FUN_PIN)

typedef enum {
    FUN_STATE_IDLE = 0,
    FUN_STATE_EN_ACTIVE,
    FUN_STATE_EN_INACTIVE
} fun_state_t;

static fun_state_t fun_state;
static timer_t fun_timer;
static uint32_t fun_en_time_ms, fun_dis_time_ms;


////#define MAX_HEATE_ON_STAGE_TIME_MS ((uint32_t)(5 * 60 * 1000))
#define HEATER_ACTIVE_TIME_MS      (1 * 1000)
#define HEATER_DELAY_TIME_MS       (10 * 1000)
#define HEATER_MAX_TEMP_C          (200)
#define HEATER_TEMP_OFFSET_C       (7)
#define HEATER_HYST_ON_C           (0)
#define HEATER_HYST_OFF_C          (5)
#define HEATER_ON gpio_set_pins(HEATER_PIN)
#define HEATER_OFF gpio_reset_pins(HEATER_PIN)

typedef enum {
    HEATER_STATE_IDLE = 0,
    HEATER_STATE_EN_ACTIVE,
    HEATER_STATE_EN_INACTIVE,
    HEATER_STATE_EN_OVERTEMP
} heater_state_t;

static heater_state_t heater_state;
static timer_t heater_timer;
uint8_t heater_current_temperature_c;
static uint8_t heater_target_temperature_c;


static int_adc_channel_t int_adc_channel = {
    .channel_number = 5,
    .samples_qty = 20
};


void outputs_init(void) {
    gpio_config_pins(FUN_PIN, GPIO_MODE_OUTPUT_PP, GPIO_PULL_NONE, GPIO_SPEED_LOW, 0, false);
    gpio_config_pins(HEATER_PIN, GPIO_MODE_OUTPUT_PP, GPIO_PULL_NONE, GPIO_SPEED_LOW, 0, false);

    fun_state = FUN_STATE_IDLE;
    heater_state = HEATER_STATE_IDLE;


    int_adc_init(INT_ADC_CLK_SRC_PCLK_DIV_4, INT_ADC_SAMPLE_RATE_71_5_ADC_CLOCK_CYCLE);
    int_adc_add_channel(&int_adc_channel);
    int_adc_start_continuous_converts();
}


void outputs_process(void) {
    uint16_t adc_result, adc_mv;
    if (int_adc_is_data_ready(&int_adc_channel, &adc_result)) {
        adc_mv = ((uint32_t)adc_result * 3300) / 4095;
        heater_current_temperature_c = adc_mv / 10;
    }



    switch (fun_state) {
        case FUN_STATE_IDLE:
            break;

        case FUN_STATE_EN_ACTIVE:
            if (timer_triggered(fun_timer)) {
                FUN_OFF;
                fun_timer = timer_restart_ms(fun_timer, fun_dis_time_ms);
                fun_state = FUN_STATE_EN_INACTIVE;
            }
            break;

        case FUN_STATE_EN_INACTIVE:
            if (timer_triggered(fun_timer)) {
                FUN_ON;
                fun_timer = timer_restart_ms(fun_timer, fun_en_time_ms);
                fun_state = FUN_STATE_EN_ACTIVE;
            }
            break;
        
        default:
            FUN_OFF;
            fun_state = FUN_STATE_IDLE;
            break;
    }


    switch (heater_state) {
        case HEATER_STATE_IDLE:
            break;


        case HEATER_STATE_EN_ACTIVE:
            if (heater_current_temperature_c > (heater_target_temperature_c + HEATER_HYST_ON_C - HEATER_TEMP_OFFSET_C)) {
                HEATER_OFF;
                heater_state = HEATER_STATE_EN_OVERTEMP;
            }
            else if (timer_triggered(heater_timer)) {
                HEATER_OFF;
                heater_timer = timer_restart_ms(heater_timer, HEATER_DELAY_TIME_MS);
                heater_state = HEATER_STATE_EN_INACTIVE;
            }
            ////
            /*
            else if (timer_triggered(heater_on_stage_timer)) {
                HEATER_OFF;
                PORTB &= ~(1 << 0);
                // Err code
                PORTB |= (0b111 << 1);
                PORTB &= ~(2 << 1);
                heater_state = SYS_STATE_FATAL_HEATER_ERR;
            }
            */
            break;


        case HEATER_STATE_EN_INACTIVE:
            if (heater_current_temperature_c > (heater_target_temperature_c + HEATER_HYST_ON_C - HEATER_TEMP_OFFSET_C)) {
                HEATER_OFF;
                heater_state = HEATER_STATE_EN_OVERTEMP;
            }
            else if (timer_triggered(heater_timer)) {
                HEATER_ON;
                heater_timer = timer_restart_ms(heater_timer, HEATER_ACTIVE_TIME_MS);
                heater_state = HEATER_STATE_EN_ACTIVE;
            }
            ////
            /*
            else if (timer_triggered(heater_on_stage_timer)) {
                HEATER_OFF;
                PORTB &= ~(1 << 0);
                // Err code
                PORTB |= (0b111 << 1);
                PORTB &= ~(2 << 1);
                heater_state = SYS_STATE_FATAL_HEATER_ERR;
            }
            */
            break;


        case HEATER_STATE_EN_OVERTEMP:
            if (heater_current_temperature_c < (heater_target_temperature_c - HEATER_HYST_OFF_C - HEATER_TEMP_OFFSET_C)) {
                HEATER_ON;
                ////heater_on_stage_timer = timer_start_ms(MAX_HEATE_ON_STAGE_TIME_MS);
                heater_timer = timer_start_ms(HEATER_ACTIVE_TIME_MS);
                heater_state = HEATER_STATE_EN_ACTIVE;
            }
            break;


        default:
            HEATER_OFF;
            heater_state = HEATER_STATE_IDLE;
            break;
    }
}


void heater_en(uint8_t target_temperature_c) {
    if (target_temperature_c > HEATER_MAX_TEMP_C) target_temperature_c = HEATER_MAX_TEMP_C;

    heater_target_temperature_c = target_temperature_c;
    ////heater_on_stage_timer = timer_start_ms(MAX_HEATE_ON_STAGE_TIME_MS);
    heater_timer = timer_start_ms(HEATER_ACTIVE_TIME_MS);
    HEATER_ON;
    heater_state = HEATER_STATE_EN_ACTIVE;
}


void heater_dis(void) {
    HEATER_OFF;
    heater_state = HEATER_STATE_IDLE;
}


void fun_en(uint32_t period_s, uint8_t duty_cycle_pct) {
    if (duty_cycle_pct > 100) duty_cycle_pct = 100;

    if ((period_s > 0) && (duty_cycle_pct > 0)) {
        fun_en_time_ms = period_s * duty_cycle_pct;
        fun_en_time_ms = fun_en_time_ms * 10;   // /100*1000 -> *10
        fun_dis_time_ms = period_s * 1000;
        fun_dis_time_ms -= fun_en_time_ms;

        FUN_ON;
        fun_timer = timer_start_ms(fun_en_time_ms);
        fun_state = FUN_STATE_EN_ACTIVE;
    }
    else {
        FUN_OFF;
        fun_state = FUN_STATE_IDLE;
    }
}


void fun_dis(void) {
    FUN_OFF;
    fun_state = FUN_STATE_IDLE;
}
