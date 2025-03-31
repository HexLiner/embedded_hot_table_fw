//  ***************************************************************************
/// @file    int_adc_driver.c
//  ***************************************************************************
#include "int_adc_driver.h"


#define ADC_TIMEOUT_MS (20)


static int_adc_channel_t *int_adc_channels[17];
static uint32_t active_channel;


static bool int_adc_calibration(void);
static bool int_adc_enable(void);
static void int_adc_disable(void);




void int_adc_init(int_adc_clk_src_t clk_src, int_adc_sample_rate_t smp_rate) {
    uint32_t i;


    sysclk_enable_peripheral(ADC1);

    ADC1->CFGR1 = (1 << ADC_CFGR1_CONT_Pos)    |   // Mode: 1 - Continuous conversion mode
                      (0 << ADC_CFGR1_RES_Pos)     |   // Data resolution: 00 - 2 bits
                      (0 << ADC_CFGR1_SCANDIR_Pos) |   // Upward scan (from CHSEL0 to CHSEL17)
                      (0 << ADC_CFGR1_DMAEN_Pos);
    ADC1->CFGR2 = clk_src << ADC_CFGR2_CKMODE_Pos;
    ADC1->SMPR = smp_rate << ADC_SMPR_SMP_Pos;

    ADC1->CHSELR = 0;
    for (i = 0; i < 17; i++) int_adc_channels[i] = NULL;
    active_channel = 0;

    int_adc_calibration();
    int_adc_enable();

    // Enable ADC IRQ
    ADC1->ISR = 0xFFFF;   // Clear flags
    ADC1->IER = 0;
    NVIC_EnableIRQ(ADC1_IRQn);

}


void int_adc_add_channel(int_adc_channel_t *int_adc_channel) {
    if ((ADC1->CR & ADC_CR_ADSTART) != 0) return;   // ADC must be stopped!
    int_adc_channels[int_adc_channel->channel_number] = int_adc_channel;
    ADC1->CHSELR |= 1 << int_adc_channel->channel_number;
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


bool int_adc_is_data_ready(int_adc_channel_t *int_adc_channel, uint16_t *data) {
    if (int_adc_channel->samples_cnt < int_adc_channel->samples_qty) return false;
    int_adc_channel->buffer = int_adc_channel->buffer / int_adc_channel->samples_qty;
    *data = (uint16_t)int_adc_channel->buffer;

    int_adc_channel->buffer = 0;
    int_adc_channel->samples_cnt = 0;
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


    status = ADC1->ISR;
    if (status & ADC_ISR_EOC) {
        while (int_adc_channels[active_channel] == NULL) {
            active_channel++;
        }
        if (int_adc_channels[active_channel]->samples_cnt < int_adc_channels[active_channel]->samples_qty) {
            int_adc_channels[active_channel]->buffer += ADC1->DR;
            int_adc_channels[active_channel]->samples_cnt++;
        }
        active_channel++;
        if (active_channel >= 17) active_channel = 0;
    }
    if (status & ADC_ISR_EOS) {
        active_channel = 0;
    }

    ADC1->ISR |= (ADC_ISR_EOS | ADC_ISR_EOC);   // Clear flags
}
