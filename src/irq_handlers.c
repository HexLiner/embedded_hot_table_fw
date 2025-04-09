//  ***************************************************************************
/// @file    irq_handlers.c
//  ***************************************************************************
#include "irq_handlers.h"
#include "hal/systimer/systimer.h"
#include "mcu_clock.h"
#include "int_adc_driver.h"
#include "usb_cdc.h"
#include "indicators_driver.h"


void irq_handlers_init(void) {
    NVIC_SetPriority(SysTick_IRQn, 1);
    NVIC_SetPriority(RCC_IRQn, 1);
    NVIC_SetPriority(USB_IRQn, 2);
    NVIC_SetPriority(ADC1_IRQn, 3);


    NVIC_EnableIRQ(SysTick_IRQn);
    NVIC_EnableIRQ(ADC1_IRQn);
    NVIC_EnableIRQ(RCC_IRQn);
    NVIC_EnableIRQ(USB_IRQn);
}


void NMI_Handler(void);
void NMI_Handler(void) {

}

void HardFault_Handler(void);
void HardFault_Handler(void) {
    mcu_clock_set_safe_config();
    indicators_init();
    indicators_led_error(true);
    while (1) {
        indicators_process();
    }
}

void SVC_Handler(void);
void SVC_Handler(void) {

}

void PendSV_Handler(void);
void PendSV_Handler(void) {

}

void SysTick_Handler(void);
void SysTick_Handler(void) {
    systimer_handler();
}

void ADC1_IRQHandler(void);
void ADC1_IRQHandler(void) {
    int_adc_handler();
}

void RCC_IRQHandler(void);
void RCC_IRQHandler(void) {
    mcu_clock_hse_error_handler();
}

void USB_IRQHandler(void);
void USB_IRQHandler(void) {
    usb_cdc_handler();
}