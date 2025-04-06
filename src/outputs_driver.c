//  ***************************************************************************
/// @file    outputs_driver.c
//  ***************************************************************************
#include "outputs_driver.h"


#define FUN_PIN                                (PA2)
#define HEATER_PIN                             (PA3)
#define HEATER_TEMPERATURE_SENSOR_PIN          (PA5)
#define HEATER_TEMPERATURE_SENSOR_ADC_CHANNEL  (5)

#define MCU_TEMPERATURE_MAX_C           (85 + 10)


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


#define HEATER_ON gpio_set_pins(HEATER_PIN); is_heater_pin_en = true;
#define HEATER_OFF gpio_reset_pins(HEATER_PIN); is_heater_pin_en = false;

uint32_t heater_active_time_ms = 1 * 1000;
uint32_t heater_delay_time_ms = 10 * 1000;
uint8_t heater_hist_on_c = 0;
uint8_t heater_hist_off_c = 5;

typedef enum {
    HEATER_STATE_IDLE = 0,
    HEATER_STATE_EN_ACTIVE,
    HEATER_STATE_EN_INACTIVE,
    HEATER_STATE_EN_OVERTEMP
} heater_state_t;

static heater_state_t heater_state;
static timer_t heater_timer;
uint8_t heater_current_temperature_c;
bool is_heater_pin_en;
static uint8_t mcu_current_temperature_c;
static uint8_t heater_target_temperature_c;

static int_adc_channel_t int_adc_channel_mcu_temp_sensor = {
    .channel_number = INT_ADC_TEMPERATURE_CHANNEL,
    .samples_qty = 100
};
static int_adc_channel_t int_adc_channel_heater_temp_sensor = {
    .channel_number = HEATER_TEMPERATURE_SENSOR_ADC_CHANNEL,
    .samples_qty = 20
};
static int_adc_channel_t int_adc_channel_vrefint = {
    .channel_number = INT_ADC_VREFINT_CHANNEL,
    .samples_qty = 100
};


void outputs_init(void) {
    gpio_config_pins(FUN_PIN, GPIO_MODE_OUTPUT_PP, GPIO_PULL_NONE, GPIO_SPEED_LOW, 0, false);
    gpio_config_pins(HEATER_PIN, GPIO_MODE_OUTPUT_PP, GPIO_PULL_NONE, GPIO_SPEED_LOW, 0, false);

    gpio_config_pins(HEATER_TEMPERATURE_SENSOR_PIN, GPIO_MODE_ANALOG, GPIO_PULL_NONE, GPIO_SPEED_LOW, 0, false);
    int_adc_init(INT_ADC_CLK_SRC_PCLK_DIV_4, INT_ADC_SAMPLE_RATE_239_5_ADC_CLOCK_CYCLE);
    int_adc_add_channel(&int_adc_channel_mcu_temp_sensor);
    int_adc_add_channel(&int_adc_channel_heater_temp_sensor);
    int_adc_add_channel(&int_adc_channel_vrefint);
    int_adc_start_continuous_converts();
    heater_current_temperature_c = HEATER_MAX_TEMP_C;
    is_heater_pin_en = false;

    fun_state = FUN_STATE_IDLE;
    heater_state = HEATER_STATE_IDLE;
}


void meas_process(void) {
    static uint16_t adc_vdd_mv = 0;
    uint16_t adc_raw, adc_mv;


    // Vdda
    if (int_adc_is_raw_data_ready(&int_adc_channel_vrefint, &adc_raw)) {
        adc_vdd_mv = int_adc_calc_vdda(adc_raw);
    }

    if (adc_vdd_mv > 0) {
        // Heater temperature
        if (int_adc_is_voltage_data_ready(&int_adc_channel_heater_temp_sensor, &adc_mv, adc_vdd_mv)) {
            heater_current_temperature_c = adc_mv / 10;
        }

        // MCU temperature
        if (int_adc_is_raw_data_ready(&int_adc_channel_mcu_temp_sensor, &adc_raw)) {
            mcu_current_temperature_c = int_adc_calc_tc(adc_raw, adc_vdd_mv);
            if (mcu_current_temperature_c > MCU_TEMPERATURE_MAX_C) eh_set_fail_mcu_overtemperature();
        }
    }
}


void outputs_process(void) {
    
    meas_process();

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
            if (heater_current_temperature_c > (heater_target_temperature_c + heater_hist_on_c)) {
                HEATER_OFF;
                heater_state = HEATER_STATE_EN_OVERTEMP;
            }
            else if (timer_triggered(heater_timer)) {
                HEATER_OFF;
                heater_timer = timer_restart_ms(heater_timer, heater_delay_time_ms);
                heater_state = HEATER_STATE_EN_INACTIVE;
            }
            break;


        case HEATER_STATE_EN_INACTIVE:
            if (heater_current_temperature_c > (heater_target_temperature_c + heater_hist_on_c)) {
                HEATER_OFF;
                heater_state = HEATER_STATE_EN_OVERTEMP;
            }
            else if (timer_triggered(heater_timer)) {
                HEATER_ON;
                heater_timer = timer_restart_ms(heater_timer, heater_active_time_ms);
                heater_state = HEATER_STATE_EN_ACTIVE;
            }
            break;


        case HEATER_STATE_EN_OVERTEMP:
            if (heater_current_temperature_c < (heater_target_temperature_c - heater_hist_off_c)) {
                HEATER_ON;
                heater_timer = timer_start_ms(heater_active_time_ms);
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
    if (target_temperature_c == 0) heater_dis();
    if (target_temperature_c > HEATER_MAX_TEMP_C) target_temperature_c = HEATER_MAX_TEMP_C;

    heater_target_temperature_c = target_temperature_c;
    heater_timer = timer_start_ms(heater_active_time_ms);
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
        fun_dis();
    }
}


void fun_dis(void) {
    FUN_OFF;
    fun_state = FUN_STATE_IDLE;
}
