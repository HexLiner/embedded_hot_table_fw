//  ***************************************************************************
/// @file    int_adc_driver.h
/// @brief   Internal ADC driver
//  ***************************************************************************
#ifndef _INT_ADC_DRIVER_H_
#define _INT_ADC_DRIVER_H_

#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include "common/mcu.h"
#include "common/error.h"
#include "hal/sysclk/sysclk.h"
#include "hal/systimer/systimer.h"


#define INT_ADC_MAX_CHANNELS_QTY (3)

#define INT_ADC_TEMPERATURE_CHANNEL (16)
#define INT_ADC_VREFINT_CHANNEL     (17)


typedef struct {
    // Public
    uint8_t channel_number;
    uint8_t samples_qty;
    // Private
    uint32_t buffer;
    uint32_t samples_cnt;
} int_adc_channel_t;

typedef enum {
    INT_ADC_CLK_SRC_ADCCLK     = 0,
    INT_ADC_CLK_SRC_PCLK_DIV_2 = 1,
    INT_ADC_CLK_SRC_PCLK_DIV_4 = 2
} int_adc_clk_src_t;

typedef enum {
    INT_ADC_SAMPLE_RATE_1_5_ADC_CLOCK_CYCLE   = 0,
    INT_ADC_SAMPLE_RATE_7_5_ADC_CLOCK_CYCLE   = 1,
    INT_ADC_SAMPLE_RATE_13_5_ADC_CLOCK_CYCLE  = 2,
    INT_ADC_SAMPLE_RATE_28_5_ADC_CLOCK_CYCLE  = 3,
    INT_ADC_SAMPLE_RATE_41_5_ADC_CLOCK_CYCLE  = 4,
    INT_ADC_SAMPLE_RATE_55_5_ADC_CLOCK_CYCLE  = 5,
    INT_ADC_SAMPLE_RATE_71_5_ADC_CLOCK_CYCLE  = 6,
    INT_ADC_SAMPLE_RATE_239_5_ADC_CLOCK_CYCLE = 7
} int_adc_sample_rate_t;


extern void int_adc_init(int_adc_clk_src_t clk_src, int_adc_sample_rate_t smp_rate);
extern void int_adc_add_channel(int_adc_channel_t *int_adc_channel);
extern uint16_t int_adc_calc_vdda(uint16_t vref_data_raw);
extern uint16_t int_adc_calc_tc(uint16_t tc_data_raw, uint16_t vdda_mv);
extern void int_adc_start_continuous_converts(void);
extern void int_adc_stop_continuous_converts(void);
extern bool int_adc_is_raw_data_ready(int_adc_channel_t *int_adc_channel, uint16_t *data_raw);
extern bool int_adc_is_voltage_data_ready(int_adc_channel_t *int_adc_channel, uint16_t *data_mv, uint16_t vdda_mv);


#endif   // _INT_ADC_DRIVER_H_
