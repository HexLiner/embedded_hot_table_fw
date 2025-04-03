//  ***************************************************************************
/// @file    int_adc_driver.c
//  ***************************************************************************
#include "int_adc_driver.h"


#define ADC_TIMEOUT_MS (20)

#define ADC_VREFINT_CAL_ADDRESS (0x1FFFF7BA)
#define ADC_VDDA_CHARAC_MV      (3300)
#define ADC_TEMPERATURE_CHANNEL (16)
#define ADC_VREFINT_CHANNEL     (17)

static uint16_t int_adc_ref_mv;
static int_adc_channel_t *int_adc_channels[18];
static uint32_t active_channel, active_channels_qty;
static int_adc_channel_t int_adc_channel_vrefint_channel;


static bool int_adc_calibration(void);
static bool int_adc_enable(void);
static void int_adc_disable(void);




void int_adc_init(int_adc_clk_src_t clk_src, int_adc_sample_rate_t smp_rate) {
    uint32_t i;


    sysclk_enable_peripheral(ADC1);

    ADC1->CFGR1 = (1 << ADC_CFGR1_CONT_Pos)    |   // Mode: 1 - Continuous conversion mode
                  (0 << ADC_CFGR1_RES_Pos)     |   // Data resolution: 00 - 12 bits
                  (0 << ADC_CFGR1_SCANDIR_Pos) |   // Upward scan: 0 - from CHSEL0 to CHSEL17
                  (0 << ADC_CFGR1_DMAEN_Pos);
    ADC1->CFGR2 = clk_src << ADC_CFGR2_CKMODE_Pos;
    ADC1->SMPR = smp_rate << ADC_SMPR_SMP_Pos;
    ADC1_COMMON->CCR = (0 << ADC_CCR_TSEN_Pos) |
                       (1 << ADC_CCR_VREFEN_Pos);

    ADC1->CHSELR = 0;
    for (i = 0; i < 18; i++) int_adc_channels[i] = NULL;
    active_channel = 0;
    active_channels_qty = 0;

    int_adc_channel_vrefint_channel.channel_number = ADC_VREFINT_CHANNEL;
    int_adc_channel_vrefint_channel.samples_qty = 100;
    int_adc_add_channel(&int_adc_channel_vrefint_channel);

    int_adc_calibration();
    int_adc_ref_mv = ADC_VDDA_CHARAC_MV;   ////
    int_adc_enable();

    // Enable ADC IRQ
    ADC1->ISR = 0xFFFF;   // Clear flags
    ADC1->IER = 0;
    NVIC_EnableIRQ(ADC1_IRQn);
}


void int_adc_add_channel(int_adc_channel_t *int_adc_channel) {
    int32_t i;

    
    if ((ADC1->CR & ADC_CR_ADSTART) != 0) return;   // ADC must be stopped!
    if (int_adc_channels[17] != NULL) return;    // busy

    // sort from min to max
    for (i = 17; i >= 0; i--) {
        if (int_adc_channels[i] == NULL) continue;
        if (int_adc_channels[i]->channel_number > int_adc_channel->channel_number) {
            int_adc_channels[i + 1] = int_adc_channels[i];
        }
        else {
            int_adc_channels[i] = int_adc_channel;
            break;
        }
    }
    if (i == -1) int_adc_channels[0] = int_adc_channel;

    int_adc_channel->buffer = 0;
    int_adc_channel->samples_cnt = 0;
    ADC1->CHSELR |= 1 << int_adc_channel->channel_number;
    active_channels_qty++;
}


void int_adc_start_continuous_converts(void) {
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


bool int_adc_is_voltage_data_ready(int_adc_channel_t *int_adc_channel, uint16_t *data_mv) {
    uint16_t data_raw;


    if (!int_adc_is_raw_data_ready(int_adc_channel, &data_raw)) return false;
    *data_mv = ((uint32_t)data_raw * int_adc_ref_mv) / 4095;

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




void ADC1_IRQHandler(void);
void ADC1_IRQHandler(void) {
    uint32_t status;
    volatile uint16_t adc_dr_skip;
    const uint16_t *vrefint_cal = (uint16_t*)ADC_VREFINT_CAL_ADDRESS;


    status = ADC1->ISR;
    if (status & ADC_ISR_EOC) {
        if (int_adc_channels[active_channel]->samples_cnt < int_adc_channels[active_channel]->samples_qty) {
            int_adc_channels[active_channel]->buffer += ADC1->DR;
            int_adc_channels[active_channel]->samples_cnt++;
        }
        active_channel++;
        if (active_channel >= active_channels_qty) active_channel = 0;

        if (int_adc_is_raw_data_ready(&int_adc_channel_vrefint_channel, &int_adc_ref_mv)) {
            int_adc_ref_mv = ((uint32_t)*vrefint_cal * ADC_VDDA_CHARAC_MV) / int_adc_ref_mv;
        }
    }
    if (status & ADC_ISR_EOS) {
        active_channel = 0;
    }

    ADC1->ISR |= (ADC_ISR_EOS | ADC_ISR_EOC);   // Clear flags
}
