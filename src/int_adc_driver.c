//  ***************************************************************************
/// @file    int_adc_driver.c
//  ***************************************************************************
#include "int_adc_driver.h"


#define ADC_TIMEOUT_MS (20)

#define ADC_VREFINT_CAL_ADDRESS (0x1FFFF7BA)
#define ADC_VDDA_CHARAC_MV      (3300)

#define ADC_TC_CAL1_ADDRESS     (0x1FFFF7B8)
#define ADC_TS_CAL1_TEMP        (30)
#define ADC_TC_AVG_SLOPE_UV_C   (4300)
#define ADC_TC_AVG_SLOPE_CODE   (((uint32_t)ADC_TC_AVG_SLOPE_UV_C * 4096) / 3300000)

static int_adc_channel_t *active_channels[INT_ADC_MAX_CHANNELS_QTY] = {NULL, };
static uint32_t active_channel_index, active_channels_qty;


static bool int_adc_calibration(void);
static bool int_adc_enable(void);
static void int_adc_disable(void);




void int_adc_init(int_adc_clk_src_t clk_src, int_adc_sample_rate_t smp_rate) {
    sysclk_enable_peripheral(ADC1);

    ADC1->CFGR1 = (1 << ADC_CFGR1_CONT_Pos)    |   // Mode: 1 - Continuous conversion mode
                  (0 << ADC_CFGR1_RES_Pos)     |   // Data resolution: 00 - 12 bits
                  (0 << ADC_CFGR1_SCANDIR_Pos) |   // Upward scan: 0 - from CHSEL0 to CHSEL17
                  (0 << ADC_CFGR1_DMAEN_Pos);
    ADC1->CFGR2 = clk_src << ADC_CFGR2_CKMODE_Pos;
    ADC1->SMPR = smp_rate << ADC_SMPR_SMP_Pos;
    ADC1->CHSELR = 0;
    ADC1_COMMON->CCR = 0;

    active_channel_index = 0;
    active_channels_qty = 0;

    int_adc_calibration();
    int_adc_enable();

    // Enable ADC IRQ
    ADC1->ISR = 0xFFFF;   // Clear flags
    ADC1->IER = 0;
}


void int_adc_handler(void) {
    uint32_t status;
    volatile uint16_t adc_dr_skip;


    status = ADC1->ISR;
    if (status & ADC_ISR_EOC) {
        if (active_channels[active_channel_index]->samples_cnt < active_channels[active_channel_index]->samples_qty) {
            active_channels[active_channel_index]->buffer += ADC1->DR;
            active_channels[active_channel_index]->samples_cnt++;
        }
        else {
            adc_dr_skip = ADC1->DR;
        }
        active_channel_index++;
        if (active_channel_index >= active_channels_qty) active_channel_index = 0;
    }
    if (status & ADC_ISR_EOS) {
        active_channel_index = 0;
    }

    ADC1->ISR |= (ADC_ISR_EOS | ADC_ISR_EOC);   // Clear flags
}


void int_adc_add_channel(int_adc_channel_t *int_adc_channel) {
    int32_t i;

    
    if ((ADC1->CR & ADC_CR_ADSTART) != 0) ERROR_FATAL(int_adc_add_channel, __LINE__);   // ADC must be stopped!
    if (active_channels[INT_ADC_MAX_CHANNELS_QTY - 1] != NULL) ERROR_FATAL(int_adc_add_channel, __LINE__);

    // sort from min to max
    for (i = (INT_ADC_MAX_CHANNELS_QTY - 1); i >= 0; i--) {
        if (active_channels[i] == NULL) continue;
        if (active_channels[i]->channel_number > int_adc_channel->channel_number) {
            active_channels[i + 1] = active_channels[i];
        }
        else {
            active_channels[i] = int_adc_channel;
            break;
        }
    }
    if (i == -1) active_channels[0] = int_adc_channel;

    if (int_adc_channel->channel_number == INT_ADC_TEMPERATURE_CHANNEL) {
        ADC1_COMMON->CCR |= ADC_CCR_TSEN;
    }
    if (int_adc_channel->channel_number == INT_ADC_VREFINT_CHANNEL) {
        ADC1_COMMON->CCR |= ADC_CCR_VREFEN;
    }

    int_adc_channel->buffer = 0;
    int_adc_channel->samples_cnt = 0;
    ADC1->CHSELR |= 1 << int_adc_channel->channel_number;
    active_channels_qty++;
}


uint16_t int_adc_calc_vdda(uint16_t vref_data_raw) {
    const uint16_t *vrefint_cal = (uint16_t*)ADC_VREFINT_CAL_ADDRESS;
    uint16_t vdda_mv;

    if (vref_data_raw == 0) return 0;
    vdda_mv = ((uint32_t)*vrefint_cal * ADC_VDDA_CHARAC_MV) / vref_data_raw;
    return vdda_mv;
}


uint16_t int_adc_calc_tc(uint16_t tc_data_raw, uint16_t vdda_mv) {
    const uint16_t *tc_cal1 = (uint16_t*)ADC_TC_CAL1_ADDRESS;
    int32_t temperature_c;


    // Temperature_C = ((Sense_DATA - TS_CAL1) / Avg_Slope_Code) + TS_CAL1_TEMP
    // Avg_Slope_Code =  Avg_Slope * 4096 / 3300
    // Sense_DATA = TS_DATA * VDDA / 3.3
    
    temperature_c = ((uint32_t)tc_data_raw * vdda_mv) / 3300;   // Sense_DATA
    temperature_c -= *tc_cal1;
    temperature_c /= ADC_TC_AVG_SLOPE_CODE;
    temperature_c += ADC_TS_CAL1_TEMP;
    return temperature_c;
}


void int_adc_start_continuous_converts(void) {
    active_channel_index = 0;
    ADC1->ISR |= (ADC_ISR_EOS | ADC_ISR_EOC);   // Clear flags
    ADC1->IER |= (ADC_IER_EOSIE | ADC_IER_EOCIE);
    ADC1->CR |= ADC_CR_ADSTART;
}


void int_adc_stop_continuous_converts(void) {
    ADC1->IER &= ~(ADC_IER_EOSIE | ADC_IER_EOCIE);
    ADC1->CR |= ADC_CR_ADSTP;
}


bool int_adc_is_raw_data_ready(int_adc_channel_t *int_adc_channel, uint16_t *data_raw) {
    if (int_adc_channel->samples_cnt < int_adc_channel->samples_qty) return false;
    int_adc_channel->buffer = int_adc_channel->buffer / int_adc_channel->samples_qty;
    *data_raw = (uint16_t)int_adc_channel->buffer;

    int_adc_channel->buffer = 0;
    int_adc_channel->samples_cnt = 0;
    return true;
}


bool int_adc_is_voltage_data_ready(int_adc_channel_t *int_adc_channel, uint16_t *data_mv, uint16_t vdda_mv) {
    uint16_t data_raw;


    if (!int_adc_is_raw_data_ready(int_adc_channel, &data_raw)) return false;
    *data_mv = ((uint32_t)data_raw * vdda_mv) / 4095;

    return true;
}




static bool int_adc_calibration(void) {
    timer_t adc_timeout;

    if ((ADC1->CR & ADC_CR_ADEN) != 0) return false;   // ADC must be disabled!
    if ((ADC1->CFGR1 & ADC_CFGR1_DMAEN) != 0) return false;   // ADC DMA must be disabled!
    
    ADC1->CR |= ADC_CR_ADCAL;
    adc_timeout = timer_start_ms(ADC_TIMEOUT_MS);
    while ((ADC1->CR & ADC_CR_ADCAL) != 0) {
        if (timer_triggered(adc_timeout)) return false;
    }

    return true;
}


static bool int_adc_enable(void) {
    timer_t adc_timeout;

    ADC1->ISR |= ADC_ISR_ADRDY;   // Clear ready flag
    ADC1->CR |= ADC_CR_ADEN;
    adc_timeout = timer_start_ms(ADC_TIMEOUT_MS);
    while ((ADC1->ISR & ADC_ISR_ADRDY) == 0) {
        if (timer_triggered(adc_timeout)) return false;
    }
    return true;
}


static void int_adc_disable(void) {
    ADC1->CR |= ADC_CR_ADSTP;
    ADC1->CR |= ADC_CR_ADDIS;
}
